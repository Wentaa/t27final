#include "ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_TABLE_SIZE 10007 // A prime number for hash table size
#define BUFFER_SIZE 256       // Maximum word size

// Structure for hash table node
typedef struct HashNode {
   char* word;            // The word stored in this node
   int freq;              // Frequency of the word
   struct HashNode* next; // Pointer to the next node (chaining for collisions)
} HashNode;

// Structure for hash table
typedef struct HashTable {
   HashNode* table[HASH_TABLE_SIZE]; // Array of pointers to hash nodes
} HashTable;

// Hash function to calculate index
static unsigned int hash_function(const char* word) {
   unsigned int hash = 0;
   while (*word) {
      hash = (hash * 31) + tolower(*word);
      word++;
   }
   return hash % HASH_TABLE_SIZE;
}

// Initialize the hash table
dict* dict_init(void) {
   HashTable* ht = (HashTable*)calloc(1, sizeof(HashTable));
   if (!ht) {
      fprintf(stderr, "Memory allocation failed in dict_init\n");
      exit(EXIT_FAILURE);
   }
   return (dict*)ht;
}

// Add a word to the hash table
bool dict_addword(dict* p, const char* wd) {
   if (!p || !wd || !*wd) {
      return false; // Invalid input
   }

   HashTable* ht = (HashTable*)p;
   char lower_word[BUFFER_SIZE];
   for (int i = 0; wd[i] != '\0'; i++) {
      lower_word[i] = tolower(wd[i]);
   }
   lower_word[strlen(wd)] = '\0';

   unsigned int index = hash_function(lower_word);
   HashNode* current = ht->table[index];

   // Search for the word in the chain
   while (current) {
      if (strcmp(current->word, lower_word) == 0) {
         current->freq++; // Increment frequency if found
         return false;    // Word already exists
      }
      current = current->next;
   }

   // Create a new node for the word
   HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
   if (!new_node) {
      fprintf(stderr, "Memory allocation failed in dict_addword\n");
      exit(EXIT_FAILURE);
   }
   new_node->word = strdup(lower_word);
   new_node->freq = 1;
   new_node->next = ht->table[index];
   ht->table[index] = new_node; // Insert at the beginning of the chain

   return true; // Successfully added a new word
}

// Free the hash table
void dict_free(dict** d) {
   if (!d || !*d) {
      return;
   }

   HashTable* ht = (HashTable*)(*d);
   for (int i = 0; i < HASH_TABLE_SIZE; i++) {
      HashNode* current = ht->table[i];
      while (current) {
         HashNode* temp = current;
         current = current->next;
         free(temp->word); // Free the word
         free(temp);       // Free the node
      }
   }
   free(ht);  // Free the hash table structure
   *d = NULL; // Set pointer to NULL
}

// Count the total number of words in the hash table
int dict_wordcount(const dict* p) {
   if (!p) {
      return 0;
   }

   HashTable* ht = (HashTable*)p;
   int count = 0;

   for (int i = 0; i < HASH_TABLE_SIZE; i++) {
      HashNode* current = ht->table[i];
      while (current) {
         count += current->freq; // Add the frequency of each word
         current = current->next;
      }
   }
   return count;
}

// Check if a word exists in the hash table
dict* dict_spell(const dict* p, const char* wd) {
   if (!p || !wd || !*wd) {
      return NULL; // Invalid input
   }

   HashTable* ht = (HashTable*)p;
   char lower_word[BUFFER_SIZE];
   for (int i = 0; wd[i] != '\0'; i++) {
      lower_word[i] = tolower(wd[i]);
   }
   lower_word[strlen(wd)] = '\0';

   unsigned int index = hash_function(lower_word);
   HashNode* current = ht->table[index];

   while (current) {
      if (strcmp(current->word, lower_word) == 0) {
         return (dict*)current; // Return node if word found
      }
      current = current->next;
   }
   return NULL; // Word not found
}

// Find the frequency of the most common word
int dict_mostcommon(const dict* p) {
   if (!p) {
      return 0;
   }

   HashTable* ht = (HashTable*)p;
   int max_freq = 0;

   for (int i = 0; i < HASH_TABLE_SIZE; i++) {
      HashNode* current = ht->table[i];
      while (current) {
         if (current->freq > max_freq) {
            max_freq = current->freq;
         }
         current = current->next;
      }
   }
   return max_freq;
}

// Placeholder test function for compatibility with driver.
void test(void)
{
   // Testing is done in driverext.c
}
