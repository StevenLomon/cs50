#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
} candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    // TODO
    // Loop through all the candidates to see if the name is a valid candidate
    // If found, record the preference
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            // Store the index of the candidate at the correct location in preferences[i][j]
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    // TODO
    // Loop through all the voters and get each voter's top candidate
    // Given that this top candidate is not eliminated, increment theirs votes by one
    // Update the voter's top choice if their previous top choice has been eliminated

    for (int i = 0; i < voter_count; i++)
    {
        // Loop through this voter's preferences to get the top preference candidate that is not
        // eliminated
        for (int j = 0; j < candidate_count; j++)
        {
            int top_candidate_index = preferences[i][j];
            // If the voter's top candidate is not eliminated, increment the votes for that
            // candidate by one and don't continue down the ballot (break out of the loop)
            if (!candidates[top_candidate_index].eliminated)
            {
                candidates[top_candidate_index].votes++;
                break;
            }
        }
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    // TODO
    // Loop through all the candidates to check if any candidate has more than half of the votes.
    // We get this number using voter_count
    float winner_count = (float) voter_count / 2;
    // printf("Winner count is %f\n", winner_count);

    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > winner_count)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    // TODO
    // Loop through all the candidates to find the one that still isn't eliminated and has the
    // fewest amount of votes
    int fewest_votes = voter_count; // Initialize as votes_count and have it go lower from there

    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated && candidates[i].votes < fewest_votes)
            fewest_votes = candidates[i].votes;
    }

    return fewest_votes;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    // TODO
    // Loop through all of the candidates to check if a single non-eliminated candidate has more
    // votes than the minimum If, and only if, all of the remaining non-eliminated candidates have
    // this minimum as their amount of votes do we call it a tie and return true, otherwise false
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated && candidates[i].votes > min)
            return false;
    }
    return true;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{
    // TODO
    // Loop through all of the candidates to find the one with the corresponding min value of fewest
    // votes and set the eliminated bool to true
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == min)
            candidates[i].eliminated = true;
    }

    return;
}
