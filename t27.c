#include "t27.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define buffer size for string operations
#define BUFFER_SIZE 256 

dict* dict_init(void)
{
   // Allocate memory for the root node of the dictionary tree.
   // If allocation fails, the program will terminate.
   dict* root = (dict*)calloc(1, sizeof(dict));
   if (!root) {
      fprintf(stderr, "Memory allocation failed in dict_init\n");
      exit(EXIT_FAILURE);
   }

   // Initialize fields of the root node.
   // The root node does not have a parent.
   root->up = NULL;

   // The root node is not the end of any word.
   root->terminal = false;

   // The initial frequency is zero.
   root->freq = 0;

   // Return the pointer to the initialized root node.
   return root;
}


bool dict_addword(dict* p, const char* wd)
{
   // Check for invalid input
   if (!p || !wd || !*wd) {
      return false;
   }

   dict* current = p;

   // Process each character in the word
   while (*wd) {
      int index;

      // Calculate the index for the current character
      if (*wd == '\'') {
         index = 26; // Apostrophe
      } else if (isalpha(*wd)) {
         index = tolower(*wd) - 'a'; // Lowercase letters
      } else {
         return false; // Invalid character
      }

      // If the path doesn't exist, create a new node
      if (!current->dwn[index]) {
         current->dwn[index] = (dict*)calloc(1, sizeof(dict));
         if (!current->dwn[index]) {
            fprintf(stderr, "Memory allocation failed in dict_addword\n");
            exit(EXIT_FAILURE);
         }
         current->dwn[index]->up = current; // Set parent node
      }

      // Move to the next node
      current = current->dwn[index];
      wd++;
   }

   // Check if the word already exists
   if (current->terminal) {
      current->freq++; // Increase frequency
      return false;    // Word already existed
   }

   // Mark the node as the end of a word
   current->terminal = true;
   current->freq = 1; // Initialize frequency

   return true; // Successfully added a new word
}

void dict_free(dict** d)
{
   // Check if the pointer to the dictionary or its content is NULL.
   if (d == NULL) {
      return;
   }
   if (*d == NULL) {
      return;
   }

   dict* current = *d;

   // Recursively free all child nodes.
   for (int i = 0; i < ALPHA; i++) {
      dict_free(&current->dwn[i]);
   }

   // Free the current node.
   free(current);

   // Set the dictionary pointer to NULL to avoid dangling pointers.
   *d = NULL;
}

int dict_wordcount(const dict* p)
{
   // Base case: If the node is NULL, it does not contribute to the count.
   if (p == NULL) {
      return 0;
   }

   // Initialize the count with the frequency of this node, if it is terminal.
   int count = 0;
   if (p->terminal) {
      count = p->freq;
   }

   // Recursively add the word counts of all child nodes.
   for (int i = 0; i < ALPHA; i++) {
      count += dict_wordcount(p->dwn[i]);
   }

   // Return the total word count for this node and its children.
   return count;
}


int dict_nodecount(const dict* p)
{
   // Base case: If the node is NULL, it does not contribute to the count.
   if (p == NULL) {
      return 0;
   }

   // Initialize the count for the current node.
   int count = 1;

   // Recursively add the counts of all child nodes.
   for (int i = 0; i < ALPHA; i++) {
      count += dict_nodecount(p->dwn[i]);
   }

   // Return the total node count for this node and its children.
   return count;
}


/**
 * Checks if a word exists in the dictionary tree.
 * Returns the terminal node if the word exists, NULL otherwise.
 */
dict* dict_spell(const dict* p, const char* str)
{
   // Return NULL if the input dictionary or string is invalid
   if (!p || !str || !*str) {
      return NULL;
   }

   const dict* current = p;

   // Traverse the tree character by character
   while (*str) {
      // Calculate the index for the current character
      int index = (*str == '\'') ? 26 : tolower(*str) - 'a';

      // If the index is out of bounds or the child node doesn't exist, return NULL
      if (index < 0 || index >= ALPHA || !current->dwn[index]) {
         return NULL;
      }

      // Move to the next node
      current = current->dwn[index];
      str++;
   }

   // Return the node if it is a terminal node, otherwise NULL
   return current->terminal ? (dict*)current : NULL;
}


int dict_mostcommon(const dict* p)
{
   // Base case: If the node is NULL, it cannot have a frequency.
   if (!p) {
      return 0;
   }

   // Start with the frequency of this node if it is terminal.
   int max_freq = p->terminal ? p->freq : 0;

   // Recursively find the maximum frequency in child nodes.
   for (int i = 0; i < ALPHA; i++) {
      int child_freq = dict_mostcommon(p->dwn[i]);
      if (child_freq > max_freq) {
         max_freq = child_freq;
      }
   }

   // Return the maximum frequency found.
   return max_freq;
}


// CHALLENGE1
unsigned dict_cmp(dict* p1, dict* p2)
{
   if (!p1 || !p2) {
      return 0; // If either node is NULL, return 0
   }

   int depth1 = 0, depth2 = 0;
   dict* temp;

   // Calculate the depth of p1
   for (temp = p1; temp; temp = temp->up) {
      depth1++;
   }

   // Calculate the depth of p2
   for (temp = p2; temp; temp = temp->up) {
      depth2++;
   }

   // Calculate the total distance
   unsigned total_distance = 0;

   // Bring both nodes to the same depth
   while (depth1 > depth2) {
      p1 = p1->up;
      depth1--;
      total_distance++;
   }
   while (depth2 > depth1) {
      p2 = p2->up;
      depth2--;
      total_distance++;
   }

   // Move both nodes up until they meet at a common ancestor
   while (p1 != p2) {
      p1 = p1->up;
      p2 = p2->up;
      total_distance += 2; // Increment for each step up
   }

   return total_distance;
}

// CHALLENGE2
// Static helper function declaration
static void dict_autocomplete_helper(const dict* p, char* buffer, int depth, char* best_word, int* max_freq);

void dict_autocomplete(const dict* p, const char* wd, char* ret)
{
   if (!p || !wd) {
      *ret = '\0'; // Invalid input, return empty string
      return;
   }

   const dict* current = p;

   // Traverse the prefix
   while (*wd) {
      int index = (*wd == '\'') ? ALPHA - 1 : tolower(*wd) - 'a';
      if (index < 0 || index >= ALPHA || !current->dwn[index]) {
         *ret = '\0'; // Prefix not found
         return;
      }
      current = current->dwn[index];
      wd++;
   }

   // Buffer for constructing the word and tracking max frequency
   char buffer[BUFFER_SIZE] = {0}; // Use defined BUFFER_SIZE
   strcpy(buffer, wd);             // Start buffer with prefix
   int depth = strlen(wd);         // Set initial depth as prefix length
   int max_freq = 0;

   // Initialize best_word to empty
   char best_word[BUFFER_SIZE] = {0};

   // Recursive search for the most frequent word
   dict_autocomplete_helper(current, buffer, depth, best_word, &max_freq);

   // Extract the additional letters required
   if (strlen(best_word) > depth) {
      strcpy(ret, &best_word[depth]);
   } else {
      *ret = '\0'; // No additional letters
   }
}

static void dict_autocomplete_helper(const dict* p, char* buffer, int depth, char* best_word, int* max_freq)
{
   if (!p) {
      return;
   }

   // Recursively explore child nodes (skip the current node itself)
   for (int i = 0; i < ALPHA; i++) {
      if (p->dwn[i]) {
         buffer[depth] = (i == ALPHA - 1) ? '\'' : 'a' + i; // Append the current character
         dict_autocomplete_helper(p->dwn[i], buffer, depth + 1, best_word, max_freq);
      }
   }

   // Check if the current node terminates a word and is a valid candidate
   if (p->terminal && depth > 0 && p->freq > *max_freq) {
      buffer[depth] = '\0'; // Null-terminate the buffer
      strcpy(best_word, buffer); // Update best_word
      *max_freq = p->freq;       // Update max frequency
   }
}




void test(void)
{
   // Initialize the dictionary
   dict* my_dict = dict_init();
   assert(my_dict != NULL);

   // Test adding words
   assert(dict_addword(my_dict, "car"));
   assert(dict_addword(my_dict, "cart"));
   assert(dict_addword(my_dict, "part"));
   assert(!dict_addword(my_dict, "car")); // Duplicate word

   // Test word count
   int word_count = dict_wordcount(my_dict);
   assert(word_count == 4); 

   // Test node count
   int node_count = dict_nodecount(my_dict);
   assert(node_count ==9);

   // Test spell checking
   dict* found = dict_spell(my_dict, "car");
   assert(found != NULL && found->terminal);

   found = dict_spell(my_dict, "dog");
   assert(found == NULL); // "dog" not in the dictionary

   // Test most common word frequency
   int most_common_freq = dict_mostcommon(my_dict);
   assert(most_common_freq == 2); // "car" was added twice

   // Test dict_cmp
   dict* node1 = dict_spell(my_dict, "car");
   dict* node2 = dict_spell(my_dict, "part");
   assert(node1 != NULL && node2 != NULL);
   unsigned distance = dict_cmp(node1, node2);

   // Test autocomplete
   char result[256];
   dict_autocomplete(my_dict, "ca", result);
   assert(strcmp(result, "r") == 0);


   dict_autocomplete(my_dict, "dog", result);
   assert(result[0] == '\0'); // No autocomplete suggestions for "dog"

   // Free the dictionary
   dict_free(&my_dict);
   assert(my_dict == NULL);
}
