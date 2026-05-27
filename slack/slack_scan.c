#include <stdio.h>
#include <string.h>

#include "../include/filesystem.h"
#include "../include/exfat_parser.h"

extern char selected_partition[64];

void slack_scan()
{
    if (strlen(selected_partition) == 0)
    {
        printf("[-] No partition selected\n\n");
        return;
    }

    printf("\nSlackForge Forensic Scan\n");
    printf("=================================\n\n");

    printf("Device: %s\n\n", selected_partition);

    exfat_info fs;

    /* Parse exFAT boot sector */
    if (parse_exfat(selected_partition, &fs) != 0)
    {
        printf("Filesystem parsing failed\n");
        return;
    }

    printf("\nScanning filesystem...\n");

    /* Scan directory entries */
    scan_directory_entries_exfat(selected_partition, &fs);

    printf("\nScan completed.\n\n");
}
