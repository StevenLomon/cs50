import random

def main():
    score = 0
    level = get_level()

    for _ in range(10): # Loop for 10 problems. No outer While True loop with a break condition needed
        int1 = generate_integer(level)
        int2 = generate_integer(level)
        current_lives = 3

        while current_lives > 0: # End condition to break out of the current problem loop
            try:
                # print(f"Lives: {current_lives}, Score: {score}, Processed problems: {processed_problems}") FOR DEBUGGING
                user_input = int(input(f"{int1} + {int2} = "))

                if user_input == int1 + int2: # If correct input
                    score += 1
                    break # Break out of the current problem loop and on to the next problem

            # Handle both wrong answers and invalid input
            except ValueError:
                pass
            print("EEE")
            current_lives -= 1

        # If all three lives for the current problem becomes 0
        if current_lives == 0:
            print(f"{int1} + {int2} = {int1 + int2}")

    # Print the final score after all 10 problems
    print(f"Score: {score}")

def get_level():
    while True:
        try:
            level = int(input("Level: "))
            if 1 <= level <= 3:
                return level
        except ValueError:
            pass

def generate_integer(level):
    ranges = {1: (0, 9), 2: (10, 99), 3: (100, 999)}
    return random.randint(*ranges[level])

if __name__ == "__main__":
    main()

# def generate_integer(level):
#     # If level == 1 -> between 0 and 9
#     # If level == 2 -> between 10 and 99
#     # If level == 3 -> between 100 and 999
#     if level == 1:
#         return random.randint(0,9)
#     elif level == 2:
#         return random.randint(10,99)
#     elif level == 3:
#         return random.randint(100,999)
#     else:
#         raise ValueError

# else: # If incorrect input
#     print("EEE")
#     current_lives -= 1

# except ValueError: # If non-number input
# print("EEE")
# current_lives -= 1
