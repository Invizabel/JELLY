#include "core.h"
#include <stdio.h>

int main()
{
    FILE * file = fopen("output.bmp", "rb");
    if (!file)
        return 1;

    // go to end to measure size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    // allocate buffer
    unsigned char * buffer = malloc(size);
    if (!buffer)
    {
        fclose(file);
        return 1;
    }

    // read entire file
    size_t read = fread(buffer, 1, size, file);
    fclose(file);

    Image img = BMP(buffer);

    printf("Width: %d\n", img.width);
    printf("Height: %d\n", img.height);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            printf("(%d,%d,%d) ",
                img.pixels[y][x][0],
                img.pixels[y][x][1],
                img.pixels[y][x][2]);
        }
        printf("\n");
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            free(img.pixels[y][x]);
        }
        free(img.pixels[y]);
    }
    free(img.pixels);

    if (read != size)
    {
        free(buffer);
        return 1;
    }

    free(buffer);
    return 0;
}