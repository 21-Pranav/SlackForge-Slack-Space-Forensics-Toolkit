#ifndef SLACKFORGE_EXFAT_PARSER_H
#define SLACKFORGE_EXFAT_PARSER_H

#include <stdint.h>

/*
 * exFAT filesystem information
 * extracted from the boot sector
 */
typedef struct
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;

    uint32_t fat_offset;
    uint32_t cluster_heap_offset;

    uint32_t root_dir_cluster;

} exfat_info;


/*
 * Parse exFAT boot sector
 */
int parse_exfat(const char *device, exfat_info *fs);


/*
 * Convert cluster number → disk offset
 */
uint64_t exfat_cluster_to_offset(exfat_info *fs, uint32_t cluster);


/*
 * Follow FAT chain to locate last cluster
 */
uint32_t get_last_cluster_exfat(const char *device, exfat_info *fs, uint32_t start_cluster);


/*
 * Scan root directory entries
 */
void scan_directory_entries_exfat(const char *device, exfat_info *fs);

void scan_directory_recursive(const char *device,
                              exfat_info *fs,
                              uint32_t dir_cluster);

#endif
