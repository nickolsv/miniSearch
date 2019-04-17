#include <stdlib.h>

#include "../header/trie.h"
#include "../header/postinglist.h"


int incrementPostingList(postingList *pl, int docID)
{
  // Searches posting list for node with docID equal to the given ID
  // and increments the word occurence count of that node
  // If node is not found, it is inserted
  // Posting list is sorted by docID in descending order}

  postingListNode *curr = pl->start;
  postingListNode *prev, *temp;

  if( curr == NULL )                                                            // If list is empty
  {
    curr = malloc(sizeof(postingListNode));                                     // Try to create new listnode
    if( curr == NULL )                                                 return 1;

    curr->docID = docID;
    curr->count = 1;
    curr->next  = pl->start;

    pl->start = curr;                                                           // Assign new node to start of list
    pl->docFrequency++;

    return 0;
  }


  if( curr->docID == docID )                                                    // If list head is the node with docID
  {                                                                             // increment this node's term count
    curr->count++;

    return 0;
  }
  else if( curr->docID < docID )                                                // If node to be inserted's docID > the head's docID
  {                                                                             // New head is created
    temp = malloc(sizeof(postingListNode));
    if( temp == NULL )                                                 return 1;

    temp->docID = docID;
    temp->count = 1;
    temp->next  = pl->start;

    pl->start = temp;
    pl->docFrequency++;

    return 0;
  }

  // DocIDs are inserted in ascending order, therefore every docID will always be greater than
  // or equal than those in the posting list. Since files whose docIDs are not in ascending
  // order have been rejected by now, if control reaches this point, something has probably gone wrong

  prev = curr;
  curr = curr->next;

  while( curr != NULL )                                                         // Attempts to iterate through the entire list
  {
    if( curr->docID == docID )                                                  // If node with docID exists, increment it's count
    {
      curr->count++;

      return 0;
    }
    else if( curr->docID < docID )                                              // If docID finds it's place, a node is created
    {
      temp = malloc(sizeof(postingListNode));
      if( temp == NULL )                                               return 1;

      temp->docID = docID;
      temp->count = 1;
      temp->next = curr;

      prev->next = temp;
      pl->docFrequency++;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }

  temp = malloc(sizeof(postingListNode));                                       // If end of the list is reached, a node is created at the end
  if( temp == NULL )                                                   return 1;

  temp->docID = docID;
  temp->count = 1;
  temp->next = NULL;

  prev->next = temp;
  pl->docFrequency++;

  return 0;
}


int getPostingCount(postingList *pl, int docID)
{
  // Returns the 'count' field of a word's posting list
  // entry for docID ( i.e. the word's term frequency for
  // document with id = docID )

  postingListNode *node = pl->start;
  while( node != NULL )
  {
    if( node->docID == docID )                               return node->count;
    else if( node->docID < docID )                                        break;// No posting list entry with docID ,since postinglist is sorted
    node = node->next;
  }

  return 0;
}


postingList *getPostingList(trie *tr, char *word)
{
  // Returns reference to the head node of the posting list
  // that correspond to term 'word'

  trieNode *curr = tr->top;
  characterNode *cnode;
  int i = 0;


  while( word[i] != 0 )                                                         // Search the trie for the word
  {
    if(curr == NULL)                                                return NULL;
    
    cnode = curr->start;

    while( cnode != NULL && cnode->key < word[i])
      cnode = cnode->next;

    if( cnode == NULL || cnode->key > word[i] )                     return NULL;

    curr = cnode->down;
    i++;
  }

  return cnode->pl;                                                             // And return the postinglist in word's last character's characternode
}


void postingListDestroy(postingList *pl)
{
  // Destroys posting list pl, freeing all nodes

  postingListNode *pnode,*next;

  pnode = pl->start;

  while( pnode != NULL)                                                         // Iterate through the list and free every node
  {
    next = pnode->next;
    free(pnode);
    pnode = next;
  }
}
