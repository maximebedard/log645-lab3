#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include <unistd.h>
#include <stddef.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int err, processors, rank, m, n, np, td, h;
  err = MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  m = atoi(argv[1]);
  n = atoi(argv[2]);
  np = atoi(argv[3]);
  td = atoi(argv[4]);
  h = atoi(argv[5]);

  err = MPI_Finalize();
  return err;
}
