#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int text_condition_count(string text, int (*condition)(int));
int number_of_letters(string input_text);
int is_space(int c);
int number_of_words(string input_text);
int is_sentence_stopper(int c);
int number_of_sentences(string input_text);
int calculate_Coleman_Liau_index(string input_text);

int main(void)
{
    string user_text = get_string("Text: ");

    int coleman_liau_index = calculate_Coleman_Liau_index(user_text);
    if (coleman_liau_index < 1)
        printf("Before Grade 1\n");
    else if (coleman_liau_index > 16)
        printf("Grade 16+\n");
    else
        printf("Grade %i\n", coleman_liau_index);

    return 0;
}

// Helper function to modulrize the loop and avoid repetition. I had the idea myself but duck.ai
// helped me write the function arguments and implement it in the other functions :))
int text_condition_count(string text, int (*condition)(int))
{
    int counter = 0;
    for (int i = 0, n = strlen(text); i < n; i++)
    {
        if (condition(text[i]))
            counter += 1;
    }

    return counter;
}

int number_of_letters(string input_text)
{
    // We calculate the number of letters by counting every instance of a-z or A-Z
    int letter_count = text_condition_count(input_text, isalpha);

    return letter_count;
}

int is_space(int c)
{
    return c == ' ';
}

int number_of_words(string input_text)
{
    // We calculate the number of words by counting the number of spaces;
    // If a sentence has three spaces, it will contain four words
    int word_count = text_condition_count(input_text, is_space) + 1;

    return word_count;
}

int is_sentence_stopper(int c)
{
    return (c == '.' || c == '!' || c == '?');
}

int number_of_sentences(string input_text)
{
    // We calculate the number of sentences in a text by counting the number of full stops
    int sentence_count = text_condition_count(input_text, is_sentence_stopper);

    // If the last sentence doesn't end with a full stop, exlamation mark or question mark,
    // increment by one. Also from getting the Alice sentence wrong, if it ends with a " symbol!
    char last_char = input_text[strlen(input_text) - 1];
    if (last_char != '.' && last_char != '!' && last_char != '?' && last_char != '"')
        sentence_count += 1;

    return sentence_count;
}

int calculate_Coleman_Liau_index(string input_text)
{
    /*printf("Number of letters: %i\n", number_of_letters(input_text));
    printf("Number of words: %i\n", number_of_words(input_text));
    printf("Number of sentences: %i\n", number_of_sentences(input_text));*/

    int word_count = number_of_words(input_text);
    if (word_count != 0)
    {
        float L = (number_of_letters(input_text) / (float) word_count) * 100.0;
        float S = (number_of_sentences(input_text) / (float) word_count) * 100.0;
        // printf("L: %f, S:%f\n", L, S);

        float index = 0.0588 * L - 0.296 * S - 15.8;
        // printf("Index before rounding: %f\n", index);

        return round(index);
    }
    else
        return 0;
}
