#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 512
#define JPG_STRING_SIZE 8

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != optind + 1)
    {
        printf("Usage: .recover FILE\n");
        return 1;
    }

    // Open the memory card
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open %s\n", argv[1]);
        return 1;
    }

    // Create a buffer for a block of data, a buffer for the JPG filename string, and a counter for
    // JPG file names
    uint8_t buffer[BLOCKSIZE];
    char filename[JPG_STRING_SIZE];
    int file_number = 0;

    // Initialize output file
    FILE *img = NULL;

    // While there is still data to read from memory card
    while (fread(buffer, 1, BLOCKSIZE, card) == BLOCKSIZE)
    {
        // Start of a JPG file?
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            printf("Start of JPG!\n");

            // Close the current output file (if it exists)
            if (img != NULL)
            {
                fclose(img);
                file_number++;
            }

            // Format the filename with sprintf
            sprintf(filename, "%03i.jpg", file_number);
            printf("File name: %s\n", filename);

            // Open a new output file
            img = fopen(filename, "w");
            if (img == NULL)
            {
                fclose(card);
                printf("Could not create %s.\n", filename);
                return 1;
            }
        }

        // Read data from card to the current JPG file with name filename
        // current_pixel = buffer[i];
        if (img != NULL)
            fwrite(buffer, 1, BLOCKSIZE, img);
    }

    // Close the current output file (if it exists)
    if (img != NULL)
        fclose(img);

    // Close the memory card
    fclose(card);
    return 0;
}
