#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define SLEEP_TIME 5

void chaleur_seq(int m, int n, int np, int td, int h);
void chaleur_par(int m, int n, int np, double td, double h);
void initialize_matrix(int m, int n, double matrix[m][n]);
void print_matrix(int m, int n, double matrix[m][n]);
double get_current_time();

struct Cell {
  int i;
  int j;
  int val;
};

MPI_Datatype mpi_cell_type;

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

  for(k = 0; k < np; k++) {
    double m2[m][n];
    memset(m2, 0, sizeof(m2));
    for(i = 0; i < m; i++) {
      for(j = 0; j < n; j++) {
        usleep(SLEEP_TIME);
        int n1 = (j > 0) ? matrix[i][j - 1] : 0;
        int n2 = (j < n - 1) ? matrix[i][j + 1] : 0;
        int n3 = (i > 0) ? matrix[i - 1][j] : 0;
        int n4 = (i < m - 1) ? matrix[i + 1][j] : 0;

        m2[i][j] = (1 -4.0* td / (h*h)) * matrix[i][j] + ((double)td / (h*h)) * (n1 + n2 + n3 + n4);
      }
    }
    memcpy(matrix, m2, sizeof(matrix));
  }
  print_matrix(m, n, matrix);
}

void chaleur_par(int m, int n, int np, double td, double h) {
  int processors, rank;

  MPI_Comm_size(MPI_COMM_WORLD, &processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;

  int blocklengths[3] = {1, 1, 1};
  MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
  MPI_Aint offsets[3];
  offsets[0] = offsetof(struct Cell, i);
  offsets[1] = offsetof(struct Cell, j);
  offsets[2] = offsetof(struct Cell, val);

  MPI_Type_create_struct(3, blocklengths, offsets, types, &mpi_cell_type);
  MPI_Type_commit(&mpi_cell_type);

  if(rank == 0) {
    double matrix[m][n];
    int i, j;
    initialize_matrix(m, n, matrix);
    print_matrix(m, n, matrix);

    for(i = 0; i < m; i++) {
      for(j = 0; j < n; j++) {
        struct Cell cell;
        cell.i = i;
        cell.j = j;
        cell.val = matrix[i][j];
        MPI_Send(&cell, 1, mpi_cell_type, (i * j) % (processors - 1) + 1, 0, MPI_COMM_WORLD);
      }
    }

    for(i = 0; i < m; i++) {
      for(j = 0; j < n; j++) {
        struct Cell cell;
        MPI_Recv(&cell, 1, mpi_cell_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        matrix[cell.i][cell.j] = cell.val;
      }
    }

    printf(" => eval\n");
    print_matrix(m, n, matrix);

  } else {
    int cell_count = (int)ceil(m*n / processors);
    struct Cell cells[cell_count];
    int i, k, cell_tag;

    for(i = 0; i < cell_count;i++) {
      MPI_Recv(&cells[i], 1, mpi_cell_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }


    for(i = 0; i < cell_count; i++) {
      cell_tag = cells[i].j * m + cells[i].i;
      MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag - 1, MPI_COMM_WORLD);
      MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag + 1, MPI_COMM_WORLD);
      MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag - m, MPI_COMM_WORLD);
      MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag + m, MPI_COMM_WORLD);
    }
    for(k = 0; k < np; k++) {
      for(i = 0; i < cell_count; i++) {
        cell_tag = cells[i].j * m + cells[i].i;
        struct Cell neighbors[4];
        for(int c = 0; c < 4; c++) {
          MPI_Recv(&neighbors[c], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag, MPI_COMM_WORLD, &status);
        }

        cells[i].val = 1;

        MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag - 1, MPI_COMM_WORLD);
        MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag + 1, MPI_COMM_WORLD);
        MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag - m, MPI_COMM_WORLD);
        MPI_Send(&cells[i], 1, mpi_cell_type, MPI_ANY_SOURCE, cell_tag + m, MPI_COMM_WORLD);
      }
    }

    for(i = 0; i < cell_count;i++) {
      MPI_Send(&cells[i], 1, mpi_cell_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    }
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
