#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <getopt.h>

const char VERSION_STRING[] = "1.0.0";
const char DATE_STRING[] = "26-AUG-2019";

const char DEFAULT_IN_FILE[] = "in.txt";
const char DEFAULT_OUT_FILE[] = "out.bin";

void usage(const char *prog, const char *extraLine = (const char *)(NULL));

void usage(const char *prog, const char *extraLine)
{
    fprintf(stderr, "usage: %s <options>\n", prog);
    fprintf(stderr, "-i --in inputFile\n");
    fprintf(stderr, "-o --out outputFile\n");
    if (extraLine) fprintf(stderr, "\n%s\n", extraLine);
}

int main(int argc, char *argv[])
{
    int opt;
    char inFile[256];
    char outFile[256];
    char errorLine[256];
    char lineBuf[1024];
    bool usageError = false;
    FILE *fpIn;
    FILE *fpOut;
    uint8_t b;
    char c;
    char *cp;
    bool lastNibble;

    printf("%s v%s %s\n", basename(argv[0]), VERSION_STRING, DATE_STRING);

    struct option longOptions[] =
    {
        {"in",          required_argument,  0,      'i'}
        ,{"out",        required_argument,  0,      'o'}
        ,{0,0,0,0}
    };

    inFile[0] = '\0';
    outFile[0] = '\0';
    while (1)
    {
        int optionIndex = 0;

        opt = getopt_long(argc, argv, "i:o:h?", longOptions, &optionIndex);

        if (-1 == opt) break;

        switch (opt)
        {
            case 'i':
                strcpy(inFile, optarg);
                break;
            case 'o':
                strcpy(outFile, optarg);
                break;
            case 'h':
            case '?':
            default:
                usageError = true;
                break;
        }
    }

    if (usageError)
    {
        usage(basename(argv[0]));
        return -1;
    }

    if ('\0' == inFile[0])
    {
        strcpy(inFile, DEFAULT_IN_FILE);
    }

    if ('\0' == outFile[0])
    {
        strcpy(outFile, DEFAULT_OUT_FILE);
    }

    fpIn = fopen(inFile, "r");
    if ((FILE *)(NULL) == fpIn)
    {
        sprintf(errorLine, "Unable to open input file %s\n", inFile);
        usage(basename(argv[0]), errorLine);
        return -1;
    }

    fpOut = fopen(outFile, "wb");
    if ((FILE *)(NULL) == fpOut)
    {
        sprintf(errorLine, "Unable to open output file %s\n", outFile);
        usage(basename(argv[0]), errorLine);

        return -1;
    }

    while (fgets(lineBuf, sizeof(lineBuf), fpIn))
    {
        b = 0;
        cp = lineBuf;
        lastNibble = false;
        while   (   ('\0' != *cp)
                 && ('\n' != *cp)
                )
        {
            if (isxdigit(*cp))
            {
                b *= 16;
                c = toupper(*cp);
                if (c <= '9')
                {
                    b += c - '0';
                }
                else
                {
                    b += c - 'A' + 10;
                }
                if (lastNibble)
                {
                    fputc(b, fpOut);
                    b = 0;
                    lastNibble = false;
                }
                else
                {
                    lastNibble = true;
                }
            }
            ++cp;
        }
    }
    fclose(fpIn);
    fclose(fpOut);
    return 0;
}
