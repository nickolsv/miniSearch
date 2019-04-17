#ifndef __MS_MISC_H__
#define __MS_MISC_H__

// Miscellaneous functions and data structures for minisearch

typedef struct docAndScore {
  // Packs docIDs and scores together

  int docID;
  float score;
} docAndScore;

void quickSort(docAndScore **, int, int);
int getDigits(int);

#endif /* end of include guard: __MS_MISC_H__ */
