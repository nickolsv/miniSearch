#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../header/msmisc.h"
#include "../header/myindex.h"
#include "../header/termsearch.h"

#define BUFFER_SIZE 256

int main(int argc, char const *argv[])
{
  myIndex searchIndex;
  char *filename,*token;
  char *searchterms[10];
  char buffer[BUFFER_SIZE],tbuff[BUFFER_SIZE];
  int maxResNumber = 10,exitflag=0;
  FILE *fp;


  // Argument parsing

  int fileOK = 1;

  if(argc < 3 )
  {
    fprintf(stderr, "Not enough arguments. Exiting...\n");
    return 1;
  }

  for( int i = 1; i < argc-1; i++)
  {
    if( strcmp(argv[i],"-i") == 0 && fileOK == 1 )
    {
      i++;
      int len = strlen(argv[i]);
      filename = malloc((len+1)*sizeof(char));
      strncpy(filename,argv[i],(len+1)*sizeof(char));
      fileOK = 0;
    }
    else if(strcmp(argv[i],"-k") == 0)
    {
      i++;
      maxResNumber = atoi(argv[i]);
    }
  }

  if(fileOK == 1 || maxResNumber <= 0)
  {
    fprintf(stderr, "Argument Error. Exiting...\n" );
    if( fileOK == 0 )                 free(filename);
    return 2;
  }


  // File opening

  if ( (fp = fopen(filename,"r")) == NULL )
  {
    fprintf(stderr, "Error opening file. Exiting...\n");
    free(filename);
    return 3;
  }


  // Index initialization

  if( indexInit(&searchIndex) != 0 )
  {
    fprintf(stderr,"Could not initialize index. Exiting...\n");
    free(filename);
    return 4;
  }


  // Storing documents in index

  if( fileToIndex(&searchIndex,fp) != 0 )
  {
    fprintf(stderr, "Indexing error. Exiting...\n");
    free(filename);
    return 5;
  }

  fprintf(stderr,"Indexing done... Accepting commands: \n");


  int dc = getDocCount(searchIndex);                                            // Upper bound of results should be the number of documents in the index
  if(  dc != -1 && maxResNumber > dc )  maxResNumber = dc;

  // Shell Loop

  while ( fgets(buffer,BUFFER_SIZE,stdin) != NULL)
  {

    int c=0;
    while(buffer[c]!='\n' && buffer[c]!=0)  c++;                                // Replace \n with \0
    buffer[c] = 0;

    strncpy(tbuff,buffer,BUFFER_SIZE);

    if(tbuff[0] == 0 )                            continue;

    token = strtok(tbuff, " \t");

    if( strcmp(token,"/df") == 0 )                                              // Document Frequency
    {
      int frq;
      token = strtok(NULL," \t");

      frq = getOrPrintDocFrequency(searchIndex,token);
      if( frq != -1)  printf("%s %d\n",token,frq);
    }
    else if( strcmp(token,"/tf") == 0 )                                         // Term Frequency
    {
      int docID;

      token = strtok(NULL," \t");

      if(token == NULL)                           continue;
      docID = atoi(token);

      token = strtok(NULL," \t");
      if(token == NULL)                           continue;

      printf("%d %s %d\n",docID,token,getTermFrequency(searchIndex,docID,token));
    }
    else if( strcmp(token,"/search") == 0 )                                     // Search
    {
      int tcount = 0;

      token = strtok(NULL," \t");
      while( token != NULL && tcount < 10)
      {
        int len = strlen(token) + 1;
        searchterms[tcount] = malloc(len*sizeof(char));
        strncpy( searchterms[tcount], token , len );
        token = strtok(NULL," \t");
        tcount++;
      }

      if( tcount == 0 )   continue;
      printSearch(searchIndex, searchterms, tcount, maxResNumber);

      for(int a=0; a < tcount; a++) free(searchterms[a]);
    }
    else if( strcmp(token,"/exit") == 0 )                                       // Exit
    {
      exitflag = 1;
      break;
    }
    else    fprintf(stderr, "Unknown Command.\n");

  }


  fclose(fp);                                                                   // Free everything
  free(filename);
  indexDestroy(&searchIndex);

  if( exitflag == 0 )
  {
    fprintf(stderr, "Error on user input. Exiting...\n");
    return 5;
  }

  return 0;

}
