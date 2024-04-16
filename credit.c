#include <cs50.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_length_of_card_number(long card_number);
int get_the_nth_last_digit(int n, long input_long);
int get_first_n_digits(int n, long input_long);
bool is_even(int n);
bool satisfies_checksum(long card_number, int card_number_length);
int determine_card_type(long card_number, int card_number_length);

int main(void)
{
    long user_card_number = get_long("Number: ");
    int user_card_number_length = get_length_of_card_number(user_card_number);

    // There will be debugging remnants here, I think they're charming, they remind me of everything
    // I did and I can look back on what my past self did like a proud dad :))
    /*printf("Card number length: %i\n", user_card_number_length);
    printf("\nFirst digit: %i\n", get_first_n_digits(1, user_card_number));
    printf("Last digit: %i\n", get_the_nth_last_digit(1, user_card_number));
    printf("Second to last digit: %i\n", get_the_nth_last_digit(2, user_card_number));*/

    if (satisfies_checksum(user_card_number, user_card_number_length))
    {
        int card_type = determine_card_type(user_card_number, user_card_number_length);
        if (card_type == 0)
            printf("INVALID\n");
        else if (card_type == 1)
            printf("AMEX\n");
        else if (card_type == 2)
            printf("MASTERCARD\n");
        else
            printf("VISA\n");
    }
    else
    {
        printf("INVALID\n");
    }

    return 0;
}

int get_the_nth_last_digit(int n, long input_long)
{
    long modulo_number = pow(10, n);
    long digit = input_long %
                 modulo_number; // This was int first but it has to be long to keep track of the
                                // first digits when going backwards in a 15-16 digit card number

    // The above will give us the n last digits. To get the nth last *digit*
    // we divide by 10 to the power of n - 1
    // I.e. if the two last digits are 76, we divide by 10 to get 7
    if (n > 1)
    {
        digit = digit / pow(10, n - 1);
    }
    return digit;
}

int get_first_n_digits(int n, long input_long)
{
    long first_n_digits = input_long;

    // Remove last digit until only the first n digits are left
    while (first_n_digits >= pow(10, n))
    {
        first_n_digits = first_n_digits / 10;
    }

    return first_n_digits;
}

int get_length_of_card_number(long card_number)
{
    char card_number_as_string[20];
    sprintf(card_number_as_string, "%ld", card_number);
    return strlen(card_number_as_string);
}

bool is_even(int n)
{
    if (n % 2 == 0)
        return true;
    else
        return false;
}

bool satisfies_checksum(long card_number, int card_number_length)
{
    int checksum = 0;
    // if (is_even(card_number_length))
    for (int i = card_number_length; i >= 1; i--)
    {
        // Get the current digit. At the beginning of the loop it's the last one, meaning we want to
        // send 1 to the function There are probably other ways to map 16 to 1, 15 to 2 etc. but
        // this is what I settled for
        int inverse_i = abs(i - card_number_length) + 1;
        int current_digit = get_the_nth_last_digit(inverse_i, card_number);
        // printf("i: %i, inverse i: %i, current digit: %i\n", i, inverse_i, current_digit);

        // Depending on whether or not the card number is of even or odd length, we skip even or odd
        // placements of the digits i.e. if card number length is 16, we skip digit #16, #14, #12
        // etc. and the inverse if the length is odd
        if (is_even(card_number_length))
        {
            // If we skip the number, add it straight to the checksum as of step #2 in the algorithm
            if (is_even(i))
                checksum += current_digit;
            else
            {
                // Multiply by 2
                current_digit = current_digit * 2;
                // If the new digit after multiplication is 2 is now two digits (10 or above), we
                // get each separate digit and add them to the checksum
                if (current_digit > 9)
                {
                    int second_digit = get_the_nth_last_digit(1, current_digit);
                    int first_digit = get_the_nth_last_digit(2, current_digit);
                    // Add both to the checksum
                    checksum += (second_digit + first_digit);
                }
                // Otherwise, we simply add the new digit after multiplication by 2 to the checksum
                else
                    checksum += current_digit;
            }
            // printf("Checksum: %i\n", checksum);
        }
        else
        // This is simply the inverse of above. I'm aware that I am repeating the else statement but
        // I wasn't really sure how to make a function that would affect the checksum outside of it
        {
            if (!is_even(i))
                checksum += current_digit;
            else
            {
                current_digit = current_digit * 2;
                if (current_digit > 9)
                {
                    int second_digit = get_the_nth_last_digit(1, current_digit);
                    int first_digit = get_the_nth_last_digit(2, current_digit);
                    checksum += (second_digit + first_digit);
                }
                else
                    checksum += current_digit;
            }
            // printf("Checksum: %i\n", checksum);
        }
    }

    // printf("Checksum: %i\n", checksum);

    // If the last digit of the checksum is zero, we return True, else False
    if (get_the_nth_last_digit(1, checksum) == 0)
        return true;
    else
        return false;
}

// The following function gives 0 if invalid, 1 if American Express, 2 if MasterCard or 3 is Visa
int determine_card_type(long card_number, int card_number_length)
{
    int first_two_digits = get_first_n_digits(2, card_number);
    // printf("First two digits: %i\n", first_two_digits);
    if (card_number_length == 15 && (first_two_digits == 34 || first_two_digits == 37))
        return 1;
    else if (card_number_length == 16 && (51 <= first_two_digits && first_two_digits <= 55))
        return 2;
    else if ((card_number_length == 13 || card_number_length == 16) &&
             get_first_n_digits(1, card_number) == 4)
        return 3;
    else
        return 0;
}
