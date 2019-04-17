#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../header/trie.h"
#include "../header/postinglist.h"

int documentToTrie(trie *tr, int docID, char* line)
{
  // Adds every word of the string line to the trie

  char *temp,*tok;
  int len = strlen(line);

  temp = malloc((len+1)*sizeof(char));                                          // Copies document to a temporary string
  if(temp == NULL)  return 1;

  strncpy(temp,line,len+1);

  tok = strtok(temp, " \t");                                                    // Strtok splits it into words
  while( tok != NULL )
  {
    if( trieAddWord(tr,tok,docID) != 0 )                                        // Each word is added to the trie
    {
      free(temp);
      return 1;
    }
    tok = strtok(NULL, " \t");
  }

  free(temp);
  return 0;
}


int trieAddWord(trie *tr, char *word, int docID)
{
  // Adds a word to trie tr, updating its posting list if it exists
  // and inserting it to the trie otherwise

  int i = 0;
  trieNode *temp = tr->top;
  characterNode *curr;
  postingList *pl;

  while( word[i] != 0 )                                                         // For each letter of the word
  {

    curr = getNodeMaybeInsert(temp,word[i]);                                    // Find characternode in list temp whose key equals the current letter
                                                                                // If it doesn't exist, it is inserted and then returned
    if( curr == NULL )                                return 1;
    temp = curr->down;                                                          // Descends one level

    if( temp == NULL )                                                          // If lower level list head doesn't exist it is created
    {
      temp = malloc(sizeof(trieNode));
      if(temp == NULL)                                return 1;
      temp->start = NULL;
      curr->down = temp;
    }
    i++;
  }


  if(curr->pl == NULL)                                                          // If word has no postinglist, create it
  {
    pl = malloc(sizeof(postingList));
    pl->start = NULL;
    pl->docFrequency = 0;
    curr->pl = pl;
  }

  if( incrementPostingList(curr->pl,docID) != 0 )     return 1;                 // Add 1 to postinglist entry of docID ( if it doesn't exist, it is created )
  return 0;
}


characterNode *getNodeMaybeInsert(trieNode *node, char letter)
{
  // Returns the characterNode of the trieNode that contains letter
  // If it doesn't exist, it is created,inserted and returned

  characterNode *curr,*temp,*prev;
  curr = node->start;

  if( curr == NULL || curr->key > letter )                                      // If start of characterlist is empty,  or charnode with letter
  {                                                                             // needs to be inserted at the start of the list for it to be sorted
    temp = malloc(sizeof(characterNode));

    if(temp == NULL)                   return NULL;

    temp->key = letter;
    temp->down = NULL;
    temp->next = curr;
    temp->pl = NULL;

    node->start = temp;
    return node->start;
  }

  while( curr != NULL && curr->key <= letter )                                  // Traverse through the entire list or until a key equal or greater than letter is found
  {
    if( curr->key == letter)                            return curr;            // If current characternode's key is equal to letter, node has been found and is returned

    prev = curr;
    curr = curr->next;
  }

  temp = malloc(sizeof(characterNode));                                         // Characternode with letter's correct position has been found, so it is inserted there

  if(temp == NULL)                   return NULL;

  temp->key = letter;
  temp->down = NULL;
  temp->next = curr;
  temp->pl = NULL;

  prev->next = temp;
  return temp;
}


void dfRecursive(trieNode *tnode, charStackEntry *cst)
{
  // Recursively accesses every character node under tnode
  // and prints the words each path represents with the help
  // of cst

  characterNode *cnode = tnode->start;
  if( cnode == NULL )             return;

  charStackEntry * newcst = malloc(sizeof(charStackEntry));

  if(newcst == NULL )                                                           // malloc failure
  {
    return;
  }

  newcst->next = cst;

  while( cnode != NULL )                                                        // For every character node in the list, 'push' it to the stack,
  {                                                                             // execute an iteration of the loop and 'pop' it
    newcst->key = cnode->key;
    if( cnode->pl != NULL)                                                      // If a node has a posting list, then it is the final
    {                                                                           // of a word
      printStack(newcst);                                                       // Prints the stack, printing the word that ends on cnode
      printf(" %d\n",cnode->pl->docFrequency);                                  // Also prints the word's docFrequency
    }
    if( cnode->down != NULL)                                                    // If characterNode has a trieNode list under it, call dfRecursive
    {                                                                           // for that node
      dfRecursive(cnode->down,newcst);
    }
    cnode = cnode->next;
  }
  free(newcst);
}


void printStack(charStackEntry *cst)
{
  // Prints stack cst, ( used by dfRecursive to print
  // every word in trie )

  if(cst == NULL)   return;
  printStack(cst->next);
  printf("%c",cst->key);
}


void trieNodeDestroy(trieNode *tnode)
{
  // Recursively destroy everything in the trie below tnode

  characterNode *cnode, *next;

  cnode = tnode->start;
  while( cnode != NULL )                                                        // Destroy every characterNode after trieNode
  {
    next = cnode->next;

    if( cnode->pl != NULL )                                                     // If characterNode has a posting List destroy it
    {
      postingListDestroy(cnode->pl);
      free(cnode->pl);
    }

    if( cnode->down != NULL )                                                   // If characterNode has a trieNode under it,
    {                                                                           // call trieNodeDestroy it
      trieNodeDestroy(cnode->down);
      free(cnode->down);
    }
    free(cnode);
    cnode = next;
  }
}


int trieInit(trie *tr)
{
  // initializes the trie, so that words can be inserted into it

  tr->top = malloc(sizeof(trieNode));
  if( tr->top == NULL )                return 1;
  tr->top->start = NULL;
  return 0;
}


void trieDestroy(trie *tr)
{
  // Destroys trie tr and everything it contains with the help
  // of trieNodeDestroy

  if( tr->top != NULL )
  {
    trieNodeDestroy(tr->top);
    free(tr->top);
  }
}
