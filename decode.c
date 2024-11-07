#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"



Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo)
{
    char *ext = strrchr(argv[2], '.');

    if (ext != NULL && !(strcmp(ext, ".bmp")))
    {
        decInfo->input_image_fname = argv[2];
        printf("INFO: Opened %s\n",decInfo->input_image_fname);
    }
    else
    {
        printf("\nError -Invalid Img File\n");
        return e_failure;
    }

    if (argc == 4 && argv[3] != NULL)
            decInfo->secret_fname = argv[3];

    return e_success;
}


Status open_files_dec(DecodeInfo *decInfo)
{
     // Src Image file
    decInfo->fptr_input_image = fopen(decInfo->input_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_input_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->input_image_fname);

        return e_failure;
    }

    if(fseek(decInfo->fptr_input_image,54,SEEK_SET)!=0)
    {
        perror("\nError: fseek failed\n");
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}


Status decode_magic_string(char magic[], DecodeInfo *decInfo)
{
    int size = strlen(magic);
    unsigned char buffer[size * 8];
    char *data = calloc(size,sizeof(char)); 

    int read = fread(buffer, 1, (size * 8), decInfo->fptr_input_image);

    if (read != (size * 8))
        return e_failure;

    {
        int m = 0;

        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < 8; i++)
            {
                buffer[m] &= 1;
                data[j] |= (buffer[m++]<<(7-i));
            }
        }
    }

    if(strcmp(magic,data)==0)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;

}

Status decode_secret_file_ext_size(DecodeInfo *decInfo)
{
    decInfo->size_sec_file_ext = 0;
    unsigned char buffer[sizeof(int) * 8];

    int read = fread(buffer, 1, (sizeof(int) * 8), decInfo->fptr_input_image);

    if (read != (sizeof(int) * 8))
        return e_failure;

    {
        for (int i = 0; i < 32; i++)
        {
            buffer[i] &= 1;
            (decInfo->size_sec_file_ext) |= (buffer[i] << (31 - i));
        }
        
    }

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int size = decInfo->size_sec_file_ext;
    
    unsigned char buffer[size * 8];
    char *data = calloc(size,sizeof(char)); 

    int read = fread(buffer, 1, (size * 8), decInfo->fptr_input_image);

    if (read != (size * 8))
        return e_failure;

    {
        int m = 0;

        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < 8; i++)
            {
                buffer[m] &= 1;
                data[j] |= (buffer[m++]<<(7-i));
            }
        }
    }
    strcpy(decInfo->secret_file_extn, data);

    return e_success;
}


Status Open_sec_file(int argc,DecodeInfo *decInfo)
{
    if(argc < 4)
    {
        char name[] = "default_sec_out";
        strcat(name,decInfo->secret_file_extn);
        decInfo->secret_fname = name;

        printf("INFO: Output File not mentioned. Creating decoded.txt as %s\n",name);
        decInfo->fptr_secret = fopen(name, "w");
        // Do Error handling
        if (decInfo->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "\nERROR: Unable to open file %s\n", decInfo->secret_fname);

            return e_failure;
        }
    }
    else
    {
        strcat(decInfo->secret_fname,decInfo->secret_file_extn);
        decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
        // Do Error handling
        if (decInfo->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "\nERROR: Unable to open file %s\n", decInfo->secret_fname);

            return e_failure;
        }
    }
    
    return e_success;
}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->size_secret_file = 0;
    unsigned char buffer[sizeof(int) * 8];

    int read = fread(buffer, 1, (sizeof(int) * 8), decInfo->fptr_input_image);

    if (read != (sizeof(int) * 8))
        return e_failure;

    {
        for (int i = 0; i < 32; i++)
        {
            buffer[i] &= 1;
            (decInfo->size_secret_file) |= (buffer[i] << (31 - i));
        }
    }

    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int size = decInfo->size_secret_file;

    unsigned char buffer[size * 8];
    char *data = calloc(size,sizeof(char)); 

    int read = fread(buffer, 1, (size * 8), decInfo->fptr_input_image);

    if (read != (size * 8))
        return e_failure;

    {
        int m = 0;

        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < 8; i++)
            {
                buffer[m] &= 1;
                data[j] |= (buffer[m++]<<(7-i));
            }
        }
    }
    
    int write = fwrite(data, 1,size, decInfo->fptr_secret);

    if (write != size)
        return e_failure;

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extenstion Size\n");
    if(decode_secret_file_ext_size(decInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Decoding of File Extension Size Failed\n");
        return e_failure;
    }

    printf("INFO: Decoding Output File Extenstion\n");
    if(decode_secret_file_extn(decInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Decoding of File Extension Failed\n");
        return e_failure;
    }

    
    if(Open_sec_file(decInfo->argc, decInfo) == e_success)
    {
        printf("INFO: Opened %s\n",decInfo->secret_fname);
        printf("INFO Done. Opened all required files\n");
    }
    else
    {
        printf("\nError: Secret File Opening Failed\n");
        return e_failure;
    }

    printf("INFO: Decoding File Size\n");
    if(decode_secret_file_size(decInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Decoding of File Size Failed\n");
        return e_failure;
    }

    printf("INFO: Decoding File Data\n");
    if(decode_secret_file_data(decInfo) == e_success)
        printf("INFO: Done\n");
    else
    {
        printf("\nError: Decoding of File Data Failed\n");
        return e_failure;
    }

    return e_success;
}

