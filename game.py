import random

# Get the level
while True:
    try:
        level = int(input("Level: "))
        if level >= 1:
            break
    except ValueError:
        pass

# Generate the magic number: a random integer between 1 and 100
magic_number = random.randint(1,level)

# Start the guessing game
while True:
    try:
        guess = int(input("Guess: "))
        if guess < magic_number:
            print("Too small!")
        elif guess > magic_number:
            print("Too large!")
        else:
            print("Just right!")
            break
    except ValueError:
        pass
