#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/filesystem.h"
#include "../include/exfat_parser.h"

typedef struct {
    char magic[4];
    int size;
} slack_header;

extern char selected_partition[64];

void slack_carve()

{
            exfat_info fs;
parse_exfat(selected_partition, &fs);
    if (strlen(selected_partition) == 0)
    {
        printf("[-] No partition selected\n\n");
        return;
    }

    printf("\nSlackForge Forensic Carve\n");
    printf("==========================\n\n");

    

    if (parse_exfat(selected_partition, &fs) != 0)
    {
        printf("Filesystem parsing failed\n");
        return;
    }

    int fd = open(selected_partition, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open disk device\n");
        return;
    }

    uint64_t root_offset =
        fs.cluster_heap_offset * fs.bytes_per_sector;

    lseek(fd, root_offset, SEEK_SET);

    unsigned char entry[32];

    printf("Scanning slack regions for hidden payloads...\n\n");

    while (read(fd, entry, 32) == 32)
    {
        if (entry[0] == 0x00)
            break;

        if (entry[0] == 0x85)
        {
            uint32_t start_cluster =
                *(uint32_t *)(entry + 20);

            uint64_t file_size =
                *(uint64_t *)(entry + 24);



uint32_t last_cluster =
    get_last_cluster_exfat(selected_partition,
                           &fs,
                           start_cluster);

            uint64_t cluster_offset =
                exfat_cluster_to_offset(&fs,
                                        last_cluster);

            uint64_t cluster_size =
                fs.bytes_per_sector *
                fs.sectors_per_cluster;

            uint64_t cluster_end =
                cluster_offset + cluster_size;

            uint64_t slack_size =
                cluster_size - (file_size % cluster_size);

            if (slack_size == cluster_size)
                slack_size = 0;

            if (slack_size <= 0)
                continue;

            uint64_t slack_offset =
                cluster_end - slack_size;

            slack_header header;

            pread(fd,
                  &header,
                  sizeof(header),
                  slack_offset);

            if (strncmp(header.magic, "SLKF", 4) == 0)
            {
                printf("[!] Hidden payload detected\n");
                printf("Start cluster : %u\n", start_cluster);
                printf("Payload size  : %d bytes\n", header.size);
                printf("Slack offset  : %lu\n\n", slack_offset);
            }
        }
    }

    close(fd);

    printf("Carving completed.\n\n");
}
