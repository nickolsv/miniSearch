#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../header/myindex.h"
#include "../header/documentmap.h"
#include "../header/trie.h"
#include "../header/postinglist.h"

int fileToIndex(myIndex *searchIndex, FILE *fp)
{
  // Reads open file fp and saves it to the index

  int lines;
  int *wordcount;

  searchIndex->docMap->mapArray = fileToMap(fp,&lines,&wordcount);              // Saves file to map
  searchIndex->docMap->wordCount = wordcount;
  searchIndex->docMap->docCount = lines;

  if( searchIndex->docMap->mapArray == NULL )                                   return 1;

  int sum = 0;
  for(int i = 0 ; i < lines; i++)
    sum+= searchIndex->docMap->wordCount[i];


  searchIndex->docMap->avgWrds = ( (float) (sum) )/((float) (searchIndex->docMap->docCount));
  if( mapToTrie(searchIndex) == 1 )                                             // Try to insert every document to the trie
  {
    indexDestroy(searchIndex);
    return 1;
  }

  return 0;
}


int mapToTrie(myIndex *searchIndex)
{
  // Inserts every document saved in the map to the trie

  documentMap *docMap = searchIndex->docMap;
  for( int i = 0; i < docMap->docCount ; i++ )
    if( documentToTrie(searchIndex->invIndex,i,docMap->mapArray[i]) != 0 )      return 1;

  return 0;
}


int readAndCheckLine(FILE *fp, char **line, int docID, int *wc)
{
  // Reads a line from file fp and checks if it is valid
  // Also counts its words

  int i=1,count=0;
  char *temp = NULL;
  size_t n = 0;
  ssize_t nr;

  if( (nr = getline(&temp, &n, fp)) == -1 )                                     // If getline doesn't read a line
  {                                                                             // String is freed & returns error
    free(temp);
    return 1;
  }

  if( filterDocument(&temp, docID) != 0 )                                       // If getline string doesn't pass filterDocument's tests
  {                                                                             // String is freed & returns error
    free(temp);
    return 1;
  }

  // Word Counting


  if( temp[0] != ' ' && temp[0] != '\t' && (temp[1] == ' ' || temp[1] == '\t' || temp[1] == 0 ) )
    count++;                                                                    // Checks whether the string starts with a one letter word


  while(temp[i] != 0 )                                                          // Finds how many times a whitespace character is followed by a non-whitespace character
  {
    if( ( temp[i-1] == ' ' || temp[i-1] == '\t') && ( temp[i] != ' ' && temp[i] != '\t') )
      count++;
    i++;
  }
  count++;                                                                      // Adds 1 for the last word in the document

  *wc = count;
  *line = temp;
  return 0;
}


int filterDocument(char **doc,int id)
{
  // Creates a new string containing document passed as argument,
  // without the ID number, as well as any excess whitespace before and after the ID
  // and the newline character at the end of the document
  // Also verifies that the document ID is equal to the desired value of id

  int i=0,j=0,docID;
  char *temp, *line = *doc;

  if( line[0] == 0 )                            return 2;                       // Empty string

  while( line[i] == ' ' || line[i] == '\t' )      i++;
  docID = atoi(line);                                                           // ID verification
  if( docID != id )                             return 3;
  else if( docID == 0 )                                                         // atoi returns 0 on invalid number
  {                                                                             // so, check for false positive on 0
    int k=0;
    while(line[i+k] == '0') k++;
    if(line[i+k] != ' ' && line[i+k] != '\t')   return 4;                       // Non whitespace character right after an ID that's
  }                                                                             // supposedly equal to 0 means something's wrong
                                                                                // (Empty line or invalid ID )

  while(line[i] <= '9' && line[i] >= '0' ) i++;                                 // Since ID is OK, skip it

  if( line[i] != ' ' && line[i] != '\t' )       return 4;

  while( line[i] == ' ' || line[i] == '\t' )      i++;                          // Reach the start of the actual document
  while( line[i+j] != '\n' && line[i+j] != 0 )    j++;                          // Find the terminating newline

  if( j == 0 )                                  return 2;                       // Empty document

  line[i+j] = 0;                                                                // Remove the terminating newline

  temp = malloc((j+1)*sizeof(char));                                            // Create a new string with length equal to the new, truncated string

  strncpy(temp,(line + i*sizeof(char)),j+1);                                    // Copy the filtered document to the new string

  free(line);

  *doc = temp;

  return 0;
}


int getOrPrintDocFrequency(myIndex searchIndex, char *word)
{
  // Returns document frequency of term 'word' in index searchIndex
  // If word is equal to NULL, print the document frequency of every word
  // in the trie

  if( word == NULL )                                                            // If no term given as argument
  {
    if(searchIndex.invIndex != NULL && searchIndex.invIndex->top != NULL)
      dfRecursive(searchIndex.invIndex->top,NULL);                              // Print docFrequency for every word in the index and return -1
  }
  else
  {
    postingList *pl = getPostingList(searchIndex.invIndex,word);                // Return the document frequency from the head of the
    int freq = 0;                                                               // word's posting list ( if it has one ), else return 0
    if( pl != NULL )      freq = pl->docFrequency;
    return freq;
  }
  return -1;
}


int getTermFrequency(myIndex searchIndex, int docID, char *word)
{
  // Returns the term frequency of term 'word' in document with id
  // equal to docID in index searchIndex
  // If word does not exist ( has no posting list ) , tf is 0

  if( word != NULL)
  {
    postingList *pl = getPostingList(searchIndex.invIndex,word);
    int freq = 0;
    if( pl != NULL )      freq = getPostingCount(pl, docID);
    return freq;
  }
  return 0;
}

int getDocCount(myIndex searchIndex)
{
  // Returns number of documents in index

  if( searchIndex.docMap == NULL ) return -1;
  return searchIndex.docMap->docCount;
}


int indexInit(myIndex *searchIndex)
{
  // Allocates memory for and initializes the index and its data structures

  if( ( searchIndex->invIndex = malloc(sizeof(trie)) ) == NULL )  return 1;     // Create inverted Index

  if( ( searchIndex->docMap = malloc(sizeof(documentMap)) ) == NULL )           // Create document map
  {
    free(searchIndex->invIndex);
    return 1;
  }

  if( trieInit(searchIndex->invIndex) != 0 )                                    // Trie initialization
  {
    free(searchIndex->invIndex);
    free(searchIndex->docMap);
    return 1;
  }

  mapInit(searchIndex->docMap);                                                 // Map initialization

  return 0;
}


void indexDestroy(myIndex *searchIndex)
{
  // Destroys the index and its data structures

  if( searchIndex->invIndex != NULL )
  {
    trieDestroy(searchIndex->invIndex);
    free(searchIndex->invIndex);
  }

  if( searchIndex->docMap != NULL )
  {
    mapDestroy(searchIndex->docMap);
    free(searchIndex->docMap);
  }
}
