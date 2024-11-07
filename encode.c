#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
long image_size;
uint get_image_size_for_bmp(FILE *fptr_image)
{
    printf("INFO: Checking for source image capacity to handle secret.txt\n");
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    fseek(fptr_image, 0, SEEK_SET);

    // Return image capacity
    //printf("%u", width * height * 3);
    image_size = width * height * 3;
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    else
        printf("INFO: Opened %s\n",encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    else
        printf("INFO: Opened %s\n",encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    else
        printf("INFO: Opened %s\n",encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    char *ext = strrchr(argv[2], '.');

    if (ext != NULL && !(strcmp(ext, ".bmp")))
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("\nInfo : Error -Invalid Img File\n");
        return e_failure;
    }

    ext = strrchr(argv[3], '.');

    if (ext != NULL)
    {
        if (!(strcmp(ext, ".txt")))
        {
            strcpy(encInfo->secret_file_extn, ".txt");
            encInfo->size_sec_file_ext = 4;
        }
        else if (!(strcmp(ext, ".c")))
        {
            strcpy(encInfo->secret_file_extn, ".c");
            encInfo->size_sec_file_ext = 2;
        }
        else if (!(strcmp(ext, ".csv")))
        {
            strcpy(encInfo->secret_file_extn, ".csv");
            encInfo->size_sec_file_ext = 4;
        }
        else if (!(strcmp(ext, ".sh")))
        {
            strcpy(encInfo->secret_file_extn, ".sh");
            encInfo->size_sec_file_ext = 3;
        }
        else
        {
            printf("\nInfo : Error -Invalid Msg File\n");
            return e_failure;
        }

        encInfo->secret_fname = argv[3];
    }
    
    if (argc >= 5 && argv[4] != NULL)
    {
        ext = strrchr(argv[4], '.');

        if (ext != NULL && !(strcmp(ext, ".bmp")))
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("\nInfo : Error -Invalid Output File\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Output File not mentioned. Creating default_steg.bmp as default\n");
        encInfo->stego_image_fname = "default_steg.bmp";
        encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
        fclose(encInfo->fptr_stego_image);
    }

    return e_success;
}

long long data_size;

Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for secret.txt size\n");

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(encInfo->size_secret_file !=0)
        printf("INFO: Done. Not Empty\n");
    else
        printf("\nError: Secret file is empty\n");
    
    int img_size = get_image_size_for_bmp(encInfo->fptr_src_image);

    data_size = 54 + (4 * 8) + (encInfo->size_secret_file * 8) + (4 * 8) + (strlen(MAGIC_STRING) * 8) + (strlen(encInfo->secret_file_extn) * 8);

    // printf("\nData size is : %lld\n",data_size);
    if (img_size > data_size)
        return e_success;
    else
        return e_failure;
}

uint get_file_size(FILE *fptr_file)
{
    fseek(fptr_file, 0, SEEK_END);

    long file_size = ftell(fptr_file);

    fseek(fptr_file, 0, SEEK_SET);

    return file_size;
}

Status copy_bmp_header(FILE *src, FILE *dest)
{
    char buffer[54];

    int read = fread(buffer, 1, 54, src);

    if (read != 54)
        return e_failure;

    int write = fwrite(buffer, 1, 54, dest);

    if (write != 54)
        return e_failure;

    return e_success;
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

    unsigned char buffer[size * 8];

    int read = fread(buffer, 1, (size * 8), fptr_src_image);

    if (read != (size * 8))
        return e_failure;

    {
        int m = 0;

        for (int j = 0; j < (size); j++)
        {
            for (int i = 0; i < 8; i++)
            {
                buffer[m] &= ~(1);

                int k = (data[j] >> (7 - i)) & 1;

                buffer[m++] |= k;
            }
        }
    }

    int write = fwrite(buffer, 1, (size * 8), fptr_stego_image);

    if (write != (size * 8))
        return e_failure;

    return e_success;
}

Status encode_magic_string(const char *magic, EncodeInfo *encInfo)
{
    if (encode_data_to_image(magic, strlen(magic), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
}

Status encode_secret_file_ext_size(long file_size, EncodeInfo *encInfo)
{
    unsigned char buffer[sizeof(int) * 8];

    int read = fread(buffer, 1, (sizeof(int) * 8), encInfo->fptr_src_image);

    if (read != (sizeof(int) * 8))
        return e_failure;

    {
        int m = 0;

        for (int i = 0; i < 32; i++)
        {
            buffer[m] &= ~(1);

            int k = (file_size >> (31 - i)) & 1;

            buffer[m++] |= k;
        }
    }

    int write = fwrite(buffer, 1, (sizeof(int) * 8), encInfo->fptr_stego_image);

    if (write != (sizeof(int) * 8))
        return e_failure;

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    unsigned char buffer[sizeof(int) * 8];

    int read = fread(buffer, 1, (sizeof(int) * 8), encInfo->fptr_src_image);

    if (read != (sizeof(int) * 8))
        return e_failure;

    {
        int m = 0;

        for (int i = 0; i < 32; i++)
        {
            buffer[m] &= ~(1);

            int k = (file_size >> (31 - i)) & 1;

            buffer[m++] |= k;
        }
    }

    int write = fwrite(buffer, 1, (sizeof(int) * 8), encInfo->fptr_stego_image);

    if (write != (sizeof(int) * 8))
        return e_failure;

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int size = get_file_size(encInfo->fptr_secret);
    char file_data[size];

    { // copying secret data
        int read = fread(file_data, 1, size, encInfo->fptr_secret);

        if (read != (size))
            return e_failure;
    }

    if (encode_data_to_image(file_data, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Magic String Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding secret.txt File Extenstion Size\n");
    if (encode_secret_file_ext_size(encInfo->size_sec_file_ext, encInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Extenstion Size Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding secret.txt File Extenstion\n");
    if (encode_secret_file_extn(encInfo->secret_file_extn, encInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Extenstion Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding secret.txt File Size\n");
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: File Size Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Encoding secret.txt File Data\n");
    if (encode_secret_file_data(encInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: File Data Encoding Failed\n");
        return e_failure;
    }

    printf("INFO: Copying Left Over Data\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Copying Left Over Data Failed\n");
        return e_failure;
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *src, FILE *dest)
{
    // long cur_pos = ftell(src);

    // long long remain = image_size - data_size;//get_image_size_for_bmp(src) - cur_pos;

    // printf("\nBefore = %ld\n",ftell(src));

    // //fseek(src, data_size-1, SEEK_SET);

    // printf("\nAfter = %ld\n",ftell(src));

    // unsigned char buffer[remain];

    // // if (buffer == NULL)
    // // {
    // //     perror("malloc");
    // //     return e_failure;
    // // }

    // int read = fread(buffer, 1, remain, src);

    // if(read != remain)
    //     return e_failure;

    // int write = fwrite(buffer, 1, remain, dest);

    // if(write != remain)
    //     return e_failure;

    // printf("\nOutput file size : %ld",ftell(dest));

    // //free(buffer);

    
    char ch;
    while (fread(&ch, 1, 1, src))
        fwrite(&ch, 1, 1, dest);
    return e_success;
}
