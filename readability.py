def count_letters(text: str) -> int:
    # letter_count = 0
    # for letter in text:
    #     if letter.isalpha():
    #         letter_count += 1
    # return letter_count
    return len([char for char in text if char.isalpha()])


def count_words(text: str) -> int:
    return len([char for char in text if char.isspace()]) + 1


def count_sentences(text: str) -> int:
    return len([char for char in text if char in ['.', '?', '!']])


def Coleman_Liau(input_text: str) -> int:
    L = count_letters(input_text) / count_words(input_text) * 100
    S = count_sentences(input_text) / count_words(input_text) * 100
    return 0.0588 * L - 0.296 * S - 15.8


def main():
    user_text = input("Text: ")
    rounded_index = round(Coleman_Liau(user_text))

    if rounded_index < 1:
        print("Before Grade 1")
    elif rounded_index > 15:
        print("Grade 16+")
    for i in range(1, 16): # For loop to get through cases 1 through 15
        if rounded_index == i:
            print(f"Grade {i}")


main()
