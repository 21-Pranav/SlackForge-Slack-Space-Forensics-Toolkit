#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/disk.h"
#include "../include/filesystem.h"
#include "../include/entropy_scan.h"

/* function declarations */

void slack_scan();
void stealth_hide(char *secret, char *target);
void stealth_extract(char *target);
void slack_carve();

/* ------------------------------------------------ */
/* Banner                                           */
/* ------------------------------------------------ */

void banner() {

    printf("\n");
    printf("  ███████╗██╗      █████╗  ██████╗██╗  ██╗███████╗ ██████╗ ██████╗  ██████╗ ███████╗\n");
    printf("  ██╔════╝██║     ██╔══██╗██╔════╝██║ ██╔╝██╔════╝██╔═══██╗██╔══██╗██╔════╝ ██╔════╝\n");
    printf("  ███████╗██║     ███████║██║     █████╔╝ █████╗  ██║   ██║██████╔╝██║  ███╗█████╗  \n");
    printf("  ╚════██║██║     ██╔══██║██║     ██╔═██╗ ██╔══╝  ██║   ██║██╔══██╗██║   ██║██╔══╝  \n");
    printf("  ███████║███████╗██║  ██║╚██████╗██║  ██╗██║     ╚██████╔╝██║  ██║╚██████╔╝███████╗\n");
    printf("  ╚══════╝╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝      ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝\n");

    printf("\n");
    printf("           SlackForge Console v1.0\n");
    printf("      Advanced Slack Space Forensics Toolkit\n\n");

    printf("           Filesystem Support : exFAT (v1)\n");
    printf("           Developed by Pranav Prajapati\n");

    printf("--------------------------------------------------\n\n");
}

/* ------------------------------------------------ */
/* Command List                                     */
/* ------------------------------------------------ */

void show_commands() {

    printf("Available Commands:\n\n");

    printf(" disk list              - List available disks\n");
    printf(" disk select <id>       - Select a disk\n\n");

    printf(" partition list         - List partitions\n");
    printf(" partition select <id>  - Select partition\n\n");

    printf(" fs info                - Show filesystem info\n\n");

    printf(" stealth hide <secret> <target>   - Hide data in slack\n");
    printf(" stealth extract <target>         - Extract hidden data\n\n");

    printf(" forensic scan          - Scan slack space\n");
    printf(" forensic carve         - Carve hidden slack data\n\n");

    printf(" help                   - Show commands\n");
    printf(" exit                   - Exit SlackForge\n\n");
}

/* ------------------------------------------------ */
/* Main                                             */
/* ------------------------------------------------ */

int main() {

    /* Require root privileges */

    if (geteuid() != 0) {
        printf("SlackForge requires root privileges.\n");
        printf("Run using: sudo ./sf\n");
        return 1;
    }

    char command[256];

    banner();
    show_commands();

    while (1) {

        printf("sf > ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0;

        /* Exit */

        if (strcmp(command, "exit") == 0) {

            printf("\nExiting SlackForge...\n\n");
            break;
        }

        /* Help */

        else if (strcmp(command, "help") == 0) {

            show_commands();
        }

        /* Disk list */

        else if (strcmp(command, "disk list") == 0) {

            list_disks();
        }

        /* Disk select */

        else if (strncmp(command, "disk select", 11) == 0) {

            int id;

            if (sscanf(command, "disk select %d", &id) == 1) {

                select_disk(id);
            }
            else {

                printf("Usage: disk select <id>\n\n");
            }
        }

        /* Partition list */

        else if (strcmp(command, "partition list") == 0) {

            if (strlen(selected_disk) == 0) {
                printf("Select a disk first.\n\n");
                continue;
            }

            partition_list();
        }

        /* Partition select */

        else if (strncmp(command, "partition select", 16) == 0) {

            if (strlen(selected_disk) == 0) {
                printf("Select a disk first.\n\n");
                continue;
            }

            int id;

            if (sscanf(command, "partition select %d", &id) == 1) {

                partition_select(id);
            }
            else {

                printf("Usage: partition select <id>\n\n");
            }
        }

        /* Filesystem info */

        else if (strcmp(command, "fs info") == 0) {

            if (strlen(selected_partition) == 0) {
                printf("Select a partition first.\n\n");
                continue;
            }

            fs_info();
        }

        /* Slack scan */

        else if (strcmp(command, "forensic scan") == 0) {

            if (strlen(selected_partition) == 0) {
                printf("Select a partition first.\n\n");
                continue;
            }

            slack_scan();
        }

        /* Stealth hide */

        else if (strncmp(command, "stealth hide", 12) == 0) {

            if (strlen(selected_partition) == 0) {
                printf("Select a partition first.\n\n");
                continue;
            }

            char secret[128];
            char target[128];

            if (sscanf(command, "stealth hide %127s %127s", secret, target) == 2) {

                stealth_hide(secret, target);
            }
            else {

                printf("Usage: stealth hide <secret_file> <target_file>\n\n");
            }
        }

        /* Stealth extract */

        else if (strncmp(command, "stealth extract", 15) == 0) {

            if (strlen(selected_partition) == 0) {
                printf("Select a partition first.\n\n");
                continue;
            }

            char target[128];

            if (sscanf(command, "stealth extract %127s", target) == 1) {

                stealth_extract(target);
            }
            else {

                printf("Usage: stealth extract <target_file>\n\n");
            }
        }

        /* Forensic carve */

        else if (strcmp(command, "forensic carve") == 0) {

            if (strlen(selected_partition) == 0) {
                printf("Select a partition first.\n\n");
                continue;
            }

            slack_carve();
        }
        else if (strcmp(command, "forensic entropy") == 0)
	{
	    entropy_scan();
	}

        /* Unknown command */

        else {

            printf("Unknown command. Type 'help'.\n\n");
        }
        
    }

    return 0;
}
