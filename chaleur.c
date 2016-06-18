#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>
#include <mpi.h>

void chaleur_seq(int m, int n, int np, int td, int h);
void chaleur_par(int processors, int rank, int m, int n, int np, int td, int h);
void initialize_matrix(int m, int n, double matrix[m][n], int value);
void print_matrix(int m, int n, double matrix[m][n]);
double get_current_time();

int main(int argc, char **argv) {
  int err, processors, rank, m, n, np, td, h;
  double start, end;
  err = MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  m  = atoi(argv[1]);
  n  = atoi(argv[2]);
  np = atoi(argv[3]);
  td = atoi(argv[4]);
  h  = atoi(argv[5]);

  if (rank == 0) {
    printf("Version parallèle\n");
    start = get_current_time();
    chaleur_par(processors, rank, m, n, np, td, h);
    end = get_current_time();
    double dt_par = end - start;
    printf("Temps d'éxecution : %f\n", dt_par);

    printf("Version séquentielle\n");
    start = get_current_time();
    chaleur_seq(m, n, np, td, h);
    end = get_current_time();
    double dt_seq = end - start;
    printf("Temps d'éxecution : %f\n", dt_seq);
  } else {
    chaleur_par(processors, rank, m, n, np, td, h);
  }

  err = MPI_Finalize();
  return err;
}

void chaleur_seq(int m, int n, int np, int td, int h) {
  double matrix[m][n];
  int i, j, k;
  initialize_matrix(m, n, matrix, 0.0);

  printf(" => init\n");
  for(i = 0; i < m; i++) {
    for(j = 0; j < n; j++) {
      matrix[i][j] = (i * (m - i - 1)) * (j * (n - j - 1));
    }
  }
  print_matrix(m, n, matrix);

  printf(" => eval\n");
  for(k = 1; k < np; k++) {
    for(i = 1; i < m; i++) {
      for(j = 1; j < n; j++) {
        // i can't count for shit.
        matrix[i][j] = ((1 - (4 * td / h * h)) * matrix[i][j]) +
          ((td / h * h) * (matrix[i - 1][j] + matrix[i + 1][j] + matrix[i][j - 1] + matrix[i][j + 1]));
      }
    }
  }
  print_matrix(m, n, matrix);
}

void chaleur_par(int processors, int rank, int m, int n, int np, int td, int h) {

}

void initialize_matrix(int m, int n, double matrix[m][n], int value) {
  int i, j;

  for(i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      matrix[i][j] = value;
}

void print_matrix(int m, int n, double matrix[m][n]) {
  int i, j;
  for(i = 0; i < m; i++) {
    for(j = 0; j < n; j++) {
      printf("|%15.2f", matrix[i][j]);
    }
    printf("|\n");
  }
}

double get_current_time() {
  struct timeval tp;
  gettimeofday (&tp, NULL);
  return (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
}
