#include <stdio.h>
#include <string.h>
#include "decode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Stego image must be a BMP file\n\n");
        return d_failure;
    }
    printf(".bmp is present\n");
    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)
    {
        strncpy(decInfo->output_fname, argv[3], sizeof(decInfo->output_fname) - 1);
        decInfo->output_fname[sizeof(decInfo->output_fname) - 1] = '\0';

        char *dot = strrchr(decInfo->output_fname, '.');
        if (dot)
        {
            *dot = '\0';
        }
        printf("%s file is present...\n\n", decInfo->output_fname);
    }
    else
    {
        sprintf(decInfo->output_fname, "decoded_output");
        printf("%s file is present...\n\n", decInfo->output_fname);
    }

    return d_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return d_failure;
    }
    return d_success;
}


Status skip_bmp_header(FILE *fptr_src)
{
    if(fseek(fptr_src, 54, SEEK_SET) != 0)
    {
        printf("Error: Unable to skip bmp header\n");
        return d_failure;
    }
    printf("Skipping header successfully...\n");
    return d_success;
}

void decode_byte_from_lsb(char *data, char *buffer)
{
    *data = 0;
    for(int i = 0; i < 8; i++)
    {
       *data = (*data << 1) | (buffer[i] & 1);
    }
}

void decode_size_from_lsb(long *size, char *buffer)
{
    *size = 0;
    for(int i = 0; i < 32; i++)
    {
       *size = (*size << 1) | (buffer[i] & 1);
    }
}

Status decode_magic_string_len(DecodeInfo *decInfo, int *len)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb((long*)len, buffer);
    printf("Magic string length is successfully decoded...\n");
    return d_success;
}

Status decode_magic_string(char *magic_str, int len, DecodeInfo *decInfo)
{
    char buffer[8];
    for(int i = 0; i < len; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&magic_str[i], buffer);
    }
    magic_str[len] = '\0';
    printf("Magic string is successfully decoded...\n");
    return d_success;
}

Status decode_secret_file_extn_len(DecodeInfo *decInfo, int *extn_len)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb((long*)extn_len, buffer);
    decInfo->extn_size = *extn_len;
    printf("Secret file extension length is successfully decoded...\n");
    return d_success;
}

Status decode_secret_file_extn(char *extn, DecodeInfo *decInfo)
{
    char buffer[8];
    for(int i = 0; i < decInfo->extn_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&extn[i], buffer);
    }
    extn[decInfo->extn_size] = '\0';

    strcat(decInfo->output_fname, extn);

    printf("Secret file extension is successfully decoded...\n");
    return d_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(file_size, buffer);
    decInfo->size_secret_file = *file_size;
    printf("Secret file size is successfully decoded...\n");
    return d_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    FILE *fptr_output = fopen(decInfo->output_fname, "w");
    if(!fptr_output)
    {
        perror("fopen output");
        return d_failure;
    }

    printf("decoded Output file is successfully opened...\n");
    for(long i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch, buffer);
        fwrite(&ch, 1, 1, fptr_output);
    }

    fclose(fptr_output);
    printf("Secret file data is succesfully decoded...\n");
    return d_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == d_failure)
    {
        return d_failure;
    }

    skip_bmp_header(decInfo->fptr_stego_image);

    int magic_len;
    decode_magic_string_len(decInfo, &magic_len);
    decode_magic_string(decInfo->magic_string, magic_len, decInfo);

    
    char expected_magic[MAX_MAGIC_STRING];
    printf("\nEnter the magic string used during encoding: ");
    scanf("%s", expected_magic);

    if(strcmp(decInfo->magic_string, expected_magic) != 0)
    {
        printf("ERROR: Magic string mismatch! Cannot decode.\n\n");
        fclose(decInfo->fptr_stego_image);
        return d_failure;
    }
    else
    {
        printf("Magic string verified. Proceeding with decoding...\n\n");
    }

    int extn_len;
    decode_secret_file_extn_len(decInfo, &extn_len);
    decode_secret_file_extn(decInfo->extn_secret_file, decInfo);

    long secret_size;
    decode_secret_file_size(decInfo, &secret_size);
    decode_secret_file_data(decInfo);

    fclose(decInfo->fptr_stego_image);
    return d_success;
}