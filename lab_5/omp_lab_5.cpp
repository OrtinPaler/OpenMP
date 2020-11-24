#include <iostream>
#include <omp.h>
using namespace std;

int num_mass = 0;

int** create_mass(int rows, int cols) {
  int **mass = new int* [rows];

  for (int i = 0; i < rows; i++) {
    mass[i] = new int [cols];
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      mass[i][j] = rand() % 1000;
    }
  }

  return mass;
}

// одномерный массив (one-dimensional array)
int* create_mass_oda(int N) {
  int *mass = new int[N];

  for (int i = 0; i < N; i++) {
    mass[i] = rand() % 1000;
  }

  return mass;
}

void create_new_mass_parallel_reduction(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  #pragma omp parallel num_threads (3) private (i)
  {
    #pragma omp for private (j) reduction (+:sum)
    for (i = 0; i < rows; i++) {
      new_mass[i] = new int [cols];
      for (j = 0; j < cols; j++) {
        new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
        sum += new_mass[i][j];
      }
    }
  }

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

void create_new_mass_parallel_atomic(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  #pragma omp parallel num_threads (3) shared (sum, new_mass, mass_one, mass_two) private (i)
  {
    #pragma omp for private (j)
    for (i = 0; i < rows; i++) {
      new_mass[i] = new int [cols];
      for (j = 0; j < cols; j++) {
        new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
        #pragma omp atomic
          sum += new_mass[i][j];
      }
    }
  }

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

void create_new_mass_parallel_critical(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  #pragma omp parallel num_threads (3) private (i)
  {
    #pragma omp for private (j)
    for (i = 0; i < rows; i++) {
      new_mass[i] = new int [cols];
      for (j = 0; j < cols; j++) {
        new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
        #pragma omp critical
        sum += new_mass[i][j];
      }
    }
  }
  

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

void create_new_mass_parallel_sections(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  #pragma omp sections
  {
    #pragma omp section
    {
      for (i = 0; i < rows; i++) {
        new_mass[i] = new int [cols];
        for (j = 0; j < cols / 2; j++) {
          new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
          sum += new_mass[i][j];
        }
      }
    }
    #pragma omp section
    {
      for (i = 0; i < rows; i++) {
        new_mass[i] = new int [cols];
        for (j = cols / 2; j < cols; j++) {
          new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
          sum += new_mass[i][j];
        }
      }
    }
  }

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

// одномерный массив (one-dimensional array)
void create_new_mass_parallel_sections_oda(int &sum, int N, int *mass_one, int *mass_two) {
  int *new_mass = new int[N];
  int i;

  #pragma omp sections
  {
    #pragma omp section
    {
      for (i = 0; i < N / 3; i++) {
        new_mass[i] = (mass_one[i] > mass_two[i]) ? mass_one[i] : mass_two[i];
        sum += new_mass[i];
      }
    }
    #pragma omp section
    {
      for (i = N / 3; i < 2 * N / 3; i++) {
        new_mass[i] = (mass_one[i] > mass_two[i]) ? mass_one[i] : mass_two[i];
        sum += new_mass[i];
      }
    }
    #pragma omp section
    {
      for (i = 2 * N / 3; i < N; i++) {
        new_mass[i] = (mass_one[i] > mass_two[i]) ? mass_one[i] : mass_two[i];
        sum += new_mass[i];
      }
    }
  }

  delete[] new_mass;
}

void create_new_mass_unparallel(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  for (i = 0; i < rows; i++) {
    new_mass[i] = new int [cols];
    for (j = 0; j < cols; j++) {
      new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
      sum += new_mass[i][j];
    }
  }

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

// одномерный массив (one-dimensional array)
void create_new_mass_unparallel_sections_oda(int &sum, int N, int *mass_one, int *mass_two) {
  int *new_mass = new int[N];
  int i;

  for (i = 0; i < N; i++) {
    new_mass[i] = (mass_one[i] > mass_two[i]) ? mass_one[i] : mass_two[i];
    sum += new_mass[i];
  }

  delete[] new_mass;
}

void print_mass(int **mass, int rows, int cols) {
  cout << "Array " << ++num_mass << "\n";
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      cout << mass[i][j] << "\t";
    }
    cout << "\n";
  }
}

void delete_mass(int **mass, int rows) {
  for (int i = 0; i < rows; i++) {
    delete[] mass[i];
  }
  delete[] mass;
  num_mass--;
}

int main() {
  // проверяем то, что компилятор поддерживает OpenMP и узнаем его версию
  #ifdef _OPENMP
    cout << "\nOpenMP supported! Version " << _OPENMP << "\n";
  #endif // _OPENMP

  int N = 1500;
  int N_oda = 3125000;

  int **A = create_mass(N, N);
  int **B = create_mass(N, N);
  
  int *A_oda = create_mass_oda(N_oda);
  int *B_oda = create_mass_oda(N_oda);

  int sum = 0;

  double start_time = omp_get_wtime();

  //create_new_mass_unparallel(sum, N, N, A, B);
  //create_new_mass_unparallel_sections_oda(sum, N_oda, A_oda, B_oda);
  //create_new_mass_parallel_reduction(sum, N, N, A, B);
  //create_new_mass_parallel_atomic(sum, N, N, A, B);
  //create_new_mass_parallel_critical(sum, N, N, A, B);
  create_new_mass_parallel_sections(sum, N, N, A, B);
  //create_new_mass_parallel_sections_oda(sum, N_oda, A_oda, B_oda);


  double end_time = omp_get_wtime();
  double tick = omp_get_wtick();

  cout << "\nSum: " << sum
       << "\nTime: " << end_time - start_time
       << "\nTimer accuracy: " << tick << "\n\n";
       

  delete_mass(A, N);
  delete_mass(B, N);

  delete[] A_oda, B_oda;

  return 0;
}