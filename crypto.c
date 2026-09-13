#include <string.h>
#include "crypto.h"

void xor_encrypt_decrypt(unsigned char *data, int length, const char *key)
{
    int key_length = strlen(key);

    if (key_length == 0)
    {
        return;
    }

    for (int i = 0; i < length; i++)
    {
        data[i] = data[i] ^ key[i % key_length];
    }
}