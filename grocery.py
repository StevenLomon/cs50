shopping_list = {}

while True:
    try:
        item = input().upper() # We convert to uppercase up here to avoid doing it later
        shopping_list[item] = shopping_list.get(item, 0) + 1 # If we can't get the item, it defaults to zero, else we increment it
    except EOFError: # When the user hits ctrl-d
        # Before we break out of the loop and the program, print everything in the shopping list
        for item, count in sorted(shopping_list.items()): # Sort alphabetically by key
            print(f"{count} {item}")
        break

# My first solution
# if item in shopping_list:
#     shopping_list[item] += 1 # If the item is in the shopping list, increment its counter
# else:
#     shopping_list[item] = 1 # If it's not in the shopping list, add it with a counter of 1
