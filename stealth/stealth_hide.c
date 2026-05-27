#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/filesystem.h"
#include "../include/exfat_parser.h"
#include "../crypto/aes.h"

typedef struct {
    char magic[4];
    int size;
} slack_header;

extern char selected_partition[64];


void stealth_hide(char *secret, char *target)
{
    if (strlen(selected_partition) == 0)
    {
        printf("[-] No partition selected\n\n");
        return;
    }

    printf("\n[+] Stealth hide using exFAT parser\n\n");

    exfat_info fs;

    if (parse_exfat(selected_partition, &fs) != 0)
    {
        printf("Filesystem parsing failed\n");
        return;
    }

    /* open secret file */

    FILE *secret_fp = fopen(secret, "rb");

    if (!secret_fp)
    {
        printf("Cannot open secret file\n");
        return;
    }

    fseek(secret_fp, 0, SEEK_END);
    long secret_size = ftell(secret_fp);
    rewind(secret_fp);

    unsigned char *buffer = malloc(secret_size);

    fread(buffer, 1, secret_size, secret_fp);
    fclose(secret_fp);

    /* AES encryption */

    unsigned char key[32] =
        "SlackForgeEncryptionKey123456";

    unsigned char iv[16] =
        "SlackForgeInitV";

    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, buffer, secret_size);

    /* payload header */

    slack_header header;

    memcpy(header.magic, "SLKF", 4);
    header.size = secret_size;

    int payload_size = sizeof(header) + secret_size;

    /* open disk */

    int fd = open(selected_partition, O_RDWR);

    if (fd < 0)
    {
        printf("Cannot open disk device\n");
        free(buffer);
        return;
    }

    printf("Searching slack space in target file: %s\n\n", target);

    uint32_t start_cluster = 0;
    uint64_t file_size = 0;

    uint64_t root_offset =
        exfat_cluster_to_offset(&fs, fs.root_dir_cluster);

    lseek(fd, root_offset, SEEK_SET);

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
            /* stream extension */

            if (read(fd, stream, 32) != 32)
                break;

            uint32_t cluster =
                *(uint32_t *)(stream + 20);

            uint64_t size =
                *(uint64_t *)(stream + 24);

            /* filename entry */

            if (read(fd, name_entry, 32) != 32)
                break;

            int name_len = 0;

            for (int i = 2; i < 32; i += 2)
            {
                if (name_entry[i] == 0)
                    break;

                filename[name_len++] = name_entry[i];
            }

            filename[name_len] = '\0';

            if (strcmp(filename, target) == 0)
            {
                start_cluster = cluster;
                file_size = size;
                printf("Target file located: %s\n", filename);
                break;
            }
        }
    }

    if (start_cluster == 0)
    {
        printf("Target file not found in filesystem\n");
        close(fd);
        free(buffer);
        return;
    }

    uint32_t last_cluster =
        get_last_cluster_exfat(selected_partition,
                               &fs,
                               start_cluster);

    uint64_t cluster_offset =
        exfat_cluster_to_offset(&fs, last_cluster);

    uint64_t cluster_size =
        fs.bytes_per_sector *
        fs.sectors_per_cluster;

    uint64_t cluster_end =
        cluster_offset + cluster_size;

    uint64_t slack_size =
        cluster_size - (file_size % cluster_size);

    if (slack_size == cluster_size)
        slack_size = 0;

    if (payload_size > slack_size)
    {
        printf("Secret too large for slack space\n");
        close(fd);
        free(buffer);
        return;
    }

    uint64_t slack_offset =
        cluster_end - slack_size;

    /* write payload */

    pwrite(fd, &header,
           sizeof(header),
           slack_offset);

    pwrite(fd, buffer,
           secret_size,
           slack_offset + sizeof(header));

    printf("\n[+] Payload hidden successfully\n");
    printf("[+] Slack offset: %lu\n", slack_offset);
    printf("[+] Payload size: %ld bytes\n\n", secret_size);

    close(fd);
    free(buffer);
}
