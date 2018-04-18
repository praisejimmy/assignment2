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

    int i;
    int cap = 1000;
    int *cap_ptr = &cap;
    int num_results = 10;

    /* initialize each entry to be an empty struct */
    for(i = 0; i < 1000; i++){
        hash[i].val = NULL;
        hash[i].freq = 0;
    }

    if (argc >= 3 && !strcmp(argv[1], "-n")) {
        if (isdigit(argv[2])) {
            num_results = atoi(argv[2]);
        }
        else {
            fprintf(stderr, "usage: fw[-n num] [file 1 [file 2 ...]]\n");
            return 1;
        }
    }
    /* file inputs */
    if (argc > 1) {
        int i = 0;
        if (!strcmp(argv[1], "-n") && isdigit(argv[2]))
            i = 3;
        else
            i = 1;
        for (; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if(file == NULL)
                fprintf(stderr, "%s: No such file or directory\n", argv[i]);
            else
                add_words(file, hash, cap_ptr);
            fclose(file);
        }
    }
    /* standard input */
    else {
        
    }

    /* sort the hash table */
    qsort(hash, cap, sizeof(struct word), word_comp);

    for(i = 0; i < cap; i++){
        if(hash[i].val != NULL)
            printf("%s\t freq : %d\n", hash[i].val, hash[i].freq);
    }
    /* free everything? */
    free(hash); 

    return 0;
}

/* Function compares two input word structs, returns 1 if word 1 is
greater than word 2, 0 otherwise.  A word is greater if it has a
higher frequency or if frequencies are the same, the later alphabetically */

int word_comp(const void *word1, const void *word2) {
    int freq1 = ((struct word *)word1)->freq;
    int freq2 = ((struct word *)word2)->freq;
    char *val1 = ((struct word *)word1)->val;
    char *val2 = ((struct word *)word2)->val;

    if (freq1 > freq2) {
        return -1;
    }
    else if (freq1 < freq2) {
        return 1;
    }
    else if (val1 == 0){
        return 1;
    }
    else if (val2 == 0){
        return -1;
    }
    else if (strcmp(val1, val2) > 0) {
        return -1;
    }
    else if (strcmp(val1, val2) < 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/* Hash building function that takes in a file and an allocated
memory space to store words and alters the same memory space with
all words in the file added, reallocated if necessary*/

void add_words(FILE *infile, struct word *hash_table, int *cap) {
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
        else if(size > 0){
            size++;
            if (size > word_cap) {
                word_cap += 1;
                in_word = realloc(in_word, sizeof(char) * word_cap);
            }
            in_word[size - 1] = '\0';
            int loc = hash(in_word) % *cap;
            size = 0;
            word_cap = 0;
            if (hash_table[loc].val == NULL) {
                struct word new_word;/* = (struct word*)malloc(sizeof(struct word));*/
                new_word.val = in_word;
                new_word.freq = 1;
                hash_table[loc] = new_word;
                printf("\n%s was null with hash of : %d\n", hash_table[loc].val, loc);
            }
            else if (hash_table[loc].val != NULL && in_word != NULL && !strcmp(in_word, hash_table[loc].val)){
                hash_table[loc].freq++;
                printf("\n%swas equal with val of : %s\n", hash_table[loc].val, hash_table[loc].val);
            }
            else {
                struct word new_word;/* = (struct word*)malloc(sizeof(struct word));*/
                new_word.val = in_word;
                new_word.freq = 1;
                /*do {
                    rehash(hash_table, cap);
                    int loc = hash(new_word.val);
                } while (hash_table[loc].val != NULL);*/
                hash_table[loc] = new_word;
                printf("\n%swas equal with hash of : %d\n",hash_table[loc].val, loc);
            }            
        }
        c = fgetc(infile);
    }
    free(in_word);
}

void rehash(struct word *hash_table, int *cap) {
    int i;
    /* Create new hash table */
    struct word *hash_copy = (struct word*)malloc(sizeof(struct word) * 2 *(*cap));

    /* Move over old elements into new spots in the new hash table */
    for (i = 0; i < *cap; i++){
        hash_copy[hash(hash_table[i].val) % (2 * (*cap))] = hash_table[i];
    }

    /* double cap and reallocate space in old hash table */
    *cap = (*cap) * 2;
    hash_table = realloc(hash_table, sizeof(struct word) * (*cap));

    /* copy over elements back into old hash table and free new hash table */
    for (i = 0; i < *cap; i++){
        hash_table[i] = hash_copy[i];
    }
    free(hash_copy);

    return;
}

unsigned long hash(char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *(str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
