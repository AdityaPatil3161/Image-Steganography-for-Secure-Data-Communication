#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"
#include "crypto.h"


/* --------------------------------------------------
   Encode one byte into 8 image bytes
   -------------------------------------------------- */
void encode_byte_to_lsb(unsigned char data,
                        unsigned char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        unsigned char bit = (data >> (7 - i)) & 1;

        image_buffer[i] =
            (image_buffer[i] & 0xFE) | bit;
    }
}


/* --------------------------------------------------
   Encode 32-bit integer into image
   -------------------------------------------------- */
void encode_int_to_lsb(unsigned int data,
                       FILE *source_fp,
                       FILE *output_fp)
{
    unsigned char image_buffer[8];

    for (int byte = 3; byte >= 0; byte--)
    {
        unsigned char data_byte =
            (data >> (byte * 8)) & 0xFF;

        if (fread(image_buffer, 1, 8, source_fp) != 8)
        {
            printf("Error: Image does not have enough capacity.\n");
            return;
        }

        encode_byte_to_lsb(data_byte, image_buffer);

        fwrite(image_buffer, 1, 8, output_fp);
    }
}


/* --------------------------------------------------
   Main encode function
   -------------------------------------------------- */
void encode(void)
{
    char source_filename[100];
    char secret_filename[100];
    char output_filename[100];

    char password[100];

    FILE *source_fp;
    FILE *secret_fp;
    FILE *output_fp;

    unsigned char header[54];

    printf("\nEnter BMP image filename: ");
    scanf("%99s", source_filename);

    printf("Enter secret text filename: ");
    scanf("%99s", secret_filename);

    printf("Enter output BMP filename: ");
    scanf("%99s", output_filename);

    printf("Enter password: ");
    scanf("%99s", password);


    /* Open source BMP */
    source_fp = fopen(source_filename, "rb");

    if (source_fp == NULL)
    {
        printf("Error: Could not open BMP image.\n");
        return;
    }


    /* Open secret file */
    secret_fp = fopen(secret_filename, "rb");

    if (secret_fp == NULL)
    {
        printf("Error: Could not open secret file.\n");
        fclose(source_fp);
        return;
    }


    /* Read BMP header */
    if (fread(header, 1, 54, source_fp) != 54)
    {
        printf("Error: Invalid BMP file.\n");

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Check BMP signature */
    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Error: Not a valid BMP image.\n");

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Get pixel data offset */
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
        printf("Error: Only 24-bit BMP images are supported.\n");

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Get image dimensions */
    int width =
        header[18] |
        (header[19] << 8) |
        (header[20] << 16) |
        (header[21] << 24);


    int height =
        header[22] |
        (header[23] << 8) |
        (header[24] << 16) |
        (header[25] << 24);


    printf("\nValid 24-bit BMP image detected!\n");

    printf("Pixel data offset: %ld bytes\n",
           pixel_offset);

    printf("Image width  : %d pixels\n",
           width);

    printf("Image height : %d pixels\n",
           height);


    /* Get secret file size */
    fseek(secret_fp, 0, SEEK_END);

    long secret_size = ftell(secret_fp);

    rewind(secret_fp);


    if (secret_size <= 0)
    {
        printf("Error: Secret file is empty.\n");

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    printf("Secret message size: %ld bytes\n",
           secret_size);


    /* Allocate memory */
    unsigned char *message =
        malloc(secret_size);


    if (message == NULL)
    {
        printf("Error: Memory allocation failed.\n");

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Read complete secret message */
    fread(message, 1, secret_size, secret_fp);


    /* Encrypt message */
    xor_encrypt_decrypt(
        message,
        secret_size,
        password
    );


    printf("Message encrypted successfully.\n");


    /* Capacity check */

    long required_bytes =
        32 + (secret_size * 8);

    long available_bytes =
        (long)width * height * 3;


    if (required_bytes > available_bytes)
    {
        printf("Error: Secret message is too large.\n");

        free(message);

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Create output image */

    output_fp =
        fopen(output_filename, "wb");


    if (output_fp == NULL)
    {
        printf("Error: Could not create output image.\n");

        free(message);

        fclose(source_fp);
        fclose(secret_fp);

        return;
    }


    /* Copy BMP data before pixel data */

    rewind(source_fp);

    for (long i = 0; i < pixel_offset; i++)
    {
        int byte = fgetc(source_fp);

        if (byte == EOF)
        {
            printf("Error reading BMP file.");

            free(message);

            fclose(source_fp);
            fclose(secret_fp);
            fclose(output_fp);

            return;
        }

        fputc(byte, output_fp);
    }


    /* Store message length */

    unsigned int message_length =
        (unsigned int)secret_size;


    encode_int_to_lsb(
        message_length,
        source_fp,
        output_fp
    );


    /* Store encrypted message */

    unsigned char image_buffer[8];

    for (long i = 0; i < secret_size; i++)
    {
        if (fread(image_buffer, 1, 8, source_fp) != 8)
        {
            printf("Error: Image does not have enough capacity.\n");

            free(message);

            fclose(source_fp);
            fclose(secret_fp);
            fclose(output_fp);

            return;
        }


        encode_byte_to_lsb(
            message[i],
            image_buffer
        );


        fwrite(
            image_buffer,
            1,
            8,
            output_fp
        );
    }


    /* Copy remaining image data */

    unsigned char buffer[1024];

    size_t bytes_read;


    while ((bytes_read =
            fread(buffer,
                  1,
                  sizeof(buffer),
                  source_fp)) > 0)
    {
        fwrite(
            buffer,
            1,
            bytes_read,
            output_fp
        );
    }


    printf("\nSecret message encoded successfully!\n");

    printf("Output file: %s\n",
           output_filename);


    free(message);

    fclose(source_fp);
    fclose(secret_fp);
    fclose(output_fp);
}
