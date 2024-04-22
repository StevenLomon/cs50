#include <cs50.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
bool in_pairs_array (pair pair_array[], int a, int b, int current_pair_count);
void add_pairs(void);
int get_strength_of_victory(int indx);
void sort_pairs(void);
bool reachable_from_itself(int start, int current);
void lock_pairs(void);
int find_source(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }


        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Loop through the candidates array to check if the given name belongs to one of the candidates.
    // We also pick up the index of the candidate and update the ranks array with the index of their
    // ith preference, e.g. when i is 0 we set ranks[0] to 1 if Alice were to have index 1 in the
    // candidates array and their top choice is Alice
    bool valid_name = false;
    int candidate_index;
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            valid_name = true;
            candidate_index = i;
            ranks[rank] = candidate_index;
        }
    }

    return valid_name;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // The ranks array comes in the form of ith preference:index of candidate
    // Since the last candidate is not preferred over anyone, we only go to the next
    // to last element of the ranks array to not step outside the boundaries of the array
    for (int i = 0; i < candidate_count - 1; i++)
    {
        int candidate = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int other_candidate = ranks[j];
            preferences[candidate][other_candidate]++;
        }
    }
    return;
}

// Helper function to check if a potential pair of integers a and b is alreayd in the global pairs array
bool in_pairs_array (pair pair_array[], int a, int b, int current_pair_count)
{
    bool in_array = false;
    for (int i = 0; i < current_pair_count; i++)
    {
        pair current_pair = pairs[i];
        if (current_pair.winner == a && current_pair.loser == b)
            in_array = true;
    }
    return in_array;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int pair_index = 0, winner = 0, loser = 0;

    // Compare [0,1] with [1,0], [0,2] with [2,0], [1,2] with [2,1] and so on
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            // If i and j are not the same, there will be a winner and there will be a loser
            // For example, if [0][1] is 3 (pref1) and [1][0] is 1 (pref2), that means that 3 people prefer Alice
            // over Bob and only 1 person prefer Bob over Alice. Alice is the winner. We don't bother changing
            // in the pairs array if there is a tie
            if (i != j)
            {
                int pref1 = preferences[i][j];
                int pref2 = preferences[j][i];

                if (pref1 == pref2)
                    continue;
                else if (pref1 > pref2)
                {
                    winner = i;
                    loser = j;
                }
                else if (pref1 < pref2)
                {
                    winner = j;
                    loser = i;
                }

                // Check to see if the pair is not already in the pairs array. We only need to do this if there
                // actually is a winner and a loser, i.e. they are not equal, and also is pair_count is already
                // bigger than the candidate count - 1 (we've iterated over the first row representing the first candidate)
                // It's also only if this is satisfied we increment pair_count
                if (winner != loser)
                {
                    if (!in_pairs_array(pairs, winner, loser, pair_count))
                    {
                        pairs[pair_index].winner = winner;
                        pairs[pair_index].loser = loser;
                        pair_count++;
                        pair_index++;
                    }
                }
            }
        }
    }
    return;
}

int get_strength_of_victory(int indx)
{
    // Strength of victory is the margin of victory
    // Get the winner and loser count by finding the specific count in the preferences array
    int winner_count = preferences[pairs[indx].winner][pairs[indx].loser];
    int loser_count = preferences[pairs[indx].loser][pairs[indx].winner];

    return (winner_count - loser_count);
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Loop through the pairs array and compare it with the preferences array that will give
    // us the data needed to calculate the strength of victory
    // While looping we will also sort using Bubble Sort since it's simple and the array
    // size is only of max size 9

    // Variable i to keep track of number of passes we've made through the array
    // If we go through an entire pass and there were no swaps, we are done! The array is sorted
    int i = 0;
    bool swapped = false;
    do
    {
        // pair_count - 1 so that we don't compare with NULL, and additionally - i to shorten the scope we
        // compare with each pass, since the largest element will "bubble up" to the end of the array
        swapped = false;
        for (int j = 0; j < (pair_count - 1 - i); j++)
        {
            // Compare the strength of victory for the two adjacent pairs in the array
            // Less than since we want it to be in decreasing order
            if (get_strength_of_victory(j) < get_strength_of_victory(j+1))
            {
                // If out of order, perform the swap
                pair temp = pairs[j];
                pairs[j] = pairs[j+1];
                pairs[j+1] = temp;
                swapped = true;
            }
        }
        i++;
    } while (swapped);
    return;
}

bool reachable_from_itself(int start, int current)
{
    if (start == current)
        return true;
    // For each candidate c that current points to
    // Find which candidates c points to by going through the row looking for true
    for (int i = 0; i < candidate_count; i++)
        if (locked[current][i])
            {
                int c = i;
                if (reachable_from_itself(start, c))
                    return true;
                }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Pairs is now sorted by strength of victory so loop through it and change the locked 2D array accordingly
    for (int i = 0; i < pair_count; i++)
    {
        // We've created a cycle if we can start at a candidate and reach itself by following
        // all arrows starting from it. If a candidate is not reachable from itself after adding
        // the new pair, it's safe to lock the pair

        // The edge that is about to be locked
        int curr_winner = pairs[i].winner;
        int curr_loser = pairs[i].loser;

        if (!reachable_from_itself(curr_winner, curr_loser))
            locked[curr_winner][curr_loser] = true;
    }


    // pair prev; // To keep track of the previous pair
    // prev.winner = pairs[0].winner;
    // prev.loser = pairs[0].loser;
    // bool consecutive = false; // To make sure we don't create cycles
    // int consecutive_count = 0; // We create a cycle if consecutive_count is to reach candidate_count and consecutive is true

    //
    // for (int i = 0; i < pair_count; i++)
    // {
    //     consecutive = false;
    //     int curr_winner = pairs[i].winner; //This represents i in the locked array and the winner in the ith pair (current pair)
    //     int curr_loser = pairs[i].loser; //This represents j in the locked array and the loser in the ith pair

    //     // We can not have a consecutive chain on the first round of the loop
    //     if (i != 0 && (prev.loser == curr_winner || curr_loser == prev.winner))
    //         {
    //             consecutive = true;
    //             consecutive_count++;
    //         }

    //     // Update the prev pair
    //     prev.winner = curr_winner;
    //     prev.loser = curr_loser;

    //     // We only lock if we have made sure it won't create a cycle
    //     if (consecutive && consecutive_count == candidate_count - 1)
    //         continue;
    //
    // }
    return;
}

int find_source(void)
{
    int source = -1;

    // The source will be the one who has zero edges pointing to it, meaning it doesn't appear
    // as a loser in a single pair
    int losers[candidate_count]; // Array to mark each loser in the pairs
    // First, a loop throught the array set the current trash values to 0
    for (int i = 0; i < candidate_count; i++)
        losers[i] = 0;

    // Then increment at the correct candidate index corresponding to the loser in the pairs array
    // We unfortunacannot do this in one single loop
    for (int i = 0; i < pair_count; i++)
        losers[pairs[i].loser]++;

    // Now find the candidate that still has a value of 0 in the losers array. It will be
    // the source
    for (int i = 0; i < candidate_count; i++)
        if (losers[i] == 0)
            {
                source = i;
                break;
            }

    return source;
}

// Print the winner of the election
void print_winner(void)
{
    int winner_index = find_source();

    // int winner_index = 0;
    // int true_count = 0;
    // for (int i = 0; i < candidate_count; i++)
    // {
    //     true_count = 0;
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         if (locked[i][j])
    //             true_count++;
    //         if (true_count == candidate_count - 1)
    //             winner_index = i;
    //     }
    // }
    printf("%s\n", candidates[winner_index]);

    return;
}
