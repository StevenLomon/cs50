#include "helpers.h"
#include <math.h>
#include <stdio.h>

#define KERNELSIZE 3
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
void swap(BYTE *a, BYTE *b);

int gxKernel[KERNELSIZE][KERNELSIZE] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
int gyKernel[KERNELSIZE][KERNELSIZE] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int numberOfColorChannels = 3;
    // We loop through every pixel in the image, find the average RBG values, and set that as the new value
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate the average RGB value
            int RGBAvg = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / numberOfColorChannels);

            // Set this value as the value for red, green and blue
            image[i][j].rgbtRed = RGBAvg;
            image[i][j].rgbtGreen = RGBAvg;
            image[i][j].rgbtBlue = RGBAvg;
        }
    }

    return;
}

void swap(BYTE *a, BYTE *b)
{
    BYTE tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Find the "mirror line"
    int mirrorLine = round(width / 2);
    // printf("%i\n", mirrorLine);

    // Find the "mirror pixel" for each pixel and swap them
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < mirrorLine; j++)
        {
            // printf("Original pixel: %i, %i\n", i, j);
            // printf("Mirror pixel: %i, %i\n", i, 600 - j - 1);

            // Swap the RGB values for each pixel pair
            swap(&image[i][j].rgbtRed, &image[i][600 - j - 1].rgbtRed);
            swap(&image[i][j].rgbtGreen, &image[i][600 - j - 1].rgbtGreen);
            swap(&image[i][j].rgbtBlue, &image[i][600 - j - 1].rgbtBlue);
        }
    }

    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Initialize necessary variables to 0
            int totalRed = 0;
            int totalGreen = 0;
            int totalBlue = 0;
            int count = 0;

            // max(0, i - 1) ensures that we don't try to access a pixel above the current one if we're in the top row
            // i - 1 is the pixel above and if we are at the top row (i == 0), we use i = 0 rather than i = -1
            // min (height - 1, i + 1) ensures that we don't try to access a pixel below the current one if we're in the bottom row
            // i + 1 is the pixel below and if we are in the bottom row (i == height - 1), we use i = height - 1 rather than i = height
            // Very similar logic for j and width
            for (int k = max(0, i - 1); k <= min(height - 1, i + 1); k++)
            {
                for (int l = max(0, j - 1); l <= min(width - 1, j + 1); l++)
                {
                    totalRed += image[k][l].rgbtRed;
                    totalGreen += image[k][l].rgbtGreen;
                    totalBlue += image[k][l].rgbtBlue;
                    count++;
                }
            }

            // Set the new average "blurred" values as the new values
            image[i][j].rgbtRed = round(totalRed / count);
            image[i][j].rgbtGreen = round(totalGreen / count);
            image[i][j].rgbtBlue = round(totalBlue / count);
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // printf("i: %i, j: %i, Red: %i, Green: %i, Blue: %i\n", i, j, image[i][j].rgbtRed, image[i][j].rgbtGreen, image[i][j].rgbtBlue);
            int totalGxRed = 0, totalGxGreen = 0, totalGxBlue = 0;
            int totalGyRed = 0, totalGyGreen = 0, totalGyBlue = 0;

            // In this case we do allow k and l to be negative values, going outside of the boundaries of the image
            // since we will set the color values to be zero here anyway
            for (int k = i - 1; k <= i + 1; k++)
            {
                for (int l = j - 1; l <= j + 1; l++)
                {
                    // It's only if we're actually looking at a pixel inside the boundaries of the image that we grab the RGB values
                    if (k >= 0 && l >= 0)
                    {
                        // Kernel index variables
                        int m = k + 1, n = l + 1;
                        // printf("Red: %i, Green: %i, Blue: %i", image[k][l].rgbtRed, image[k][l].rgbtGreen, image[k][l].rgbtBlue);
                        // There is a discrepancy in how we index the kernels and the the pixel. -1, -1 for the pixel is the topleft pixel but that is 0,0 in the kernel
                        // Gx
                        totalGxRed += image[k][l].rgbtRed * gxKernel[m][n];
                        totalGxGreen += image[k][l].rgbtGreen * gxKernel[m][n];
                        totalGxBlue += image[k][l].rgbtBlue * gxKernel[m][n];

                        // Gy
                        totalGyRed += image[k][l].rgbtRed * gyKernel[m][n];
                        totalGyGreen += image[k][l].rgbtGreen * gyKernel[m][n];
                        totalGyBlue += image[k][l].rgbtBlue * gyKernel[m][n];
                    }
                }
            }

            // Set the new average "edge detenced" values as the new values (capped at 255)
            int totalRed = min(255, round(sqrt(totalGxRed) + sqrt(totalGyRed)));
            int totalGreen = min(255, round(sqrt(totalGxGreen) + sqrt(totalGyGreen)));
            int totalBlue = min(255, round(sqrt(totalGxBlue) + sqrt(totalGyBlue)));
            image[i][j].rgbtRed = totalRed;
            image[i][j].rgbtGreen = totalGreen;
            image[i][j].rgbtBlue = totalBlue;
        }
    }
    return;
}
