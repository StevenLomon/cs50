#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const int valid_key_length = 26;
int count_occurance(char string[], int string_length, char to_count);
bool valid_key(string cipher_key);
char get_cipher_letter(int casing, char plain_letter, string ciph_key);
string encrypt_plaintext(char *input_string, string cipher_key);

int main(int argc, string argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");

        return 1;
    }
    if (argc == 2)
    {
        string key = argv[1];
        int key_length = strlen(key);

        if (key_length != valid_key_length)
        {
            printf("Key must contain 26 characters.\n");

            return 1;
        }
        else
        {
            if (!valid_key(key))
            {
                printf("Invalid key.\n");

                return 1;
            }

            else
            {
                // printf("%s\n", argv[1]);
                // printf("%lu\n", strlen(argv[1]));
                string plaintext = get_string("plaintext:  ");
                string ciphertext = encrypt_plaintext(plaintext, key);

                printf("ciphertext: %s\n", ciphertext);

                return 0;
            }
        }
    }
}

int count_occurance(char string[], int string_length, char to_count)
{
    int count = 0;

    for (int i = 0; i < string_length; i++)
        if (string[i] == to_count)
            count++;

    return count;
}

bool valid_key(string cipher_key)
{
    // Initialize as true and set to false if there are anything that invalidates the key
    bool valid = true;

    for (int i = 0; i < valid_key_length; i++)
    {
        // Check if there are non-alphabetic characters in the key
        if (!isalpha(cipher_key[i]))
            valid = false;

        // Check if there are duplicate characters in the key
        // If it occurs more than once in the string, it's a duplicate
        int occurance_count = count_occurance(cipher_key, valid_key_length, cipher_key[i]);
        if (occurance_count > 1)
            valid = false;
    }

    return valid;
}

char get_cipher_letter(int casing, char plain_letter, string ciph_key)
{
    int nth_letter_of_the_alphabet = plain_letter - casing;

    // -1 since arrays use zero-based indexing
    char cipher_letter = ciph_key[nth_letter_of_the_alphabet - 1];

    // Convert to upper of lower depending on the casing
    if (casing == 64)
        cipher_letter = toupper(cipher_letter);
    else if (casing == 96)
        cipher_letter = tolower(cipher_letter);

    return cipher_letter;
}

string encrypt_plaintext(char *input_string, string cipher_key)
{
    // For every char of the input_string that is to be encrypter, decide what letter where looking
    // at, regardless of casing, and replace it with the corresponsing letter of the cipher key
    for (int i = 0, n = strlen(input_string); i < n; i++)
    {
        int current_char = input_string[i];
        // printf("Current char: %i\n", current_char);

        // Find out if the current char is upper or lower and act accordingly
        if (65 <= current_char && current_char <= 90)
        {
            char cipher_letter = get_cipher_letter(64, current_char, cipher_key);

            // Replace the original letter of the plain text string with this new cipher letter
            input_string[i] = cipher_letter;
        }
        else if (97 <= current_char && current_char <= 122)
        {
            char cipher_letter = get_cipher_letter(96, current_char, cipher_key);
            input_string[i] = cipher_letter;
        }
        // printf("Cipher string: %s\n", cipher_string);
    }

    return input_string;
}
