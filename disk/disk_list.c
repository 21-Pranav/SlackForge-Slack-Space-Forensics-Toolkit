#include <stdio.h>
#include <dirent.h>
#include <string.h>

char selected_disk[64] = "";

void list_disks() {

    struct dirent *entry;
    DIR *dp;

    dp = opendir("/sys/block");

    if (dp == NULL) {
        printf("Error opening /sys/block\n");
        return;
    }

    printf("\n");
    printf("ID   Device      Size(GB)   Removable\n");
    printf("--------------------------------------\n");

    int id = 1;

    while ((entry = readdir(dp))) {

        if (strncmp(entry->d_name, "sd", 2) == 0 ||
            strncmp(entry->d_name, "nvme", 4) == 0) {

            char path[256];
            FILE *fp;

            long sectors = 0;
            int removable = 0;

            /* Read disk size */
            sprintf(path, "/sys/block/%s/size", entry->d_name);

            fp = fopen(path, "r");

            if (fp != NULL) {
                fscanf(fp, "%ld", &sectors);
                fclose(fp);
            }

            double size_gb = (sectors * 512.0) / (1024 * 1024 * 1024);

            /* Read removable flag */
            sprintf(path, "/sys/block/%s/removable", entry->d_name);

            fp = fopen(path, "r");

            if (fp != NULL) {
                fscanf(fp, "%d", &removable);
                fclose(fp);
            }

            printf("%d    /dev/%-6s   %6.2f     %s\n",
                   id,
                   entry->d_name,
                   size_gb,
                   removable ? "Yes" : "No");

            id++;
        }
    }

    printf("\n");

    closedir(dp);
}

void select_disk(int id) {

    struct dirent *entry;
    DIR *dp;

    dp = opendir("/sys/block");

    if (dp == NULL) {
        printf("Error opening /sys/block\n");
        return;
    }

    int current = 1;

    while ((entry = readdir(dp))) {

        if (strncmp(entry->d_name, "sd", 2) == 0 ||
            strncmp(entry->d_name, "nvme", 4) == 0) {

            if (current == id) {

                sprintf(selected_disk, "/dev/%s", entry->d_name);

                printf("\n[+] Selected disk: %s\n\n", selected_disk);

                closedir(dp);
                return;
            }

            current++;
        }
    }

    printf("[-] Invalid disk ID\n\n");

    closedir(dp);
}
