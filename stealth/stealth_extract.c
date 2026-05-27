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

void stealth_extract(char *target)
{
            exfat_info fs;
parse_exfat(selected_partition, &fs);
    if (strlen(selected_partition) == 0)
    {
        printf("[-] No partition selected\n\n");
        return;
    }

    printf("\n[+] Stealth extraction using exFAT parser\n\n");

    
    

    if (parse_exfat(selected_partition, &fs) != 0)
    {
        printf("Filesystem parsing failed\n");
        return;
    }

    int fd = open(selected_partition, O_RDONLY);

    if (fd < 0)
    {
        printf("Cannot open disk device\n");
        return;
    }

    uint64_t root_offset =
        fs.cluster_heap_offset * fs.bytes_per_sector;

    lseek(fd, root_offset, SEEK_SET);

    unsigned char entry[32];

    uint32_t start_cluster = 0;
    uint64_t file_size = 0;

    /* scan directory entries */
    while (read(fd, entry, 32) == 32)
    {
        if (entry[0] == 0x00)
            break;

        if (entry[0] == 0x85)
        {
            start_cluster =
                *(uint32_t *)(entry + 20);

            file_size =
                *(uint64_t *)(entry + 24);

            break;
        }
    }

    if (start_cluster == 0)
    {
        printf("File entry not found\n");
        close(fd);
        return;
    }

   

   uint32_t last_cluster =
      get_last_cluster_exfat(selected_partition,
                           &fs,
                           start_cluster);

    uint64_t cluster_offset =
        exfat_cluster_to_offset(&fs,
                                last_cluster);

    uint64_t cluster_size =
        fs.bytes_per_sector *
        fs.sectors_per_cluster;

    uint64_t cluster_end =
        cluster_offset + cluster_size;

    uint64_t slack_size =
        cluster_size - (file_size % cluster_size);

    if (slack_size == cluster_size)
        slack_size = 0;

    uint64_t slack_offset =
        cluster_end - slack_size;

    slack_header header;

    pread(fd,
          &header,
          sizeof(header),
          slack_offset);

    if (strncmp(header.magic, "SLKF", 4) != 0)
    {
        printf("No hidden payload detected\n");
        close(fd);
        return;
    }

    unsigned char *buffer =
        malloc(header.size);

    pread(fd,
          buffer,
          header.size,
          slack_offset + sizeof(header));

    /* AES decrypt */

    unsigned char key[32] =
        "SlackForgeEncryptionKey123456";

    unsigned char iv[16] =
        "SlackForgeInitV";

    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);

    AES_CBC_decrypt_buffer(&ctx,
                           buffer,
                           header.size);

    FILE *out =
        fopen("recovered_secret.bin", "wb");

    fwrite(buffer, 1,
           header.size,
           out);

    fclose(out);

    free(buffer);

    close(fd);

    printf("\n[+] Payload extracted successfully\n");
    printf("[+] Saved as recovered_secret.bin\n\n");
}
