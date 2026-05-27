#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/ntfs_parser.h"

int parse_ntfs(const char *device, ntfs_info *fs)
{
    int fd = open(device, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return -1;
    }

    unsigned char boot[512];

    read(fd, boot, 512);

    fs->bytes_per_sector = *(unsigned short *)(boot + 11);
    fs->sectors_per_cluster = boot[13];
    fs->mft_cluster = *(unsigned long long *)(boot + 48);

    printf("Bytes per sector: %u\n", fs->bytes_per_sector);
    printf("Sectors per cluster: %u\n", fs->sectors_per_cluster);

    close(fd);

    return 0;
}
