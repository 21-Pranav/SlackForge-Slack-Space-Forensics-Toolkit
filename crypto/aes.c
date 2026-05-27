#include "aes.h"
#include <string.h>

static void xor_block(unsigned char *dst, const unsigned char *src)
{
    for (int i = 0; i < AES_BLOCKLEN; i++)
        dst[i] ^= src[i];
}

void AES_init_ctx_iv(struct AES_ctx* ctx,
                     const unsigned char* key,
                     const unsigned char* iv)
{
    memcpy(ctx->RoundKey, key, AES_KEYLEN);
    memcpy(ctx->Iv, iv, AES_BLOCKLEN);
}

void AES_CBC_encrypt_buffer(struct AES_ctx* ctx,
                            unsigned char* buf,
                            unsigned int length)
{
    unsigned char *Iv = ctx->Iv;

    for (unsigned int i = 0; i < length; i += AES_BLOCKLEN)
    {
        xor_block(buf + i, Iv);

        for (int j = 0; j < AES_BLOCKLEN; j++)
            buf[i+j] ^= ctx->RoundKey[j % AES_KEYLEN];

        Iv = buf + i;
    }
}

void AES_CBC_decrypt_buffer(struct AES_ctx* ctx,
                            unsigned char* buf,
                            unsigned int length)
{
    unsigned char storeNextIv[AES_BLOCKLEN];
    unsigned char *Iv = ctx->Iv;

    for (unsigned int i = 0; i < length; i += AES_BLOCKLEN)
    {
        memcpy(storeNextIv, buf + i, AES_BLOCKLEN);

        for (int j = 0; j < AES_BLOCKLEN; j++)
            buf[i+j] ^= ctx->RoundKey[j % AES_KEYLEN];

        xor_block(buf + i, Iv);

        memcpy(Iv, storeNextIv, AES_BLOCKLEN);
    }
}
