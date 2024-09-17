from datetime import datetime # For validation and formating

months = {
    "January": 1,
    "February": 2,
    "March": 3,
    "April": 4,
    "May": 5,
    "June": 6,
    "July": 7,
    "August": 8,
    "September": 9,
    "October": 10,
    "November": 11,
    "December": 12
}



while True:
    try:
        user_date = input("Date: ").strip() # Make sure we get a string with no leading or trailing whitespace

        # Try MM/DD/YYYY format
        if "/" in user_date and user_date.count("/") == 2:
            month, day, year = [int(part) for part in user_date.split("/")] # Unpack and name in one simple line


        # Try 'Month DD, YYYY' format
        elif "," in user_date and user_date.count(" ") == 2:
            month_str, day, year = user_date.replace(",", "").split() # Remove the comma and then split by space
            month = months.get(month_str)
            day = int(day)
            year = int(year)

        # Validate and format the parsed date using datetime
        formatted_date = datetime(year, month, day).strftime("%Y-%m-%d")
        print(formatted_date)
        break

    except (ValueError, KeyError, NameError): # Handle invalid dates or month names
        continue
    except EOFError:
        print()
        break

# If not using datetime for validation
# def is_valid_date(month, day):
#     # Simple validation assuming all months have 31 days
#     if 1 <= month <= 12 and 1 <= day <= 31:
#         return True
#     return False
