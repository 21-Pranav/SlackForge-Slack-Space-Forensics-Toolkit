#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "../include/filesystem.h"
#include "../include/exfat_parser.h"

extern char selected_partition[64];


double calculate_entropy(unsigned char *data, int size)
{
    int count[256] = {0};

    for (int i = 0; i < size; i++)
        count[data[i]]++;

    double entropy = 0.0;

    for (int i = 0; i < 256; i++)
    {
        if (count[i] == 0)
            continue;

        double p = (double)count[i] / size;
        entropy -= p * log2(p);
    }

    return entropy;
}


void entropy_scan()
{
    if (strlen(selected_partition) == 0)
    {
        printf("[-] No partition selected\n\n");
        return;
    }

    printf("\nSlack Entropy Scan\n");
    printf("----------------------------------\n\n");

    exfat_info fs;

    if (parse_exfat(selected_partition, &fs) != 0)
    {
        printf("Filesystem parsing failed\n");
        return;
    }

    int fd = open(selected_partition, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return;
    }

    uint64_t cluster_size =
        fs.bytes_per_sector * fs.sectors_per_cluster;

    uint64_t root_offset =
        exfat_cluster_to_offset(&fs, fs.root_dir_cluster);

    lseek(fd, root_offset, SEEK_SET);

    unsigned char entry[32];
    unsigned char stream[32];
    unsigned char name_entry[32];

    char filename[256];

    while (read(fd, entry, 32) == 32)
    {
        if (entry[0] == 0x00)
            break;

        if (entry[0] == 0x85)
        {
            read(fd, stream, 32);

            uint32_t start_cluster =
                *(uint32_t *)(stream + 20);

            uint64_t file_size =
                *(uint64_t *)(stream + 24);

            read(fd, name_entry, 32);

            int len = 0;

            for (int i = 2; i < 32; i += 2)
            {
                if (name_entry[i] == 0)
                    break;

                filename[len++] = name_entry[i];
            }

            filename[len] = '\0';

            uint32_t last_cluster =
                get_last_cluster_exfat(selected_partition,
                                       &fs,
                                       start_cluster);

            uint64_t cluster_offset =
                exfat_cluster_to_offset(&fs,
                                        last_cluster);

            uint64_t slack_size =
                cluster_size - (file_size % cluster_size);

            if (slack_size == cluster_size)
                slack_size = 0;

            if (slack_size == 0)
                continue;

            uint64_t slack_offset =
                cluster_offset + (cluster_size - slack_size);

            unsigned char buffer[4096];

            int read_size = slack_size;

            if (read_size > 4096)
                read_size = 4096;

            pread(fd, buffer, read_size, slack_offset);

            double entropy =
                calculate_entropy(buffer, read_size);

            printf("File: %s\n", filename);
            printf("Slack: %lu bytes\n", slack_size);
            printf("Entropy: %.2f ", entropy);

            if (entropy > 7.5)
                printf("→ suspicious\n\n");
            else
                printf("→ normal\n\n");
        }
    }

    close(fd);
}
