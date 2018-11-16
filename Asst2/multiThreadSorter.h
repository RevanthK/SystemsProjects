#ifndef sorter   /* Include guard */
#define sorter

struct Node {
   int rowID;
   char* sortingVal;
};

char* trimwhitespace(char* str);

void merge(struct Node arr[], int l, int m, int r, bool isAlpha);

void mergeSort(struct Node arr[], int l, int r, bool isAlpha);

#endif 