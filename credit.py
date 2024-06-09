def get_int(prompt):
    while True:
        try:
            return int(input(prompt))
        except ValueError:
            pass


def get_digit_sum(n: int) -> int:  # Get the sum of all digits in an integer
    sum = 0
    for digit in str(n):
        sum += int(digit)
    return sum


def valid_card_number(card_number: str) -> bool:
    checksum = 0

    # Create a list of ints from the card number string of digits and reverse it
    number_list = [int(digit) for digit in card_number]
    number_list.reverse()
    # print(number_list)

    # With the reverse card_number, take every other digit starting from the second element. These gets multiplied by 2
    number_list_subsection = number_list[1::2]
    # print(number_list_subsection)

    # Add all sums where we have single digit numbers
    checksum += sum([digit*2 for digit in number_list_subsection if digit*2 < 10])

    # Add all sums where we have double (or more) digit numbers
    for number in [digit*2 for digit in number_list_subsection if digit*2 > 9]:
        checksum += get_digit_sum(number)

    # Add the sum of the digits we didn't include in the subsection (every other digit starting from the beginning)
    checksum += sum(number_list[::2])

    # print(f"Checksum: {checksum}")
    # If the last digit of the checksum is 0, the card number is valid
    if str(checksum)[-1] == '0':
        return True
    return False


def isAMEX(card_number: str) -> bool:
    if len(card_number) == 15 and card_number[0:2] in ['34', '37']:
        return True
    return False


def isMasterCard(card_number: str) -> bool:
    if len(card_number) == 16 and card_number[0:2] in [str(n) for n in range(51, 56)]:
        return True
    return False


def isVisa(card_number: str) -> bool:
    if len(card_number) in [13, 16] and card_number[0] == '4':
        return True
    return False


def main():  # Driver code
    user_number = input("Number: ")

    flag = False  # We only print INVALID if the flag variable is never set to True
    if valid_card_number(user_number):
        if isAMEX(user_number):
            print("AMEX")
            flag = True
        elif isMasterCard(user_number):
            print("MASTERCARD")
            flag = True
        elif isVisa(user_number):
            print("VISA")
            flag = True
    if not flag:
        print("INVALID")


main()
