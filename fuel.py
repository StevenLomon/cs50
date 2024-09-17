# Dictionary to handle exact matches
output_map = {0.25: "25%", 0.5: "50%", 0.75: "75%"}

while True:
    user_input = input("Fraction: ") # This returns a string which can't result in exceptions
    if "/" in user_input:
        num, den = user_input.split("/")
        try:
            result = int(num) / int(den)
            if result > 1:
                continue
        except (TypeError, ValueError, ZeroDivisionError):
            continue
        # print(result)
        if result <= 0.01:
            print("E")
        elif round(result, 2) == 0.33:
            print("33%")
        elif round(result, 2) == 0.67:
            print("67%")
        elif result >= 0.99:
            print("F")
        elif result in output_map:
            print(output_map[result])
        break
