#ifndef FW_H
#define FW_H

void add_words(FILE infile, void *hash);
int word_comp(struct word word1, struct word word2);
void rehash(void *hash, int *cap);



#endif