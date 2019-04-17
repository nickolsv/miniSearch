#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../header/termsearch.h"
#include "../header/postinglist.h"
#include "../header/msmisc.h"
#include "../header/myindex.h"
#include "../header/documentmap.h"

void printSearch(myIndex searchIndex, char **terms, int termcount, int maxResNumber)
{
  // Given a searchIndex and termcount terms, prints the top maxResNumber ( 1<maxResNumber<11 )
  // documents in terms of relevance to the terms. The documents have been formatted and
  // the searchterms have been underlined

  int i,docIDCount;
  float score;

  docAndScore *dsArray;
  underlineList whitesp;

  docListNode *curr;
  docList dl;


  dl = getDocIDs(searchIndex,terms,termcount);                                  // Get relevant DocIDs for each term

  if( dl.start == NULL )  return;                                               // If no terms have been found in any documents, nothing to show, exit

  docIDCount = 0;                                                               // Calculate number of relevant documents
  curr = dl.start;

  while( curr != NULL)
  {
    docIDCount++;
    curr = curr->next;
  }

  dsArray = malloc(docIDCount*sizeof(docAndScore));                             // Transform the document list into an array

  i = 0;
  curr = dl.start;
  while( curr != NULL )                                                         // Calculate each document's scoure and add it to an array along with the docIDs
  {
    score = calculateScore(searchIndex,curr->docID,termcount,terms);
    dsArray[i].docID = curr->docID;
    dsArray[i].score = score;

    curr = curr->next;
    i++;
  }

  quickSort(&dsArray,0,docIDCount-1);                                           // Sort dsArray by score

  int offset = getDigits(searchIndex.docMap->docCount) + 6 + 6;                 // Calculate the offset - whitespace before each line of text

  for(i=0; i<maxResNumber && i<docIDCount; i++)                                 // For each relevant document in descending order (by score)
  {
    int exoffs = getDigits(i) - 1;                                              // Extra whitespace for large scores and resultIDs

    whitesp = getDocumentUnderlineList(searchIndex.docMap->mapArray[dsArray[i].docID],termcount,terms);         // Calculate the positions of the words to be underlined
    printHeader(i,dsArray[i].score,searchIndex.docMap->docCount,dsArray[i].docID);                              // Print the header with the document info

    exoffs+= getDigits((int) score) - 1;
    printDocument(searchIndex.docMap->mapArray[dsArray[i].docID],whitesp,offset+exoffs);                        // Print the formatted document with all searchterms underlined
    underlineListDestroy(whitesp);                                                                              // Destroy the whitespace list
  }

  docListDestroy(dl);                                                           // Free the docID list and array
  free(dsArray);
}


void printDocument(char * document, underlineList whitesp, int offset)
{
  // Prints document line by line, underlining all searchterms

  int term,len,usableSpace,toggle = whitesp.togglePos;
  int sres = 0,res;
  struct winsize w;
  underlineListNode *curr = whitesp.start;
  int cols = 140;                                                               // A reasonable default value for columns
                                                                                // in case ioctl fails
  if( ( term = ioctl(STDOUT_FILENO,TIOCGWINSZ,&w) == 0 ) )
    cols = w.ws_col;

  usableSpace = cols - offset - 1;

  len = strlen(document);

  res = printLine(document, 0, usableSpace, len);                               // Prints first line
  sres = res;

  curr = printUnderline(curr, &toggle, res, offset+1);                          // And first underline

  while( curr != NULL )
  {
    for(int i=0; i< offset; i++ )           printf(" ");                        // Prints padding (under header)

    res = printLine(document, sres, usableSpace, len);

    if( res <= 0 )  break;

    curr = printUnderline(curr, &toggle, res, offset);
    sres+=res;
  }
}


void printHeader(int rating,float score,int docCount, int docID)
{
  // Prints header containing docID and document score

  int dig1 = getDigits(docCount),dig2 = getDigits(docID);
  if(docID == 0)  dig2 = 1;

  printf("%d.(",rating);
  for(int i=0;i<dig1-dig2;i++)  putchar(' ');
  printf("%d)[",docID);
  printf("%.4f] ",score);
}

int printLine(char *document, int offset, int usableSpace, int docLen)
{
  // Prints a line of document , with a length of usableSpace characters or less
  // starting at the offset'th character of document.
  // Returns number of characters in line

  int i;
  if( offset > docLen )         return -1;

  char *currDocPos = document + offset*sizeof(char);

  char *line = malloc((usableSpace+1)*sizeof(char));
  strncpy(line,currDocPos,usableSpace+1);

  for(i = usableSpace; i > 0; i--)
  {
    if(line[i] != ' ' && line[i] != '\t')   line[i] = 0;                        // If only part of a word is in line, do not print it
    else                                                                        // the next line will
    {
      line[i] = 0;
      int j=0;
      while(line[i-j] == ' ' || line[i-j] == '\t')                              // Do not print extra whiespace
      {
        line[i-j] = 0;
        j++;
      }
      break;
    }

  }

  printf("%s\n",line);
  free(line);
  return i;
}

underlineListNode *printUnderline(underlineListNode *whitesp, int *toggle, int printed, int offset)
{
  // Prints '^' characters under the relevant search terms of the previous line printed with the help
  // of an underlineList. Does nothing if line contains no relevant search terms

  int tgl = *toggle;
  underlineListNode *curr = whitesp;

  if( curr == NULL || printed == -1 )         return NULL;                      // Entire document has been printed

  if(curr->charcount > printed)                                                 // If line printed contains no search terms
  {
    if(tgl == 1)  for(int i=0; i< printed; i++)   putchar('^');                 // Except if the entire line is a searchterm (edge case)
    curr->charcount-=printed;
    return curr;
  }

  for(int i=0; i< offset; i++ ) putchar(' ');                                   // Prints the starting offset

  while( printed > 0)                                                           // Only print as many characters as the length of the document line
  {
    if(tgl == 0)    putchar(' ');                                               // Print either spaces or '^'s, depending on the value of the toggle
    else            putchar('^');
    curr->charcount--;
    printed--;
    if( curr->charcount == 0 )                                                  // Node done, flip toggle and proceed to the next node
    {
      if(tgl == 0)    tgl = 1;
      else            tgl = 0;
      curr = curr->next;
      if( curr == NULL )     return NULL;                                       // Document over
    }
  }

  printf("\n");

  *toggle = tgl;
  return curr;
}


float calculateScore(myIndex searchIndex, int docID, int termCount, char **terms)
{
  // Calculates relevance score for a document, given searchterms 'terms'

  float score = 0;
  float wordNum = searchIndex.docMap->wordCount[docID];
  float avg = searchIndex.docMap->avgWrds;

  for( int i = 0; i < termCount; i++ )
  {
    float termFreq = (float) (getTermFrequency(searchIndex,docID,terms[i]));
    float IDF      = calculateIDF(searchIndex,terms[i]);
    score += IDF*( termFreq*(K1+1) )/( termFreq + K1*( 1 - BETA + BETA*( wordNum/avg )));
  }
  return score;
}

float calculateIDF(myIndex searchIndex, char * term)
{
  // Calculate the IDF of a term

  int count = 0;
  if( term != NULL )  count = getOrPrintDocFrequency(searchIndex,term);

  float temp = ( (float) (searchIndex.docMap->docCount) - (float) (count) + 0.5)/ ( (float) (count) + 0.5);
  return (float) (log10(temp));
}


underlineList getDocumentUnderlineList(char *document, int termcount, char ** terms)
{
  // Returns a list that encodes which characters of a document to underline

  int i=0,len;
  underlineList ulist;
  ulist.togglePos = 0;

  underlineListNode *ul = malloc(sizeof(underlineListNode)), *temp;

  ul->charcount = 0;
  ul->tabcount  = 0;
  ul->next = NULL;

  ulist.start = ul;

  if(document[0] != ' ' && lookUpWord( document, &len, terms, termcount) == 0 ) // If document starts with a relevant term, then the underline must
  {                                                                             // start with '^' instead of spaces, therefore the starting toggle position
    ul->charcount = len;                                                        // is flipped
    ulist.togglePos = 1;

    temp = malloc(sizeof(underlineListNode));                                   // Creates first whitespace node
    temp->charcount = 0;
    temp->tabcount = 0;
    temp->next = NULL;

    ul->next = temp;
    ul = ul->next;
    i+=len;
  }

  while( document[i] != 0 )
  {
    if( document[i] == ' ' )      ul->charcount++;
    else if( document[i] == '\t' ) ul->tabcount++;
    else                                                                        // If a word is encountered, it is compared with each term and if it matches any,
    {                                                                           // an underlining node with charcount equal to the word's length is created.
      if( lookUpWord( document + i*sizeof(char), &len, terms,termcount) == 0 )  // Then a whitespace node is inserted and the loop continues
      {
        temp = malloc(sizeof(underlineListNode));
        temp->charcount = len;
        temp->tabcount = 0;
        temp->next = NULL;

        ul->next = temp;
        ul = ul->next;

        temp = malloc(sizeof(underlineListNode));
        temp->charcount = 1-len;
        temp->tabcount = 0;
        temp->next = NULL;

        ul->next = temp;
        ul = ul->next;

      }
      else
      {
        ul->charcount+=len;
        i+=len-1;
      }
    }
    i++;
  }
  return ulist;
}


void underlineListDestroy(underlineList whitesp)
{
  // Destroy an underline list

  underlineListNode *unode = whitesp.start, *next;

  while(unode != NULL)
  {
    next = unode->next;
    free(unode);
    unode = next;
  }
}


int lookUpWord(char *str, int *len, char ** terms,int termcount)
{
  // Checks if string starting at tmp and ending at the first
  // whitespace character is equal to any of the searchterms

  int i = 0;
  char *tmp;

  while(str[i] != ' ' && str[i] != '\t' && str[i] != 0) i++;                    // Count characters to the first whitespace char (or the end of the string)
                                                                                // to find the end of the term
  tmp = malloc((i+1)*sizeof(char));
  strncpy(tmp,str,i+1);                                                         // Copy the word to a new string
  tmp[i] = 0;

  *len = i;

  for(int k=0; k < termcount ; k++)                                             // Compare the word to every term
  {                                                                             // return 0 if at least one matches, else return 1
    if(strcmp(tmp,terms[k]) == 0)
    {
      free(tmp);
      return 0;
    }
  }
  free(tmp);
  return 1;
}

docList getDocIDs(myIndex searchIndex, char **terms, int termcount)
{
  // Returns a sorted list containing the document IDs of the documents
  // in which any of the searchterms appear

  postingList *pl = NULL;
  docList dl;
  dl.start = NULL;

  for(int i = 0; i < termcount; i++)
  {
    pl = getPostingList(searchIndex.invIndex,terms[i]);
    if( pl != NULL )      postingListToDocList(&dl,pl);
  }
  return dl;
}

void postingListToDocList(docList *dl, postingList *pl)
{
  // Merges a postingList into a docList, removing duplicates
  // and making sure the resulting list is still sorted in descending order

  docListNode *dnode = dl->start, *prev = NULL, *temp;
  postingListNode *pnode = pl->start;

  if( pnode == NULL )         return;

  if( dnode == NULL )                                                           // If list is empty
  {
    temp = malloc(sizeof(docListNode));
    temp->next = NULL;
    temp->docID = pnode->docID;

    dl->start = temp;
    dnode = temp;

    pnode = pnode->next;

    while( pnode != NULL )
    {
      temp = malloc(sizeof(docListNode));
      temp->next = NULL;
      temp->docID = pnode->docID;

      dnode->next = temp;
      dnode = dnode->next;

      pnode = pnode->next;
    }
    return;
  }
  else if( dnode->docID < pnode->docID)
  {
    temp = malloc(sizeof(docListNode));
    temp->next = dnode;
    temp->docID = pnode->docID;

    dl->start = temp;
    prev = temp;

    pnode = pnode->next;
  }
  else if( dnode->docID == pnode->docID)
  {
    prev = dnode;
    dnode = dnode->next;
    pnode = pnode->next;
  }
  else
  {
    prev = dnode;
    dnode = dnode->next;
  }

  while( dnode != NULL && pnode != NULL )
  {
    if( dnode->docID < pnode->docID )
    {
      temp = malloc(sizeof(docListNode));
      temp->next = dnode;
      temp->docID = pnode->docID;
      prev->next = temp;

      pnode = pnode->next;
      prev = temp;
    }
    else if( dnode->docID == pnode->docID )
    {
      prev = dnode;
      dnode = dnode->next;

      pnode = pnode->next;
    }
    else
    {
      prev = dnode;
      dnode = dnode->next;
    }
  }


  while( pnode != NULL)
  {
    temp = malloc(sizeof(docListNode));
    temp->next = NULL;
    temp->docID = pnode->docID;
    prev->next = temp;

    prev = temp;
    pnode = pnode->next;
  }
}

void docListDestroy(docList dl)
{
  // Destroy a doc list

  docListNode *dnode = dl.start, *next;

  while(dnode != NULL)
  {
    next = dnode->next;
    free(dnode);
    dnode = next;
  }
}
