#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/disk.h"

char selected_partition[64] = "";

void partition_list() {

    if (strlen(selected_disk) == 0) {

        printf("[-] No disk selected\n\n");
        return;
    }

    char command[256];
    sprintf(command, "lsblk -ln %s", selected_disk);

    FILE *fp = popen(command, "r");

    if (!fp) {
        printf("Error reading partitions\n\n");
        return;
    }

    char line[256];
    int id = 1;

    printf("\nID   Partition\n");
    printf("-------------------\n");

    while (fgets(line, sizeof(line), fp)) {

        char name[64];

        sscanf(line, "%s", name);

        if (strcmp(name, selected_disk + 5) != 0) {

            printf("%d    /dev/%s\n", id, name);
            id++;
        }
    }

    printf("\n");

    pclose(fp);
}

void partition_select(int id) {

    if (strlen(selected_disk) == 0) {

        printf("[-] No disk selected\n\n");
        return;
    }

    char command[256];
    sprintf(command, "lsblk -ln %s", selected_disk);

    FILE *fp = popen(command, "r");

    if (!fp) {
        printf("Error reading partitions\n\n");
        return;
    }

    char line[256];
    int current = 1;

    while (fgets(line, sizeof(line), fp)) {

        char name[64];
        sscanf(line, "%s", name);

        if (strcmp(name, selected_disk + 5) != 0) {

            if (current == id) {

                sprintf(selected_partition, "/dev/%s", name);

                printf("\n[+] Selected partition: %s\n\n", selected_partition);

                pclose(fp);
                return;
            }

            current++;
        }
    }

    printf("[-] Invalid partition ID\n\n");

    pclose(fp);
}

void fs_info() {

    if (strlen(selected_partition) == 0) {

        printf("[-] No partition selected\n\n");
        return;
    }

    char command[256];
    sprintf(command, "lsblk -f %s", selected_partition);

    FILE *fp = popen(command, "r");

    if (!fp) {

        printf("Error detecting filesystem\n\n");
        return;
    }

    char line[256];

    printf("\nFilesystem information\n");
    printf("------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {

        printf("%s", line);
    }

    printf("\n");

    pclose(fp);
}
