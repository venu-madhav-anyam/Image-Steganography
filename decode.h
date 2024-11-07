#ifndef DECODE_H
#define DECODE_H
#include "types.h"

#define MAX_ARGMNTS 4
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    int size_sec_file_ext;
    char secret_file_extn[MAX_FILE_SUFFIX];
    int size_secret_file;

    /* Input Image Info */
    char *input_image_fname;
    FILE *fptr_input_image;

    /*argc and argv*/
    int argc;
    char *argv[MAX_ARGMNTS];
} DecodeInfo;


Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

Status open_files_dec(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(char magic[], DecodeInfo *decInfo);

Status decode_secret_file_ext_size(DecodeInfo *decInfo);

Status Open_sec_file(int argc, DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);



#endif