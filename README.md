*************************************************************************************************************************************************
LSB Steganography

1. Overview
    This project is an LSB (Least Significant Bit) Steganography.

    ENCODE :
        The encoding process hides a secret file inside a BMP image by modifying the least significant bits (LSBs) of the image’s pixel data.
        The output is a stego image, which looks visually identical to the original image but contains hidden data.

    DECODE :
        It extracts hidden data from a stego BMP image that was created by an encoding process.
        It supports reading:
            Magic string (to verify correct file)
            Secret file extension
            Secret file size
            Secret file data
        Then it writes the recovered data into an output file with the proper extension.

2. main.c
    Handles command-line arguments and determines the operation:
        ->  -e → Encoding
        ->  -d → Decoding

    i). If encoding is selected:
            ->  Validates arguments using read_and_validate_encode_args().
            ->  Calls do_encoding() to extract the hidden data.
            ->  Prints success/failure messages accordingly.

    ii). If decoding is selected:
            ->  Validates arguments using read_and_validate_decode_args().
            ->  Calls do_decoding() to extract the hidden data.
            ->  Prints success/failure messages accordingly.
3. encode.h

    Defines:
        ->  EncodeInfo struct → Stores:
        ->  Input source image filename & file pointer
        ->  Secret filename & file pointer
        ->  Output file name (with appended extension)
        ->  Magic string
        ->  Secret file extension & size
        ->  Secret file size

    Function prototypes for all decoding steps:
            ->  Reading & validating arguments
            ->  Opening files
            ->  Check capacity
            ->  Copying BMP header
            ->  Encoding magic string length & data
            ->  Encoding file extension length & extension
            ->  Encoding file size
            ->  Encoding file data (and writing to output file)
            ->  Copy remaining file data.


4. encode.c

    Encoding Steps
        1.  Read and Validate Arguments
                ->  Confirms that the source image is a .bmp.
                ->  Confirms that the secret file exists.
                ->  Creates or opens output stego image.
        2.  Open Files
                ->  Opens source image (read mode).
                ->  Opens secret file (read mode).
                ->  Opens stego image (write mode).
        3.  Check capacity
                ->  Get the source file size.
                ->  Get the secret file size and data.
                ->  Get the magic string length and data.
                ->  Calculate wheather the encoding file capacity is less than or equal to (source file size - 54).
        3.  Copy BMP Header
                ->  Copies first 54 bytes (header) from source image to stego image unchanged.
        4.  Encode Magic String Length
                ->  Calculates length of predefined magic string.
                ->  Stores its length in 32 LSBs of the next 32 image bytes.
        5.  Encode Magic String
                ->  Stores each character’s ASCII value, 1 bit at a time, into 8 image bytes.
        6.  Encode Secret File Extension Length
                ->  Reads extension length (e.g., ".txt" → 4).
                ->  Stores length using 32 LSBs.
        7.  Encode Secret File Extension
                ->  Stores extension character by character in LSBs.
        8.  Encode Secret File Size
                ->  Determines file size (in bytes).
                ->  Stores it using 32 LSBs.
        9.  Encode Secret File Data
                ->  Reads file byte-by-byte.
                ->  Encodes each byte’s 8 bits into 8 image bytes’ LSBs.
        10. Copy Remaining Image Data
        11. return success.

5. decode.h

    Defines:
        ->  DecodeInfo struct → Stores:
        ->  Input stego image filename & file pointer
        ->  Output file name (with appended extension)
        ->  Magic string
        ->  Secret file extension & size
        ->  Secret file size

    Function prototypes for all decoding steps:
            ->  Reading & validating arguments
            ->  Opening files
            ->  Skipping BMP header
            ->  Decoding magic string length & data
            ->  Decoding file extension length & extension
            ->  Decoding file size
            ->  Decoding file data (and writing to output file)

6. decode.c

    Implements all decoding steps:
        i).     Argument Validation
        ii).    Ensures input file is ".bmp".
        iii).   Extracts output file name (or defaults to decoded_output).
        iv).    File Handling
        v).     Opens stego image for reading.
        vi).    BMP Header Skipping
        vii).   Skips the first 54 bytes (standard BMP header).
        viii).  Decoding Functions
                    ->  decode_size_from_lsb() → Reads 32 bits (4 bytes) hidden in LSBs to get lengths/sizes.
                    ->  decode_byte_from_lsb() → Reads 8 bits (1 byte) hidden in LSBs.
                    ->  decode_magic_string_len() → Reads length of magic string.
                    ->  decode_magic_string() → Reads actual magic string and verifies with user input.
                    ->  decode_secret_file_extn_len() → Reads length of secret file extension.
                    ->  decode_secret_file_extn() → Reads extension, appends it to output file name.
                    ->  decode_secret_file_size() → Reads size of hidden file.
                    ->  decode_secret_file_data() → Extracts hidden bytes and writes to output file.
                    
        ix).    Decoding Orchestration
                    ->  do_decoding() calls all the above functions in sequence:
                            1.  Opens file
                            2.  Skips BMP header
                            3.  Decodes magic string 
                            4.  Read magic string and verify with decoded ones.
                            4.  Decodes file extension & size
                            5.  Writes secret file content to disk
                            6. return success.


OUTPUT :-

    ENCODE OUTPUT :-
        ./a.out -e beautiful.bmp secret.txt                                         or 
        ./a.out -e beautiful.bmp secret.txt stego.bmp                               or
        ./a.out -e beautiful.bmp secret.txt stego                                   or

        ./a.out -e beautiful.bmp secret.(extension like .txt, .c, .cpp, etc)

                You have choosen encoding
                .bmp is present
                Secret file with extension '.txt' is present
                No stego image name provided, using default stego.bmp

                Enter a magic string (max 19 chars): #*
                Magic string is stored.

                Image capacity is sufficient.

                Header is successfully stored...
                Magic string length is successfully stored in stego.bmp...
                Magic string is successfully stored in stego.bmp...
                Secret file extension length is successfully stored in stego.bmp...
                Secret file extension is successfully stored in stego.bmp...
                Secret file size is successfully stored in stego.bmp...
                Secret file data is successfully stored in stego.bmp...
                Copy remaining image data is successfully stored in stego.bmp...

                Encoding is successfully done....

    DECODE OUTPUT :-
        ./a.out -d stego.bmp                        or
        ./a.out -d stego.bmp output                 or
        ./a.out -d stego.bmp output.txt             or

        ./a.out -d stego.bmp output.(extension like .txt, .c, .cpp, etc)
        
                You have choosen decoding.
                .bmp is present.
                Output file is present.

                Skipping header successfully...
                Magic string length is successfully decoded...
                Magic string is successfully decoded...

                Enter the magic string used during encoding: #*
                Magic string verified. Proceeding with decoding...

                Secret file extension length is successfully decoded...
                Secret file extension is successfully decoded...
                Secret file size is successfully decoded...
                decoded Output file is successfully opened...
                Secret file data is succesfully decoded...

                Decoding is successfully done....


**************************************************************************************************************************************************
