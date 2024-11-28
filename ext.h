#ifndef EXT_H
#define EXT_H

#include <stdbool.h> 
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <assert.h>  
#include "ext.h"     

// Define the dictionary type as a hash table
typedef struct HashTable dict;

// Function prototypes
dict* dict_init(void);                        // Initialize a hash table
bool dict_addword(dict* p, const char* wd);   // Add a word to the hash table
void dict_free(dict** p);                     // Free the hash table
int dict_wordcount(const dict* p);            // Count the total words
dict* dict_spell(const dict* p, const char* wd); // Check if a word exists
int dict_mostcommon(const dict* p);           // Find the frequency of the most common word

#endif // EXT_H
