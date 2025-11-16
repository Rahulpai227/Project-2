#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

#define MAX_MAGIC_STRING 20
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    
    /* Output file info */
    char output_fname[100];   // will append proper extension

    /* Magic string */
    char magic_string[MAX_MAGIC_STRING];

    /* Secret file extension */
    char extn_secret_file[MAX_FILE_SUFFIX];
    int extn_size;

    /* Secret file size */
    long size_secret_file;

} DecodeInfo;

/* Function prototypes */
OperationType check_operation_type(char *argv[]);
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

/*Rename to avoid conflict with encode.h */
Status open_decode_files(DecodeInfo *decInfo);

Status skip_bmp_header(FILE *fptr_src);
void decode_byte_from_lsb(char *data, char *buffer);
void decode_size_from_lsb(long *size, char *buffer);

/* Updated prototypes to use pointers */
Status decode_magic_string_len(DecodeInfo *decInfo, int *len);
Status decode_magic_string(char *magic_str, int len, DecodeInfo *decInfo);
Status decode_secret_file_extn_len(DecodeInfo *decInfo, int *extn_len);
Status decode_secret_file_extn(char *extn, DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif