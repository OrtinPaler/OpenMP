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

void create_new_mass_parallel_lock(int &sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;
  omp_lock_t lock;
	omp_init_lock(&lock);

  #pragma omp parallel num_threads (2) private (i)
  {
    #pragma omp for private (j) reduction (+:sum)
    for (i = 0; i < rows; i++) {
      new_mass[i] = new int [cols];
      for (j = 0; j < cols; j++) {
        omp_set_lock(&lock);
        new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
        sum += new_mass[i][j];
        omp_unset_lock(&lock);
      }
    }
  }
  omp_destroy_lock(&lock);

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
  }
  delete[] new_mass;
}

void create_new_mass_parallel_barrier(int &main_sum, int **sum, int rows, int cols, int **mass_one, int **mass_two) {
  int **new_mass = new int* [rows];
  int i, j;

  #pragma omp parallel num_threads (3) private (i)
  {
    #pragma omp for private (j)
    for (i = 0; i < rows; i++) {
      new_mass[i] = new int [cols];
      for (j = 0; j < cols; j++) {
        new_mass[i][j] = (mass_one[i][j] > mass_two[i][j]) ? mass_one[i][j] : mass_two[i][j];
        sum[i][j] += new_mass[i][j];
      }
    }
    #pragma omp barrier
		for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        if (omp_get_thread_num() == 0)
          main_sum += sum[i][j];
      }
    }  
  }

  for (int i = 0; i < rows; i++) {
    delete[] new_mass[i];
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

  int N = 3000;

  int **A = create_mass(N, N);
  int **B = create_mass(N, N);

  int **mass_sum = new int* [N];
  for (int i = 0; i < N; i++) {
    mass_sum[i] = new int [N];
  }

  int sum = 0;

  double start_time = omp_get_wtime();

  //create_new_mass_unparallel(sum, N, N, A, B);
  //create_new_mass_parallel_reduction(sum, N, N, A, B);
  //create_new_mass_parallel_lock(sum, N, N, A, B);
  create_new_mass_parallel_barrier(sum, mass_sum, N, N, A, B);

  double end_time = omp_get_wtime();
  double tick = omp_get_wtick();

  cout << "\nSum: " << sum
       << "\nTime: " << end_time - start_time
       << "\nTimer accuracy: " << tick << "\n\n";
      
  delete_mass(A, N);
  delete_mass(B, N);

  return 0;
}