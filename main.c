#include <stdio.h>
#include "encode.h"
#include "decode.h"
int main()
{
    int choice;

    printf("\n");
    printf("====================================\n");
    printf("       IMAGE STEGANOGRAPHY\n");
    printf("====================================\n");
    printf("1. Encode\n");
    printf("2. Decode\n");
    printf("3. Exit\n");
    printf("====================================\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            encode();
            break;

        case 2:
            decode();
            break;

        case 3:
            printf("\nExiting program...\n");
            return 0;

        default:
            printf("\nInvalid choice.\n");
    }

    return 0;
}