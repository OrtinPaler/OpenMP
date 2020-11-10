#include <iostream>
#include <omp.h> 

const int NMAX = 20000; 
const int LIMIT = 20000; 

int main () {  
  
  int i, j; 
  float sum;  
  /*
  float a[NMAX][NMAX];  

  for (i = 0; i < NMAX; i++) {
    for (j = 0; j < NMAX; j++) {
      a[i][j] = i + j;
    } 
  }   
  */

 float **matrix = new float* [NMAX];
 for (int i = 0; i < NMAX; i++) {
   matrix[i] = new float[NMAX];
 }

 for (int i = 0; i < NMAX; i++) {
   for (int j = 0; j < NMAX; j++) {
     matrix[i][j] = rand() % 1000;
   }
 }

  

  double start = omp_get_wtime();

  #pragma omp parallel shared (matrix) if (NMAX > LIMIT) 
  {
    #pragma omp for private(i, j, sum)
      for (i = 0; i < NMAX; i++) {
        sum = 0;
        for (j = 0; j < NMAX; j++) {
          sum += matrix[i][j];
        }  
        //std::cout << "Sum of line items " << i << ": " << sum << std::endl; 
      }
  } 

  double end = omp_get_wtime();

  std::cout << "Time: " << end - start << std::endl;

  return 0;
} 