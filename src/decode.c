#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"
#include "crypto.h"


/* Extract one byte from 8 image bytes */
unsigned char decode_byte_from_lsb(
    unsigned char *image_buffer)
{
    unsigned char data = 0;

    for (int i = 0; i < 8; i++)
    {
        data =
            (data << 1) |
            (image_buffer[i] & 1);
    }

    return data;
}


/* Extract 32-bit integer */
unsigned int decode_int_from_lsb(
    FILE *stego_fp)
{
    unsigned int data = 0;

    unsigned char image_buffer[8];

    for (int byte = 0; byte < 4; byte++)
    {
        if (fread(image_buffer,
                  1,
                  8,
                  stego_fp) != 8)
        {
            printf(
                "Error: Not enough image data.\n"
            );

            return 0;
        }


        unsigned char decoded_byte =
            decode_byte_from_lsb(
                image_buffer
            );


        data =
            (data << 8) |
            decoded_byte;
    }

    return data;
}


void decode(void)
{
    char stego_filename[100];

    char password[100];

    FILE *stego_fp;

    unsigned char header[54];

    unsigned char image_buffer[8];


    printf("\nEnter stego BMP filename: ");

    scanf("%99s", stego_filename);


    printf("Enter password: ");

    scanf("%99s", password);


    /* Open stego image */

    stego_fp =
        fopen(stego_filename, "rb");


    if (stego_fp == NULL)
    {
        printf(
            "Error: Could not open stego image.\n"
        );

        return;
    }


    /* Read BMP header */

    if (fread(header, 1, 54, stego_fp) != 54)
    {
        printf("Error: Invalid BMP file.\n");

        fclose(stego_fp);

        return;
    }


    /* Check BMP */

    if (header[0] != 'B' ||
        header[1] != 'M')
    {
        printf(
            "Error: Not a valid BMP image.\n"
        );

        fclose(stego_fp);

        return;
    }


    /* Pixel data offset */

    long pixel_offset =
        header[10] |
        (header[11] << 8) |
        (header[12] << 16) |
        (header[13] << 24);


    /* Check 24-bit BMP */

    int bits_per_pixel =
        header[28] |
        (header[29] << 8);


    if (bits_per_pixel != 24)
    {
        printf(
            "Error: Only 24-bit BMP images are supported.\n"
        );

        fclose(stego_fp);

        return;
    }


    printf("\nValid 24-bit BMP image detected!\n");

    printf(
        "Pixel data offset: %ld bytes\n",
        pixel_offset
    );


    /* Move to pixel data */

    if (fseek(stego_fp,
              pixel_offset,
              SEEK_SET) != 0)
    {
        printf(
            "Error: Could not access pixel data.\n"
        );

        fclose(stego_fp);

        return;
    }


    /* Read message length */

    unsigned int message_length =
        decode_int_from_lsb(
            stego_fp
        );


    printf(
        "Hidden message length: %u bytes\n",
        message_length
    );


    if (message_length == 0)
    {
        printf(
            "No hidden message found.\n"
        );

        fclose(stego_fp);

        return;
    }


    /* Allocate memory */

    unsigned char *message =
        malloc(message_length);


    if (message == NULL)
    {
        printf(
            "Error: Memory allocation failed.\n"
        );

        fclose(stego_fp);

        return;
    }


    /* Extract encrypted message */

    for (unsigned int i = 0;
         i < message_length;
         i++)
    {
        if (fread(image_buffer,
                  1,
                  8,
                  stego_fp) != 8)
        {
            printf(
                "Error: Image does not contain enough data.\n"
            );

            free(message);

            fclose(stego_fp);

            return;
        }


        message[i] =
            decode_byte_from_lsb(
                image_buffer
            );
    }


    /* Decrypt */

    xor_encrypt_decrypt(
        message,
        message_length,
        password
    );


    printf(
        "\nDecrypted message:\n"
    );


    for (unsigned int i = 0;
         i < message_length;
         i++)
    {
        printf("%c", message[i]);
    }


    printf("\n");


    free(message);

    fclose(stego_fp);
}
