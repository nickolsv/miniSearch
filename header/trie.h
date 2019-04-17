#ifndef __TRIE_H__
#define __TRIE_H__

struct trieNode;

typedef struct characterNode {
  // Nodes that contain characters, Posting Lists
  // and head nodes of characterNode lists of depth d+1

  char key;
  struct characterNode *next;
  struct trieNode *down;
  struct postingList *pl;
} characterNode;

typedef struct trieNode {
  //  Head nodes of characterNode lists

  characterNode *start;
} trieNode;

typedef struct trie {
  // 'Root' of the trie, contains the head of the
  // characterNode list for depth = 1

  trieNode *top;
} trie;



typedef struct charStackEntry {
  // Struct used to store the characters contained
  // in higher levels of the trie during its traversal,
  // so that entire words can be printed from the bottom of
  // of the trie

  char key;
  struct charStackEntry* next;
} charStackEntry;

int documentToTrie(trie *, int, char *);
int trieAddWord(trie *, char *, int);
int filterDocument(char **, int);

characterNode *getNodeMaybeInsert(trieNode *,char);

void dfRecursive(trieNode *,charStackEntry *);
void printStack(charStackEntry *);

int trieInit(trie *);
void trieDestroy(trie *);



#endif /* end of include guard: __TRIE_H__ */
