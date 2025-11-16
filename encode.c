#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp is present\n");
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }

    char *dot = strrchr(argv[3], '.');
    if (dot != NULL)
    {
        printf("Secret file with extension '%s' is present\n", dot);
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf("ERROR: Secret file does not have an extension\n");
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        char *dot = strrchr(argv[4], '.');
        if (dot && strcmp(dot, ".bmp") == 0)
        {
            printf("Stego image name provided: %s\n", argv[4]);
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            static char stego_with_ext[100];
            if (dot)
                *dot = '\0';

            sprintf(stego_with_ext, "%s.bmp", argv[4]);
            printf("Converted output to: %s\n", stego_with_ext);
            encInfo->stego_image_fname = stego_with_ext;
        }
    }
    else
    {
        printf("No stego image name provided, using default stego.bmp\n");
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
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

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image) - 54;

    long secret_file_size = 0;
    char ch;
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        secret_file_size++;
    }
    rewind(encInfo->fptr_secret);

    encInfo->size_secret_file = secret_file_size;

    char *dot = strrchr(encInfo->secret_fname, '.');
    char ext[10] = "";
    if (dot != NULL)
    {
        strcpy(ext, dot);
    }

    // Calculate total required size in bytes to encode the secret in the image
    // 4 bytes for storing length of magic string
    // strlen(magic_string) bytes for the magic string itself
    // 4 bytes for storing length of file extension
    // strlen(ext) bytes for the extension
    // 4 bytes for storing secret file size
    // secret_file_size bytes for the actual secret file
    long required_size = (4 + strlen(encInfo->magic_string) + 4 + strlen(ext) + 4 + secret_file_size) * 8;

    if (image_capacity >= required_size)
    {
        printf("Image capacity is sufficient.\n\n");
        return e_success;
    }
    else
    {
        printf("ERROR: Image capacity insufficient\n\n");
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    rewind(fptr_src_image);
    fread(header, 1, 54, fptr_src_image);
    fwrite(header, 1, 54, fptr_dest_image);
    printf("Header is successfully stored...\n");
    return e_success;
}

// void encode_char(char data, unsigned char img[8])
// {
//     int n = 7;  // start from MSB (bit 7)

//     for (int i = 0; i < 8; i++)   // Step 2: loop from 0 to 7
//     {
//         int mask = 1 << n;        // Step 3: create mask
//         int bit  = data & mask;   // Step 4: fetch n-th bit (still at position n)
//         bit = bit >> n;           // Step 6: shift into LSB (0 or 1)

//         img[i] = img[i] & 0xFE;   // Step 5: clear LSB
//         img[i] = img[i] | bit;    // Step 7: write bit to LSB

//         n--;                      // Step 8: decrement n
//     }

//     // Step 9: function ends
// }

void encode_byte_to_lsb(char data, char *buffer)
{
    for (int i = 0; i < 8; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
}

void encode_size_to_lsb(long size, char *buffer)
{
    for (int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }
}

Status encode_magic_string_length(int len, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    printf("Magic string length is successfully stored in stego.bmp...\n");
    return e_success;
}

Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(magic_str); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_str[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("Magic string is successfully stored in stego.bmp...\n");
    return e_success;
}

Status encode_secret_file_extn_len(int extn_len, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(extn_len, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    printf("Secret file extension length is successfully stored in stego.bmp...\n");
    return e_success;
}

Status encode_secret_file_extn(const char *extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i = 0; i < strlen(extn); i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    printf("Secret file extension is successfully stored in stego.bmp...\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    printf("Secret file size is successfully stored in stego.bmp...\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];
    while (fread(&ch, 1, 1, encInfo->fptr_secret) > 0)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    printf("Secret file data is successfully stored in stego.bmp...\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    printf("Copy remaining image data is successfully stored in stego.bmp...\n");
    return e_success;
}

Status get_file_extn_len(EncodeInfo *encInfo)
{
    char *ext = strrchr(encInfo->secret_fname, '.');
    if(ext)
    {
        strncpy(encInfo->extn_secret_file, ext, MAX_FILE_SUFFIX - 1);
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';
        encInfo->extn_size = strlen(ext);
        return e_success;
    }
    return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("File cannot open\n");
        return e_failure;
    }

    printf("Enter a magic string (max %d chars): ", MAX_MAGIC_STRING - 1);
    scanf("%s", encInfo->magic_string);
    printf("Magic string is stored\n");

    if(check_capacity(encInfo) == e_failure)
    {
        printf("Check Capacity is fail\n");
        return e_failure;
    }

    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    encode_magic_string_length(strlen(encInfo->magic_string), encInfo);
    encode_magic_string(encInfo->magic_string, encInfo);

    get_file_extn_len(encInfo);

    encode_secret_file_extn_len(encInfo->extn_size, encInfo);
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    encode_secret_file_data(encInfo);
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    
    return e_success;
}