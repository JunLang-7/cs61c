/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

int dx[8] = {0, 0, 1, -1, 1, 1, -1, -1};
int dy[8] = {1, -1, 1, -1, 0, -1, 0, 1};

int modNear(int num, int mod) {
    return (num + mod) % mod;
}

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
    Color *state = (Color*)malloc(sizeof(Color));
    if (state == NULL) return NULL;

    int aliveR = image->image[row][col].R == 255;
    int aliveG = image->image[row][col].G == 255;
    int aliveB = image->image[row][col].B == 255;

    int neighR = 0, neighG = 0, neighB = 0;

    for (int i = 0; i < 8; i++) {
        int r = modNear(row + dx[i], image->rows);
        int c = modNear(col + dy[i], image->cols);

        neighR += (image->image[r][c].R == 255);
        neighG += (image->image[r][c].G == 255);
        neighB += (image->image[r][c].B == 255);
    }

    int idxR = 9 * aliveR + neighR;
    int idxG = 9 * aliveG + neighG;
    int idxB = 9 * aliveB + neighB;

    state->R = (rule >> idxR) & 1 ? 255 : 0;
    state->G = (rule >> idxG) & 1 ? 255 : 0;
    state->B = (rule >> idxB) & 1 ? 255 : 0;
    
    return state;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
    Image *new_img = (Image*)malloc(sizeof(Image));
    if (new_img == NULL) return NULL;

    new_img->rows = image->rows;
    new_img->cols = image->cols;
    new_img->image = (Color**)malloc(sizeof(Color*) * new_img->rows);
    if (new_img->image == NULL) {
        free(new_img);
        return NULL;
    }
    
    for (uint32_t i = 0; i < new_img->rows; i++) {
        new_img->image[i] = (Color*)malloc(sizeof(Color) * new_img->cols);
        if (new_img->image[i] == NULL) {
            for (uint32_t j = 0; j < i; j++) {
                free(new_img->image[j]);
            }
            free(new_img->image);
            free(new_img);
            return NULL;
        }
        
        for (uint32_t j = 0; j < new_img->cols; j++) {
            Color *color = evaluateOneCell(image, i, j, rule);
            if (color == NULL) {
                for (uint32_t k = 0; k <= i; k++) {
                    free(new_img->image[k]);
                }
                free(new_img->image);
                free(new_img);
                return NULL;
            }
            new_img->image[i][j] = *color;
            free(color);
        }
    }
    return new_img;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: ./gameOfLife filename rule\n");
        printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
        printf("rule is a hex number beginning with 0x; Life is 0x1808.\n");
        exit(-1);
    }
    char* filename = argv[1];
    uint32_t rule = strtoul(argv[2], NULL, 16); 

    Image *orgi_img = readData(filename);
    Image *new_img = life(orgi_img, rule);
    writeData(new_img);
    freeImage(orgi_img);
    freeImage(new_img);
    return 0;
}
