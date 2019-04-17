#include <stdlib.h>

#include "../header/documentmap.h"
#include "../header/myindex.h"

char **fileToMap(FILE *fp, int *linesNo, int**wc)
{
  // Reads text from file fp and saves it to an array with each
  // array element containing a line of text. Returns said array
  // as well as an array containing the number of words in each document

  char **map, **tmap;
  int  *wcount, *twcount ,wordc;
  char *line = NULL;
  int linesSaved = 0,mapSize = 4;

  map = malloc(mapSize*sizeof(char *));
  wcount = malloc(mapSize*sizeof(int));

  if( map == NULL || wcount == NULL )                               return NULL;

  while( readAndCheckLine(fp,&line,linesSaved,&wordc) == 0 )                    // Read and filter each line and add it to the array
  {
    if(linesSaved == mapSize)                                                   // If lines can't fit in the array anymore
    {                                                                           // arraysize is doiubled
      mapSize *= 2;
      tmap = realloc(map,mapSize*sizeof(char *));
      twcount = realloc(wcount,mapSize*sizeof(int));

      if( tmap == NULL || twcount == NULL )
      {
        free(map);
        free(wcount);
        return NULL;
      }

      map    = tmap;
      wcount = twcount;

    }

    wcount[linesSaved] = wordc;
    map[linesSaved] = line;
    line = NULL;
    linesSaved++;
  }

  if( linesSaved < mapSize )                                                    // Shrink array down to the exact number of documents (if needed)
  {
    map = realloc(map,linesSaved*sizeof(char *));
    wcount = realloc(wcount,linesSaved*sizeof(int));
  }

  *linesNo = linesSaved;
  *wc = wcount;

  return map;
}



void mapInit(documentMap *docMap)
{
  // Initializes the document map

  docMap->mapArray  = NULL;
  docMap->wordCount = NULL;
  docMap->avgWrds   = 0;
  docMap->docCount  = 0;
}


void mapDestroy(documentMap *docMap)
{
  // Destroys the document map

  if( docMap->docCount != 0 )
  {
    for( int i = 0; i < docMap->docCount; i++ )
    {
      free(docMap->mapArray[i]);
    }
    free(docMap->wordCount);
    free(docMap->mapArray);
  }
}
