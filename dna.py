# import csv
import sys
import pandas as pd


def main():

    # TODO: Check for command-line usage
    if len(sys.argv) != 3:
        print("Database file and sequence file needed when running!")
        return (1)

    # TODO: Read database file into a variable
    db_file = sys.argv[1]
    df = pd.read_csv(db_file)  # Read database csv into pandas DataFrame

    # TODO: Read DNA sequence file into a variable
    sequence_file = sys.argv[2]
    DNA_sequence = None
    with open(sequence_file, 'r') as file:  # Read DNA sequence file into string
        DNA_sequence = file.read()
    if not DNA_sequence:
        print("Could not read sequence file. Try again.")
        return (1)
    # print(DNA_sequence)

    # TODO: Find longest match of each STR in DNA sequence
    # Get all columns in the DataFrame except 'name'
    STRs = [column for column in df.columns if column != 'name']
    # print(STRs)

    # Get the integer result from longest run for every STR in the list
    runs = [longest_match(DNA_sequence, STR) for STR in STRs]
    # print(f"Runs: {runs}")

    # TODO: Check database for matching profiles
    match = False
    match_index = None
    # Loop through the df temporarily without the 'name' column
    for index, row in df.drop(['name'], axis=1).iterrows():
        # print(f"Row: {row.tolist()}")
        if row.tolist() == runs:  # If we get a match
            match = True
            match_index = index  # Get the index of the matching row

    if match:
        print(df.iloc[match_index]['name'])  # Print the name value of the matching row index
    else:
        print("No match")

    return


def longest_match(sequence, subsequence):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):

        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:

            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
