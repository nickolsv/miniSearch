#ifndef __DOCUMENT_MAP_H__
#define __DOCUMENT_MAP_H__

#include <stdio.h>

typedef struct documentMap{
  // Contains an array that stores each document read from the input
  // file, each document's word count, as well as the average word count
  // of all documents

  char **mapArray;
  int *wordCount;
  float avgWrds;
  int docCount;
} documentMap;

char **fileToMap(FILE *, int *, int **);

void mapInit(documentMap *);
void mapDestroy(documentMap *);


#endif /* end of include guard: __DOCUMENT_MAP_H__ */
