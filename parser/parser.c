#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../include/parser.h"
#include "../include/exfat_parser.h"
#include "../include/fat32_parser.h"
#include "../include/ntfs_parser.h"

void detect_filesystem(const char *device)
{
    int fd = open(device, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open device\n");
        return;
    }

    unsigned char boot[512];

    read(fd, boot, 512);

    close(fd);

    if (memcmp(boot + 3, "EXFAT", 5) == 0)
    {
        printf("Filesystem detected: exFAT\n");
        exfat_info fs;
        parse_exfat(device, &fs);
    }
    else if (memcmp(boot + 82, "FAT32", 5) == 0)
    {
        printf("Filesystem detected: FAT32\n");
        fat32_info fs;
        parse_fat32(device, &fs);
    }
    else if (memcmp(boot + 3, "NTFS", 4) == 0)
    {
        printf("Filesystem detected: NTFS\n");
        ntfs_info fs;
        parse_ntfs(device, &fs);
    }
    else
    {
        printf("Unknown filesystem\n");
    }
}
