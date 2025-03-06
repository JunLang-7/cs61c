/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
    Color *color = (Color*)malloc(sizeof(Color));
    if (color == NULL) return NULL;
    
    if ((image->image[row][col].B & 1) == 1) {
        color->R = color->G = color->B = 255;
    }
    else {
        color->R = color->G = color->B = 0;
    }
    return color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
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
            Color *color = evaluateOnePixel(image, i, j);
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
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s filename\n", argv[0]);
        printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
        exit(-1);
    }
    char *filename = argv[1];
    Image *orig_img = readData(filename);
    Image *new_img = steganography(orig_img);
    writeData(new_img);
    freeImage(orig_img);
    freeImage(new_img);
    return 0; 
}
