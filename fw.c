#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "fw.h"

/* word holds a number of word occurences (freq) and a string word (val) */
struct word {
    int freq;
    char *val;
};

int main(int argc, char *argv[]) {
    /* -n handle */
    struct word *hash = (struct word*)malloc(sizeof(struct word) * 1000);
    if (!strcmp(argv[1], "-n") && argc >= 3) {
        if (isdigit(argv[2])) {

        }
        else {
            fprintf(stderr, "usage: fw[-n num] [file 1 [file 2 ...]]");
            return 1;
        }
    }
    /* file inputs */
    else if (argc > 1) {
        int i;
        for (i = 1; i < argc; i++) {

        }
    }
    /* standard input */
    else {
        
    }
    return 0;
}

/* Function compares two input word structs, returns 1 if word 1 is
greater than word 2, 0 otherwise.  A word is greater if it has a
higher frequency or if frequencies are the same, the later alphabetically */

int word_comp(struct word *word1, struct word *word2) {
    if (word1->freq > word2->freq) {
        return 1;
    }
    else if (word1->freq < word2->freq) {
        return -1;
    }
    else if (strcmp(word1->val, word2->val) > 0) {
        return 1;
    }
    else if (strcmp(word1->val, word2->val) < 0) {
        return -1;
    }
    else {
        return 0;
    }
}

/* Hash building function that takes in a file and an allocated
memory space to store words and alters the same memory space with
all words in the file added, reallocated if necessary*/

void add_words(FILE *infile, void *hash_table, int *cap) {
    char *in_word = malloc(0);
    int size = 0;
    int word_cap = 0;
    int c = fgetc(infile);
    while (c != EOF) {
        if (isalpha(c)) {
            size++;
            if (size > word_cap) {
                word_cap += 100;
                in_word = realloc(in_word, sizeof(char) * word_cap);
            }
            in_word[size - 1] = c;
        }
        else {
            size++;
            if (size > word_cap) {
                word_cap += 1;
                in_word = realloc(in_word, sizeof(char) * word_cap);
            }
            in_word[size - 1] = '\0';
            if (size > 0) {
                int loc = hash(in_word) % *cap;
                if (hash_table[loc] == NULL) {
                    struct word *new_word = (struct word*)malloc(sizeof(struct word));
                    new_word->val = in_word;
                    new_word->freq = 1;
                    hash_table[loc] = new_word;
                    size = 0;
                    word_cap = 0;
                }
                else if (hash_table[loc] != NULL && !strcmp(in_word, hash_table[loc]->val)){
                    hash_table[loc]->freq++;
                }
                else {
                    struct word *new_word = (struct word*)malloc(sizeof(struct word));
                    new_word->val = in_word;
                    new_word->freq = 1;
                    do {
                        rehash(hash_table, cap);
                        int loc = hash(new_word->val);
                    } while (hash_table[loc] != NULL);
                    hash_table[loc] = new_word;
                }
            }
        }
        c = fgetc(infile);
    }
}

void rehash(void *hash, int *cap) {
    return NULL;
}

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
