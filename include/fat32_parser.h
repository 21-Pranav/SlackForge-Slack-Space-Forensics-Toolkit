#ifndef FAT32_PARSER_H
#define FAT32_PARSER_H

#include <stdint.h>

typedef struct
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint32_t reserved_sectors;
    uint32_t fat_size;
} fat32_info;

int parse_fat32(const char *device, fat32_info *fs);

#endif
