#include <ctime>
#include <iostream>
#include <omp.h>

using namespace std;

int main() {

  // задаем первое псевдорандомное число
  srand(/*time(nullptr)*/0);

    // проверяем то, что компилятор поддерживает OpenMP и узнаем его версию
#ifdef _OPENMP
    cout << endl << "OpenMP supported! Version " << _OPENMP << endl;
#endif // _OPENMP

  const int n_row = 500, n_col = 500; // кол-во строк и столбцов в матрице
  int matrix[n_row][n_col];

  // выводим размерность матрицы
  cout << "Matrix dimension: n_row = " << n_row << ", n_col = " << n_col << endl << endl;

  // инициализируем матрицу числами от 1 до 100
  for (int i = 0; i < n_row; ++i) {
    for (int j = 0; j < n_col; ++j) {
      matrix[i][j] = rand() % 100 + 1;
    }
  }

  int summa = 0; // кол-во единиц в строке
  int final_array[n_row]; // массив кол-ва единиц в строках

  int index_i, index_j, index_t, index_k;

  // функция omp_get_wtime() возвращает в вызвавшей нити астрономическое время в секундах
  double start_time = omp_get_wtime();

  #pragma omp parallel num_threads (2) private (index_i)
  {
    #pragma omp for schedule (static, n_row / 2) private (index_j, index_t, index_k, summa)
    // в каждой строке index_i,
    for (index_i = 0; index_i < n_row; index_i++) {
      // число на каждой позиции index_j,
      for (index_j = 0; index_j < n_col - 1; index_j++) {
        // умножим на число index_t
        for (index_t = index_j + 1; index_t < n_col; index_t++) {
          // считаем числа, полученные при попарном умножении каждого элемента строки
          int count = matrix[index_i][index_j] * matrix[index_i][index_t];

          // найдем кол-во единиц в бинарной записи числа count
          for (index_k = sizeof(count) * 8 - 1; index_k >= 0; index_k--) {
            summa += (int) ((count >> index_k) & 1);
          }
        }
      }
      final_array[index_i] = summa;
      summa = 0;
    }
  }
  
  double end_time = omp_get_wtime();

  // функция omp_get_wtick() возвращает в вызвавшей нити разрешение таймера в секундах
  double tick = omp_get_wtick();

  for (int i = 0; i < n_row; ++i) {
    cout << "Line " << i + 1 << ": " << final_array[i] << endl;
  }

  cout << "Time: " << end_time - start_time << endl << "Timer accuracy: " << tick << endl;

  return 0;
}