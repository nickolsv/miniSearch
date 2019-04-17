#ifndef __TERM_SEARCH_H__
#define __TERM_SEARCH_H__

// Functions and data structures for minisearch's searching and printing

#include "../header/myindex.h"
#include "../header/postinglist.h"


typedef struct docListNode {
  // Node of a list containing docIDs

  int docID;
  struct docListNode *next;
} docListNode;

typedef struct docList {
  // Head of a list containing docIDs
  // Used for finding all relevant documents in a search

  docListNode *start;
} docList;


typedef struct underlineListNode {
  // Node of underlineList

  int charcount;
  int tabcount;
  struct underlineListNode *next;
} underlineListNode;

typedef struct underlineList {
  // Head of a list used to determine the placement
  // of spaces and '^'s when underlining a line of text
  
  int togglePos;
  underlineListNode *start;
} underlineList;


void printSearch(myIndex, char **, int, int);

float calculateScore(myIndex, int, int, char **);
float calculateIDF(myIndex, char *);

underlineList getDocumentUnderlineList(char *, int, char **);
void underlineListDestroy(underlineList);

int lookUpWord(char *, int *, char **, int);

docList getDocIDs(myIndex, char **, int);
void postingListToDocList(docList *, postingList *);
void docListDestroy(docList);

void printDocument(char *, underlineList, int);
void printHeader(int, float, int, int);
int printLine(char *, int, int, int);
underlineListNode *printUnderline(underlineListNode *, int *, int, int);

#endif /* end of include guard: __TERM_SEARCH_H__ */
