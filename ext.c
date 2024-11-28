#include "ext.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 256

dict* dict_init(void) {
   // Allocate memory for the root node
   dict* root = (dict*)calloc(1, sizeof(dict));
   if (!root) {
      // Print error message and exit if memory allocation fails
      fprintf(stderr, "Memory allocation failed in dict_init\n");
      exit(EXIT_FAILURE);
   }
   return root;
}

bool dict_addword(dict* p, const char* wd) {
   // Check if inputs are valid
   if (!p || !wd || !*wd) {
      return false;
   }

   dict* current = p;
   while (*wd) {
      // Determine the index for the character
      int index = (*wd == '\'') ? ALPHA - 1 : tolower(*wd) - 'a';
      if (index < 0 || index >= ALPHA) {
         return false;
      }

      // Allocate a new node if necessary
      if (!current->dwn[index]) {
         current->dwn[index] = (dict*)calloc(1, sizeof(dict));
         if (!current->dwn[index]) {
            fprintf(stderr, "Memory allocation failed in dict_addword\n");
            exit(EXIT_FAILURE);
         }
         current->dwn[index]->up = current;
      }
      current = current->dwn[index];
      wd++;
   }

   // Mark the node as a terminal node for the word
   if (current->terminal) {
      current->freq++;
      return false;
   }

   current->terminal = true;
   current->freq = 1;
   return true;
}

void dict_free(dict** d) {
   // Check if the dictionary pointer is valid
   if (!d || !*d) {
      return;
   }

   dict* current = *d;
   // Recursively free all child nodes
   for (int i = 0; i < ALPHA; i++) {
      dict_free(&current->dwn[i]);
   }
   // Free the current node and nullify the pointer
   free(current);
   *d = NULL;
}

bool dict_fuzzy_match(const dict* p, const char* target, int max_diff, char* best_match) {
   if (!p || !target || !best_match) {
      return false;
   }

   char buffer[BUFFER_SIZE] = {0};
   int min_diff = max_diff + 1;
   dict_fuzzy_helper(p, target, buffer, 0, max_diff, best_match, &min_diff);
   return min_diff <= max_diff;
}

static void dict_fuzzy_helper(const dict* p, const char* target, char* buffer, int depth, int remaining_diff, char* best_match, int* min_diff) {
   if (!p || remaining_diff < 0) {
      return;
   }

   if (*target == '\0') {
      if (p->terminal && remaining_diff < *min_diff) {
         buffer[depth] = '\0';
         strcpy(best_match, buffer);
         *min_diff = remaining_diff;
      }
      return;
   }

   for (int i = 0; i < ALPHA; i++) {
      if (p->dwn[i]) {
         buffer[depth] = (i == ALPHA - 1) ? '\'' : 'a' + i;
         int cost = (*target == buffer[depth]) ? 0 : 1;
         dict_fuzzy_helper(p->dwn[i], target + 1, buffer, depth + 1, remaining_diff - cost, best_match, min_diff);
      }
   }

   dict_fuzzy_helper(p, target + 1, buffer, depth, remaining_diff - 1, best_match, min_diff);
}

int dict_wordcount(const dict* p) {
   // Base case: if the dictionary is null, return 0
   if (!p) {
      return 0;
   }

   int count = p->terminal ? 1 : 0;
   // Recursively count words in child nodes
   for (int i = 0; i < ALPHA; i++) {
      count += dict_wordcount(p->dwn[i]);
   }
   return count;
}

bool dict_autocomplete(const dict* p, const char* prefix, char* ret) {
   // Validate inputs
   if (!p || !prefix || !ret) {
      return false;
   }

   const dict* current = p;
   while (*prefix) {
      // Navigate to the node corresponding to the prefix
      int index = (*prefix == '\'') ? ALPHA - 1 : tolower(*prefix) - 'a';
      if (index < 0 || index >= ALPHA || !current->dwn[index]) {
         *ret = '\0';
         return false;
      }
      current = current->dwn[index];
      prefix++;
   }

   char buffer[BUFFER_SIZE] = {0};
   int max_freq = 0;
   // Find the most frequent word starting with the prefix
   dict_autocomplete_helper(current, buffer, 0, ret, &max_freq);
   return max_freq > 0;
}

static void dict_autocomplete_helper(const dict* p, char* buffer, int depth, char* best_word, int* max_freq) {
   if (!p) {
      return;
   }

   // Check if the current node terminates a word
   if (p->terminal && p->freq > *max_freq) {
      buffer[depth] = '\0';
      strcpy(best_word, buffer);
      *max_freq = p->freq;
   }

   // Recursively explore child nodes
   for (int i = 0; i < ALPHA; i++) {
      if (p->dwn[i]) {
         buffer[depth] = (i == ALPHA - 1) ? '\'' : 'a' + i;
         dict_autocomplete_helper(p->dwn[i], buffer, depth + 1, best_word, max_freq);
      }
   }
}

bool dict_save(const dict* p, const char* filename) {
   // Validate inputs
   if (!p || !filename) {
      return false;
   }

   FILE* fp = fopen(filename, "w");
   if (!fp) {
      return false;
   }

   char buffer[BUFFER_SIZE] = {0};
   // Save words to the file using a helper function
   save_helper(p, fp, buffer, 0);
   fclose(fp);
   return true;
}

static void save_helper(const dict* p, FILE* fp, char* buffer, int depth) {
   if (!p) {
      return;
   }

   // Write the word and frequency to the file if it's a terminal node
   if (p->terminal) {
      buffer[depth] = '\0';
      fprintf(fp, "%s %d\n", buffer, p->freq);
   }

   // Recursively save child nodes
   for (int i = 0; i < ALPHA; i++) {
      if (p->dwn[i]) {
         buffer[depth] = (i == ALPHA - 1) ? '\'' : 'a' + i;
         save_helper(p->dwn[i], fp, buffer, depth + 1);
      }
   }
}
