// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // For strcasecmp

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 500;

// Hash table
node *table[N];

// Size of tash table
unsigned int SIZE;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // Hash the word to obtain its index in the hash table
    unsigned int index = hash(word);

    // Go to the obtained index and traverse the linked list
    for (node *trav_ptr = table[index]; trav_ptr != NULL; trav_ptr = trav_ptr->next)
    {
        // If the word we're pointing at and the word we've hashed are case-insensitively the same,
        // we return true
        if (strcasecmp(trav_ptr->word, word) == 0)
            return true;
    }

    // If we reach here, the word is not in the hash table, thus not in the dictionary
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO: Improve this hash function
    int ascii_sum = 0;

    for (int i = 0, n = strlen(word); i < n; i++)
    {
        ascii_sum += tolower(word[i]);
    }

    return ascii_sum % N;
    // return toupper(word[0]) - 'A';
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // TODO
    bool loaded = false;

    // Load the dictionary
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return loaded;
    }

    // Prepare the hash table by filling it with pointers all pointing to NULL
    for (int i = 0; i < N; i++)
    {
        // node *null_ptr = NULL;
        table[i] = NULL;
    }

    // Word for word, hash it and store it in its correct bucket in our hash table by prepending
    char word[LENGTH + 1];
    while (fscanf(dict, "%s", word) != EOF)
    {
        // Allocate space for the new hash table node
        node *n = malloc(sizeof(node));
        if (n == NULL)
        {
            printf("Could not allocate size for node in hash table.\n");
            return false;
        }

        // Copy the string from word as to not assign the address of the local variable
        strcpy(n->word, word);

        // Hash the word to obtain its index in the array
        unsigned int index = hash(word);

        // If there already is a node in the bucket, point to the node and re-direct the previous
        // pointer
        if (table[index] != NULL)
        {
            n->next = table[index];
            table[index] = n;
        }

        // If there is not a node in the bucket we can simply insert our node and have it point to
        // NULL
        else
        {
            n->next = NULL;
            table[index] = n;
        }

        // Increment the size variable
        SIZE++;
    }

    // If we've reached this line of code in the function, we're succesful
    loaded = true;

    // Close the dictionary file
    fclose(dict);

    return loaded;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // If the hash table only contains pointers to NULL, it's not yet loaded
    bool loaded = false;
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            loaded = true;
        }
    }

    if (!loaded)
        return 0;

    return SIZE;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    bool unloaded = false;

    // First make sure that the hash table is loaded
    bool loaded = false;
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            loaded = true;
        }
    }

    if (!loaded)
    {
        printf("Hash table not loaded.\n");
        return unloaded;
    }

    // Traverse every bucket in the hash table
    for (int i = 0; i < N; i++)
    {
        // For every bucket, traverse the linked list
        // We use a while loop instead of a for loop as to not modify the list as we're traversing
        // it
        node *trav_ptr = table[i];
        while (trav_ptr != NULL)
        {
            // Set a temporary pointer to the current node, move the main pointer, and free the
            // current node
            node *tmp = trav_ptr;
            trav_ptr = trav_ptr->next;
            free(tmp);
        }
    }

    // If we reach this line of code, we succeeded
    unloaded = true;

    return unloaded;
}
