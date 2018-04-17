#ifndef FW_H
#define FW_H

struct word;

void add_words(FILE *infile, void *hash, int *cap);
int word_comp(struct word *word1, struct word *word2);
void rehash(void *hash, int *cap);
unsigned long hash(unsigned char *str);


#endif
