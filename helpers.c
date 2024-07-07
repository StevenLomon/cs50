#include "helpers.h"
#include <math.h>

int minInt(int n, int m);
int maxInt(int n, int m);
void swap(BYTE *a, BYTE *b);

int minInt(int n, int m)
{
    if (n <= m)
        return n;
    else
        return m;
}

int maxInt(int n, int m)
{
    if (n >= m)
        return n;
    else
        return m;
}

void swap(BYTE *a, BYTE *b)
{
    BYTE tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // Loop over all pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Take average of red, green, and blue
            int totalRGBvalue = 0;
            totalRGBvalue += image[i][j].rgbtRed;
            totalRGBvalue += image[i][j].rgbtGreen;
            totalRGBvalue += image[i][j].rgbtBlue;
            int averageRGB = round(totalRGBvalue / 3.0);

            // Update pixel values
            image[i][j].rgbtRed = averageRGB;
            image[i][j].rgbtGreen = averageRGB;
            image[i][j].rgbtBlue = averageRGB;
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Compute sepia values
            // The value will be the minimum of the rounded computation and 255, effectively capping
            // the result at 255
            int sepiaRed = minInt(round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen +
                                        .189 * image[i][j].rgbtBlue),
                                  255);
            int sepiaGreen =
                minInt(round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen +
                             .168 * image[i][j].rgbtBlue),
                       255);
            int sepiaBlue = minInt(round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen +
                                         .131 * image[i][j].rgbtBlue),
                                   255);

            // Update pixel with sepia values
            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Find the "mirror line"
    int mirrorLine = round(width / 2);

    // Loop through every row in the image
    for (int i = 0; i < height; i++)
    {
        // Find the "mirror pixel" for each pixel in the row and swap them
        for (int j = 0; j < mirrorLine; j++)
        {
            // printf("Original pixel: %i, %i\n", i, j);
            // printf("Mirror pixel: %i, %i\n", i, mirrorLine - j - 1);

            // Swap the RGB values for each pixel pair
            swap(&image[i][j].rgbtRed, &image[i][width - j - 1].rgbtRed);
            swap(&image[i][j].rgbtGreen, &image[i][width - j - 1].rgbtGreen);
            swap(&image[i][j].rgbtBlue, &image[i][width - j - 1].rgbtBlue);
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy of the original image
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
            copy[i][j] = image[i][j];
    }

    // Read pixels’ colors from copy but write pixels’ colors in image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Initialize necessary variables to 0
            int totalRed = 0;
            int totalGreen = 0;
            int totalBlue = 0;
            int count = 0;

            // max(0, i - 1) ensures that we don't try to access a pixel above the current one if
            // we're in the top row i - 1 is the pixel above and if we are at the top row (i == 0),
            // we use i = 0 rather than i = -1 min (height - 1, i + 1) ensures that we don't try to
            // access a pixel below the current one if we're in the bottom row i + 1 is the pixel
            // below and if we are in the bottom row (i == height - 1), we use i = height - 1 rather
            // than i = height Very similar logic for j and width
            for (int k = maxInt(0, i - 1); k <= minInt(height - 1, i + 1); k++)
            {
                for (int l = maxInt(0, j - 1); l <= minInt(width - 1, j + 1); l++)
                {
                    totalRed += copy[k][l].rgbtRed;
                    totalGreen += copy[k][l].rgbtGreen;
                    totalBlue += copy[k][l].rgbtBlue;
                    count++;
                }
            }

            // Set the new average "blurred" values as the new values
            image[i][j].rgbtRed = round(totalRed / (float) count);
            image[i][j].rgbtGreen = round(totalGreen / (float) count);
            image[i][j].rgbtBlue = round(totalBlue / (float) count);
        }
    }
    return;
}
