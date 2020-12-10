#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <stdbool.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: name of file\n");
        return 1;
    }

    char *infile = argv[optind];

    FILE *input = fopen(infile, "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 2;
    }

    // calculating the size of the file
    fseek(input, 0L, SEEK_END);
    long int res = ftell(input) / 512;
    fseek(input, 0, SEEK_SET);

    int count = 0;
    bool cntn = false;
    FILE *outptr = NULL;
    for (int i = 0; i < res; i++)
    {
        BYTE *buffer = calloc(512, sizeof(BYTE));
        if (buffer == NULL)
        {
            printf("Could not allocate memory.\n");
            return 3;
        }

        fread(buffer, sizeof(BYTE), 512, input);
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if (outptr != NULL)
            {
                fclose(outptr);
                count++;
            }
            char name[8];
            sprintf(name, "%03i.jpg", count);
            outptr = fopen(name, "w");
            fwrite(buffer, sizeof(BYTE), 512, outptr);
            cntn = true;
        }
        else if (cntn)
        {
            fwrite(buffer, sizeof(BYTE), 512, outptr);
        }
        free(buffer);
    }
    fclose(outptr);
    fclose(input);
    return 0;
}
