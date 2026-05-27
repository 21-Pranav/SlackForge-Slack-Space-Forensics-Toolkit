#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "../include/exfat_parser.h"


/* ------------------------------------------------ */
/* Parse exFAT boot sector                          */
/* ------------------------------------------------ */

int parse_exfat(const char *device, exfat_info *fs)
{
    int fd = open(device, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return -1;
    }

    unsigned char boot[512];

    if (read(fd, boot, 512) != 512)
    {
        printf("Boot sector read failed\n");
        close(fd);
        return -1;
    }

    fs->bytes_per_sector = 1 << boot[108];
    fs->sectors_per_cluster = 1 << boot[109];

    fs->fat_offset = *(uint32_t *)(boot + 80);
    fs->cluster_heap_offset = *(uint32_t *)(boot + 88);
    fs->root_dir_cluster = *(uint32_t *)(boot + 96);

    printf("Filesystem: exFAT\n");
    printf("Bytes per sector: %u\n", fs->bytes_per_sector);
    printf("Sectors per cluster: %u\n", fs->sectors_per_cluster);
    printf("FAT offset: %u\n", fs->fat_offset);
    printf("Cluster heap offset: %u\n", fs->cluster_heap_offset);
    printf("Root directory cluster: %u\n", fs->root_dir_cluster);

    close(fd);

    return 0;
}


/* ------------------------------------------------ */
/* Convert cluster → disk offset                    */
/* ------------------------------------------------ */

uint64_t exfat_cluster_to_offset(exfat_info *fs, uint32_t cluster)
{
    uint64_t cluster_size =
        fs->bytes_per_sector * fs->sectors_per_cluster;

    return (cluster - 2) * cluster_size +
           ((uint64_t)fs->cluster_heap_offset *
            fs->bytes_per_sector);
}


/* ------------------------------------------------ */
/* Follow FAT chain                                 */
/* ------------------------------------------------ */

uint32_t get_last_cluster_exfat(const char *device,
                                exfat_info *fs,
                                uint32_t start_cluster)
{
    int fd = open(device, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return 0;
    }

    uint32_t cluster = start_cluster;
    uint32_t next;

    uint64_t fat_offset =
        (uint64_t)fs->fat_offset * fs->bytes_per_sector;

    while (1)
    {
        off_t entry_offset =
            fat_offset + (cluster * 4);

        lseek(fd, entry_offset, SEEK_SET);

        if (read(fd, &next, 4) != 4)
        {
            printf("FAT read error\n");
            break;
        }

        if (next == 0xFFFFFFFF || next == 0xFFFFFFF8)
            break;

        cluster = next;
    }

    close(fd);

    return cluster;
}


/* ------------------------------------------------ */
/* Recursive directory scan                         */
/* ------------------------------------------------ */

void scan_directory_recursive(const char *device,
                              exfat_info *fs,
                              uint32_t dir_cluster)
{
    int fd = open(device, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return;
    }

    uint64_t cluster_size =
        fs->bytes_per_sector * fs->sectors_per_cluster;

    uint64_t dir_offset =
        exfat_cluster_to_offset(fs, dir_cluster);

    lseek(fd, dir_offset, SEEK_SET);

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

            int name_len = 0;

            for (int i = 2; i < 32; i += 2)
            {
                if (name_entry[i] == 0)
                    break;

                filename[name_len++] = name_entry[i];
            }

            filename[name_len] = '\0';

            uint8_t flags = stream[1];

            if (flags & 0x10)
            {
                /* directory */

                printf("Directory: %s\n", filename);

                scan_directory_recursive(device,
                                         fs,
                                         start_cluster);
            }
            else
            {
                /* file */

                uint32_t last_cluster =
                    get_last_cluster_exfat(device,
                                           fs,
                                           start_cluster);

                uint64_t cluster_offset =
                    exfat_cluster_to_offset(fs,
                                            last_cluster);

                uint64_t slack_size =
                    cluster_size -
                    (file_size % cluster_size);

                if (slack_size == cluster_size)
                    slack_size = 0;

                printf("File: %s\n", filename);
                printf("Start cluster: %u\n", start_cluster);
                printf("File size: %lu\n", file_size);
                printf("Slack size: %lu bytes\n\n", slack_size);
            }
        }
    }

    close(fd);
}


/* ------------------------------------------------ */
/* Root scan entry point                            */
/* ------------------------------------------------ */

void scan_directory_entries_exfat(const char *device,
                                  exfat_info *fs)
{
    printf("\nSlack Scan Report\n");
    printf("----------------------------------\n\n");

    scan_directory_recursive(device,
                             fs,
                             fs->root_dir_cluster);
}
