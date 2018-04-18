#ifndef FW_H
#define FW_H

struct word;

void add_words(FILE *infile, struct word *hash, int *cap);
int word_comp(const void *word1, const void *word2);
void rehash(struct word *hash, int *cap);
unsigned long hash(char *str);


#endif
