#ifndef EXT_H
#define EXT_H

#include <stdbool.h>

#define ALPHA 27 // 'a'-'z' and apostrophe

typedef struct dict {
   struct dict* dwn[ALPHA]; // Children nodes
   struct dict* up;         // Parent node
   bool terminal;           // Marks end of a word
   int freq;                // Frequency of the word
} dict;

// Existing function prototypes
dict* dict_init(void);
bool dict_addword(dict* p, const char* wd);
void dict_free(dict** d);
int dict_wordcount(const dict* p);
bool dict_autocomplete(const dict* p, const char* prefix, char* ret);

// New function prototype for fuzzy search
bool dict_fuzzy_match(const dict* p, const char* target, int max_diff, char* best_match);

#endif
