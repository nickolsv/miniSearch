#ifndef __MY_INDEX_H__
#define __MY_INDEX_H__

#include <stdio.h>

#define K1    1.2
#define BETA  0.75

typedef struct myIndex {
  // Data structure containing a trie
  // as well as a document map

  struct trie *invIndex;
  struct documentMap *docMap;
} myIndex;

int fileToIndex(myIndex *,FILE *);
int mapToTrie(myIndex *);

int readAndCheckLine(FILE *, char **, int, int *);
int filterDocument(char **, int);

int getOrPrintDocFrequency(myIndex,char *);
int getTermFrequency(myIndex,int,char *);

int getDocCount(myIndex);

int indexInit(myIndex *);
void indexDestroy(myIndex *);


#endif /* end of include guard: __MY_INDEX_H__ */
