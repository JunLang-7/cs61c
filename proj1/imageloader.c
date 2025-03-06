/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				Junlang
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    } 
    
    char format[3];
    uint32_t col, row;
    if (fscanf(fp, "%2s", format) != 1 || strcmp(format, "P3") != 0)
	{
		fprintf(stderr, "Error: wrong format of file");
		fclose(fp);
		return NULL;
	}

	if (fscanf(fp, "%u %u", &col, &row) != 2)
	{
		fprintf(stderr, "Error: Unable to read the number of rows and columns.\n");
		fclose(fp);
		return NULL;
	}

	int max_value;
	if (fscanf(fp, "%d", &max_value) != 1 || max_value != 255)
	{
		printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
		fclose(fp);
		return NULL;
	}

    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) {
        fclose(fp);
        return NULL;
    }

    img->rows = row;
    img->cols = col;
    img->image = (Color**)malloc(row * sizeof(Color*));
    if (!img->image) {
        free(img);
        fclose(fp);
        return NULL;
    }

    for (uint32_t i = 0; i < row; i++) {
        img->image[i] = (Color*) malloc(col * sizeof(Color));
        if (!img->image[i]) {
            for (uint32_t j = 0; j < i; j++) {
                free(img->image[j]);
            }
            free(img->image);
            free(img);
            fclose(fp);
            return NULL;
        }
    } 

    for (uint32_t i = 0; i < row; i++) {
        for (uint32_t j = 0; j < col; j++) {
            uint8_t r, g, b;
            fscanf(fp, "%hhu %hhu %hhu", &r, &g, &b);
            img->image[i][j].R = r;
            img->image[i][j].G = g;
            img->image[i][j].B = b;
        }
    }
    
    fclose(fp);
    return img;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
    printf("P3\n");
    printf("%u %u\n", image->rows, image->cols);
    printf("255\n");
    
    for (uint32_t i = 0; i < image->rows; i++) {
        for (uint32_t j = 0; j < image->cols; j++) { 
            Color color = image->image[i][j];
            printf("%3hhu %3hhu %3hhu", color.R, color.G, color.B);
            if (j != image->cols - 1) {
                printf("   ");
            } 
        }
        printf("\n");
    }
}

void freeImage(Image *image)
{
    if (image) {
        if (image->image) {
            for (uint32_t i = 0; i < image->rows; i++) {
                free(image->image[i]);
            } 
            free(image->image);
        }
        free(image);
    }
}
