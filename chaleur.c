#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <mpi.h>

#define SLEEP_TIME 5

void chaleur_seq(int m, int n, int np, int td, int h);
void chaleur_par(int m, int n, int np, double td, double h);
void initialize_matrix(int m, int n, double matrix[m][n]);
void print_matrix(int m, int n, double matrix[m][n]);
double get_current_time();

int main(int argc, char **argv) {
  int err, rank, m, n, np;
  double start, end, td, h;
  err = MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  m  = atoi(argv[1]);
  n  = atoi(argv[2]);
  np = atoi(argv[3]);
  td = atof(argv[4]);
  h  = atof(argv[5]);


  if (rank == 0) {
    printf("m=%d, n=%d, np=%d, td=%.2f, h=%.2f\n", m, n, np, td, h);

    printf("Version parallèle\n");
    start = get_current_time();
    chaleur_par(m, n, np, td, h);
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
    chaleur_par(m, n, np, td, h);
  }

  err = MPI_Finalize();
  return err;
}

void chaleur_seq(int m, int n, int np, int td, int h) {
  double matrix[m][n];
  int i, j, k;

  printf(" => init\n");

  initialize_matrix(m, n, matrix);
  print_matrix(m, n, matrix);

  printf(" => eval\n");

  for(k = 1; k < np; k++) {
    double m2[m][n];
    memset(m2, 0, sizeof(m2));

    for(i = 1; i < m - 1; i++) {
      for(j = 1; j < n - 1; j++) {
        usleep(SLEEP_TIME);
        m2[i][j] = (1 -4.0* td / (h*h)) * matrix[i][j] + ((double)td / (h*h)) * (matrix[i - 1][j] + matrix[i + 1][j] + matrix[i][j - 1] + matrix[i][j + 1]);
      }
    }
    memcpy(matrix, m2, sizeof(matrix));
  }
  print_matrix(m, n, matrix);
}

void chaleur_par(int m, int n, int np, double td, double h) {
  int processors, rank;
  int i;
  MPI_Comm_size(MPI_COMM_WORLD, &processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0) {

  } else {

  }
}

void initialize_matrix(int m, int n, double matrix[m][n]) {
  int i, j;

  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++)
      matrix[i][j] = (i * (m - i - 1)) * (j * (n - j - 1));
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
