#ifndef AES_H
#define AES_H

#define AES_BLOCKLEN 16
#define AES_KEYLEN 32
#define AES_keyExpSize 240

struct AES_ctx {
    unsigned char RoundKey[AES_keyExpSize];
    unsigned char Iv[AES_BLOCKLEN];
};

void AES_init_ctx_iv(struct AES_ctx* ctx,
                     const unsigned char* key,
                     const unsigned char* iv);

void AES_CBC_encrypt_buffer(struct AES_ctx* ctx,
                            unsigned char* buf,
                            unsigned int length);

void AES_CBC_decrypt_buffer(struct AES_ctx* ctx,
                            unsigned char* buf,
                            unsigned int length);

#endif
