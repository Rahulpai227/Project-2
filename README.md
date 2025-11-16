// MP3 Tag Reader //

1. Creates a new file named "new_file.mp3" for writing the updated content.
2. Checks if the input file (fptr) is opened properly.

    i>. EDIT
        Defines an array of tag frame identifiers corresponding to common ID3v2 metadata fields
            1	"TIT2"	Title
            2	"TPE1"	Artist
            3	"TALB"	Album
            4	"TYER"	Year
            5	"TCON"	Genre/Content
            6	"TCOM"	Composer

            1.  't' → Title (TIT2)
                    Edits the Title of the MP3 track.
                    Searches for the TIT2 frame and replaces its content.
                    Useful to update or correct the song title.

            2.  'a' → Artist (TPE1)
                    Edits the Artist name of the song.
                    Searches for the TPE1 frame and updates it with new artist info.
                    Helps assign correct artist metadata.

            3.  'A' → Album (TALB)
                    Edits the Album name of the track.
                    Searches for the TALB frame and writes new album information.
                    Helps organize songs by album in music players.

            4.  'y' → Year (TYER)
                    Edits the Year the song was released.
                    Locates the TYER frame and replaces it.
                    Useful for cataloging music chronologically.

            5.  'c' → Genre (TCON)
                    Edits the Genre of the track.
                    Finds the TCON frame and updates its content.
                    Helps group music by genre (e.g., Rock, Pop).

            6.  'm' → Composer (TCOM)
                    Edits the Composer of the song.
                    Locates the TCOM frame and updates the name.
                    Useful for classical or instrumental music metadata.

    ii>.    copy(...)
                This function handles the actual replacement of metadata in the MP3 file. It:
                Locates the position of the target frame using the search() function.
                Copies everything from the original MP3 file to a new file, except the old content of the specified frame.
                Writes the new content into the appropriate frame, then copies the rest of the original file.
                Replaces the original MP3 with the new one and displays the updated metadata.

    iii>.   view(...)
                This function displays the values of standard ID3v2 tags such as Title, Artist, Album, Year, Genre, and Composer. It:
                Iterates over a predefined list of frame identifiers.
                Calls search() to get the content for each frame.
                Prints the tag name and its content in box. If found.

    iv>.    swap(...)
                This function performs a 4-byte endianness conversion, swapping bytes to convert between little-endian and big-endian formats. 
                This is necessary because the size values in ID3v2 frames are stored in big-endian format, but most systems read integers in little-endian.

    v>.     search(...)
                This function searches the MP3 file for a specific ID3v2 frame (e.g., TIT2 for title).It
                Reads the frame ID, size, and skips flags.
                Reads the content of the frame.
                Compares the frame ID with the target and returns the content size if it matches, otherwise returns 0.

    vi>.    print(...)
                This helper function prints the tag content character by character (excluding the last byte which is usually a null terminator).

    vii>.   error_msg(...)
                Displays a user-friendly error message showing correct usage syntax for both viewing and editing MP3 metadata.
                It also explains what each command-line flag (-t, -a, etc.) stands for and exits the program on error.

// OUTPUT //

./a.out -v sample.mp3 (view)

++------------++----------------------------------------------------++
||            ||                                                    ||
||    TAG     ||                    VALUE                           ||
||            ||                                                    ||
++------------++----------------------------------------------------++
||            ||                                                    ||
|| TITLE      || Sunnysunny                                         ||
||            ||                                                    ||
++------------++----------------------------------------------------++
||            ||                                                    ||
|| ARTIST     || Yo Yo Honey Singh - [SongsPk.CC]- [SongsPk.CC]     ||
||            ||                                                    ||
++------------++----------------------------------------------------++
||            ||                                                    ||
|| ALBUM      || Yaariyanney Singh - [SongsPk.CC]- [SongsPk.CC]     ||
||            ||                                                    ||
++------------++----------------------------------------------------++
||            ||                                                    ||
|| YEAR       || 2025iyanney Singh - [SongsPk.CC]- [SongsPk.CC]     ||
||            ||                                                    ||
++------------++----------------------------------------------------++
||            ||                                                    ||
|| CONTENT    || Bollywood song - [SongsPk.CC]CC]- [SongsPk.CC]     ||
||            ||                                                    ||
++------------++----------------------------------------------------++
