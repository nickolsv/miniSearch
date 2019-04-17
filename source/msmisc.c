#include "../header/msmisc.h"

void quickSort(docAndScore **dsArray,int low, int high)
{
  // Sorts an array of docIDs and scores by score in descending order

  docAndScore *array = *dsArray,temp,pivot;

  if(low >= high) return;                                                       // If partition is empty return

  pivot = array[high];
  int wall = low - 1;

  for( int i = low ; i < high ; i++)                                            // Partition the array using the high element as pivot
  {
    if( array[i].score >= pivot.score)
    {
      wall++;
      temp = array[i];
      array[i] = array[wall];
      array[wall] = temp;
    }
  }
  temp = array[wall+1];
  array[wall+1] = array[high];
  array[high] = temp;

  // Now the pivot is in its final position, (wall+1) with
  // every element with a score greater than the pivot
  // to the left of the pivot and every element with a score
  // less than that of the pivot to the right

  quickSort(&array,low,wall);                                                   // Quicksort each partition
  quickSort(&array,wall+2,high);

  *dsArray = array;
}

int getDigits(int number)
{
  // Returns the number of digits of an integer ( base 10 )

  int sum = 0;
  if( number == 0 )     return 1;
  if( number < 0 )      sum++;                                                  // Minus sign takes up space too
  while( number != 0 )
  {
    sum+=1;
    number = number/10;
  }
  return sum;
}
