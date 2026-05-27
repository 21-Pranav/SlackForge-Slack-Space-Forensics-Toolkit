#ifndef NTFS_PARSER_H
#define NTFS_PARSER_H

#include <stdint.h>

typedef struct
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint64_t mft_cluster;
} ntfs_info;

int parse_ntfs(const char *device, ntfs_info *fs);

#endif
