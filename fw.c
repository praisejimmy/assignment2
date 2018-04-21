#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "fw.h"

int num_words;

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
    num_words = 0;

    /* initialize each entry to be an empty struct */
    for(i = 0; i < 1000; i++){
        hash[i].val = malloc(0);
        hash[i].freq = 0;
    }

    if (argc >= 3 && !strcmp(argv[1], "-n")) {
        int i = 0;
        while (argv[2][i] != '\0') {
            if (!isdigit(argv[2][i])) {
                fprintf(stderr, "usage: fw[-n num] [file 1 [file 2 ...]]\n");
                exit(1);
            }
            i++;
        }
        num_results = atoi(argv[2]);
    }

    /* standard input  with -n handle */
    if (argc == 3 && !strcmp(argv[1], "-n")) {
        hash = add_words(stdin, hash, cap_ptr);
    }

    /* file inputs */
    else if (argc > 1) {
        int i = 0;
        if (!strcmp(argv[1], "-n"))
            i = 3;
        else
            i = 1;
        for (; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if(file == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", argv[i]);
                exit(1);
            }
            else {
                hash = add_words(file, hash, cap_ptr);
            }
            fclose(file);
        }
    }

    /* standard input */
    else {
        hash = add_words(stdin, hash, cap_ptr);
    }

    /* sort the hash table */
    qsort(hash, cap, sizeof(struct word), word_comp);

    printf("The top %d words (out of %d) are:\n", num_results, num_words);
    for(i = 0; i < num_results; i++){
        if(hash[i].freq != 0){
            printf("%9d %s\n", hash[i].freq, hash[i].val);
        }
    }
    /* free everything? */
    for(i = 0; i < *cap_ptr; i++){
        free(hash[i].val);
    }
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
    else if (freq1 == 0){
        return 1;
    }
    else if (freq2 == 0){
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

struct word *add_words(FILE *infile, struct word *hash_table, int *cap) {
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
            in_word[size - 1] = tolower(c);
        }
        else if(size > 0){
            unsigned int loc;
            size++;
            if (size > word_cap) {
                word_cap += 1;
                in_word = realloc(in_word, sizeof(char) * word_cap);
            }
            in_word[size - 1] = '\0';
            loc = hash(in_word) % *cap;
            if (hash_table[loc].freq == 0) {
                hash_table[loc].val = realloc(hash_table[loc].val, size*sizeof(char));
                strcpy(hash_table[loc].val, in_word);
                hash_table[loc].freq = 1;
                num_words++;
                /*printf("\n%s was null with hash of : %d\n", hash_table[loc].val, loc);*/
            }
            else if (in_word != NULL && !strcmp(in_word, hash_table[loc].val)){
                hash_table[loc].freq++;
                /*printf("\n%swas equal with val of : %s\n", hash_table[loc].val, hash_table[loc].val);*/
            }
            else {
                int i = 1;
                while(hash_table[loc].freq != 0 && strcmp(in_word, hash_table[loc].val)){
                    loc = (loc + i*i) % *cap;
                    i++;
                }
                if (hash_table[loc].freq == 0) {
                    hash_table[loc].val = realloc(hash_table[loc].val, (size + 1)*sizeof(char));
                    strcpy(hash_table[loc].val, in_word);
                    hash_table[loc].freq = 1;
                    num_words++;
                }
                else {
                    hash_table[loc].freq++;
                }
                /*do {
                    rehash(hash_table, cap);
                    int loc = hash(new_word.val);
                } while (hash_table[loc].val != NULL);*/
                /*printf("\n%swas equal with hash of : %d\n",hash_table[loc].val, loc);*/
            }
            size = 0;
            word_cap = 0;

            /* check if needs to be rehashed */
            if((num_words*1.0 / *cap) > 0.8){
                hash_table = rehash(hash_table, cap);
            }
        }

        c = fgetc(infile);
    }
    free(in_word);
    return hash_table;
}

struct word *rehash(struct word *hash_table, int *cap) {
    int i;
    struct word *hash_copy;
    *cap = 2 * (*cap);
    /* Create new hash table */
    hash_copy = (struct word*)malloc(sizeof(struct word) * *cap);

    /* finish initializing rest of new table */
    for (i = 0; i < *cap; i++){
        hash_copy[i].val = malloc(0);
        hash_copy[i].freq = 0;
    }
    /* Move over old elements into new spots in the new hash table */
    for (i = 0; i < (*cap)/2; i++){
        if(hash_table[i].freq != 0){
            unsigned int loc = hash(hash_table[i].val) % *cap;
            if(hash_copy[loc].freq != 0){
                int j = 1;
                while(hash_copy[loc].freq != 0){
                    loc = (loc + j*j) % *cap;
                    j++;
                }
            }
            hash_copy[loc].val = realloc(hash_copy[loc].val, sizeof(char) * (1 + strlen(hash_table[i].val)));
            strcpy(hash_copy[loc].val, hash_table[i].val);
            hash_copy[loc].freq = hash_table[i].freq;
        }
    }

    for(i = 0; i < (*cap)/2; i++){
        free(hash_table[i].val);
    }
    free(hash_table);

    return hash_copy;
}

unsigned long hash(char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *(str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
