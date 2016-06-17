#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include <unistd.h>
#include <stddef.h>
#include <mpi.h>

#define MATRIX_LEN 8

void problem1(int rank, int iterations);
void problem2(int rank, int iterations);
void dispatchWorkers(int default_value, int workers);

struct Cell {
  int i;
  int j;
  int val;
};

MPI_Datatype mpi_cell_type;

int main(int argc, char const *argv[]) {
  int err, workers, rank;
  err = MPI_Init(&argc,(char ***) &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &workers);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int problem = atoi(argv[1]);
  int default_value = atoi(argv[2]);
  int iterations = atoi(argv[3]);

  int blocklengths[3] = {1, 1, 1};
  MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
  MPI_Aint offsets[3];
  offsets[0] = offsetof(struct Cell, i);
  offsets[1] = offsetof(struct Cell, j);
  offsets[2] = offsetof(struct Cell, val);

  MPI_Type_create_struct(3, blocklengths, offsets, types, &mpi_cell_type);
  MPI_Type_commit(&mpi_cell_type);

  if(rank == 0) {
    dispatchWorkers(default_value, workers);
  }
  else {
    if(problem == 1) {
      problem1(rank, iterations);
    }
    else if(problem == 2) {
      problem2(rank, iterations);
    }
  }

  err = MPI_Finalize();
  return err;
}

void dispatchWorkers(int default_value, int workers) {
  MPI_Status status;

  double timeStart, timeEnd, Texec;
  struct timeval tp;
  gettimeofday (&tp, NULL);
  timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;

  int p;
  for(p = 0; p < 64; p++) {
    struct Cell c;
    c.i = p % 8;
    c.j = p / 8;
    c.val = default_value;

    MPI_Send(&c, 1, mpi_cell_type, (p % 16) + 1, 0, MPI_COMM_WORLD);
  }

  int matrix[MATRIX_LEN][MATRIX_LEN];

  for (p = 0; p < 64; p++) {
    struct Cell c;
    MPI_Recv(&c, 1, mpi_cell_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

    matrix[c.i][c.j] = c.val;
  }

  int i, j;
  for (i = 0; i < MATRIX_LEN; i++) {
    for (j = 0; j < MATRIX_LEN; j++) {
      printf("%10d ", matrix[i][j]);
    }
    printf("\n");
  }

  gettimeofday (&tp, NULL);
  timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
  Texec = timeEnd - timeStart;

  printf("Temps d'éxécution: %f\n", Texec);
}

void problem1(int rank, int iterations) {
  MPI_Status status;

  int i;
  for(i = 0; i < 4; i++) {
    struct Cell c;
    MPI_Recv(&c, 1, mpi_cell_type, 0, 0, MPI_COMM_WORLD, &status);

    int k;
    for(k = 0; k < iterations; k++) {
      c.val += (c.i + c.j) * (k + 1);
      usleep(100000);
    }
    MPI_Send(&c, 1, mpi_cell_type, 0, 0, MPI_COMM_WORLD);
  }
}

void problem2(int rank, int iterations) {
  MPI_Status status;

  int i;
  for(i = 0; i < 4; i++) {
    struct Cell c;
    MPI_Recv(&c, 1, mpi_cell_type, 0, 0, MPI_COMM_WORLD, &status);

    int k;
    int receiver = rank < 9 ? rank + 8 : rank % 8;
    if(receiver == 0) { receiver += 8; }

    for(k = 0; k < iterations; k++) {
      struct Cell previous_c;
      if(c.j != 0) {
        MPI_Recv(&previous_c, 1, mpi_cell_type, receiver, 0, MPI_COMM_WORLD, &status);

        c.val += previous_c.val * (k + 1);
      }
      else
      {
        c.val += c.i * (k + 1);
      }
      usleep(100000);
      MPI_Send(&c, 1, mpi_cell_type, receiver, 0, MPI_COMM_WORLD);
    }

    MPI_Send(&c, 1, mpi_cell_type, 0, 0, MPI_COMM_WORLD);
  }
}
