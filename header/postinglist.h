#ifndef __POSTING_LIST_H__
#define __POSTING_LIST_H__

#include "../header/trie.h"

struct postingListNode;

typedef struct postingList {
  // Head node of posting list

  struct postingListNode *start;
  int docFrequency;
} postingList;

typedef struct postingListNode {
   // Posting list nodes that contain entries

  struct postingListNode *next;
  int docID;
  int count;
} postingListNode;

int incrementPostingList(postingList *, int);
int getPostingCount(postingList *, int);
postingList *getPostingList(trie *, char *);
void postingListDestroy(postingList *);


#endif /* end of include guard: __POSTING_LIST_H__ */
