#include <stdlib.h>
#include <string.h>

typedef struct {
    int width;
    int height;
    int *** pixels;
} Image;

Image BMP(unsigned char * data)
{
    Image img;

    int pixel_offset;

    memcpy(&pixel_offset, data + 10, 4);
    memcpy(&img.width,    data + 18, 4);
    memcpy(&img.height,   data + 22, 4);

    int row_size = (img.width * 3 + 3) & ~3;

    img.pixels = malloc(img.height * sizeof(int**));

    for (int y = 0; y < img.height; y++)
    {
        img.pixels[y] = malloc(img.width * sizeof(int*));

        int row_start = pixel_offset + (img.height - 1 - y) * row_size;

        for (int x = 0; x < img.width; x++)
        {
            img.pixels[y][x] = malloc(3 * sizeof(int));

            int i = row_start + x * 3;

            int r = data[i + 2];
            int g = data[i + 1]; 
            int b = data[i];
            int gray = (r + g + b) / 3;
            int bw = (gray > 128) ? 255 : 0;
            img.pixels[y][x][0] = bw;
            img.pixels[y][x][1] = bw;
            img.pixels[y][x][2] = bw;
        }
    }

    return img;
}