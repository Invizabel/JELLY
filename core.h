#include <stdio.h>

int Exists(char * filename)
{
    FILE * file = fopen(filename, "rb");
    if (!file)
    {
        return 0;
    }
    fclose(file);
    return 1;
}

int Overwrite(char * filename)
{
    unsigned char buffer = 0;
    FILE * file = fopen(filename, "rb");
    if (!file)
    {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long idx = ftell(file);
    fclose(file);
    file = fopen(filename, "wb");
    if (!file)
    {
        return -1;
    }
    for (long i = 0; i < idx; i++)
    {
        fwrite(&buffer, 1, 1, file);
    }
    fclose(file);
    return 0;
}

int Verify(char * filename)
{
    unsigned char buffer[1];
    long idx = 0;
    FILE * file = fopen(filename, "rb");
    if (!file)
    {
        return -1;
    }
    while (getc(file) != EOF)
    {
        fseek(file, idx, SEEK_SET);
        fread(buffer,1,1,file);
        if (buffer[0] != 0)
        {
            return 0;
        }
        idx += 1;
    }
    fclose(file);
    return 1;
}