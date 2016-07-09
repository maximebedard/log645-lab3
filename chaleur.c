#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <mpi.h>

#define SLEEP_TIME    5
#define MASTER_WORKER 0
#define LEFT_TAG      3
#define RIGHT_TAG     4
#define DONE_TAG      5
#define START_TAG     6

#define DEBUG 1

#ifdef DEBUG
#  define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#else
#  define DEBUG_PRINT(...) do{ } while (0)
#endif

void chaleur_seq(int m, int n, int np, int td, int h);
void chaleur_par(int m, int n, int np, double td, double h);
void initialize_matrix(int m, int n, double matrix[m][n]);
void print_matrix(int m, int n, double matrix[m][n]);
void init_types();
double get_current_time();

MPI_Datatype mpi_info_type;

struct Info {
  int offset;
  int row;
  int left;
  int right;
};

int main(int argc, char **argv) {
  int err, rank, m, n, np;
  double start, end, td, h;
  err = MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  init_types();

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

  initialize_matrix(m, n, matrix);

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
  int processors, rank, i, j, start, end, offset;
  double matrix[m][n];
  MPI_Status status;
  MPI_Comm_size(MPI_COMM_WORLD, &processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == MASTER_WORKER) {
    initialize_matrix(m, n, matrix);
    int workers, average_row, extra, destination;
    workers     = processors - 1;
    average_row = m / workers;
    extra       = m % workers;
    offset      = 0;

    DEBUG_PRINT("workers=%d average_row=%d extra=%d\n", workers, average_row, extra);
    for(i = 1; i <= workers; i++) {
      struct Info info;
      info.offset = offset;
      info.row    = (i <= extra) ? average_row + 1 : average_row;
      info.left   = (i == 1 ? -1 : i - 1);
      info.right  = (i == workers ? -1 : i + 1);

      // destination
      destination = i;

      MPI_Send(&info, 1, mpi_info_type, destination, START_TAG, MPI_COMM_WORLD);
      MPI_Send(&matrix[info.offset][0], info.row * n, MPI_DOUBLE, destination, START_TAG, MPI_COMM_WORLD);
      DEBUG_PRINT(
        "send => dest=%d, offset=%d, row=%d, left=%d, right=%d\n",
        destination, info.offset, info.row, info.left, info.right
      );

      offset += info.row;
    }

    for(i = 1; i <= workers; i++) {
      struct Info info;
      MPI_Recv(&info, 1, mpi_info_type, i, DONE_TAG, MPI_COMM_WORLD, &status);
      MPI_Recv(&matrix[info.offset][0], info.row * n, MPI_DOUBLE, i, DONE_TAG, MPI_COMM_WORLD, &status);
    }

    print_matrix(m, n, matrix);
  } else {
    struct Info info;
    MPI_Recv(&info, 1, mpi_info_type, MASTER_WORKER, START_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(&matrix[info.offset][0], info.row * n, MPI_DOUBLE, MASTER_WORKER, START_TAG, MPI_COMM_WORLD, &status);

    start = info.offset;
    end   = info.offset + info.row - 1;
    if(info.offset == 0) start = 1;
    if(info.offset + info.row == m) end -= 1;

    DEBUG_PRINT("recv => worker=%d, start=%d, end=%d\n", rank, start, end);
    for(i = 0; i < np; i++) {
      double m2[m][n];
      memset(m2, 0, sizeof(m2));

      // left neighbor
      if(info.left != -1) {
        MPI_Send(&m2[info.offset][0], n, MPI_FLOAT, info.left, RIGHT_TAG, MPI_COMM_WORLD);
        MPI_Recv(&m2[info.offset-1][0], n, MPI_FLOAT, info.left, LEFT_TAG, MPI_COMM_WORLD, &status);
      }

      // right neighbor
      if(info.right != -1) {
        MPI_Send(&m2[info.offset+info.row-1][0], n, MPI_FLOAT, info.right, LEFT_TAG, MPI_COMM_WORLD);
        MPI_Recv(&m2[info.offset+info.row][0], n, MPI_FLOAT, info.right, RIGHT_TAG, MPI_COMM_WORLD, &status);
      }

      // update
      for(int a = start; a <= end; a++) {
        for(int b = 1; b < n - 2; b++) {
          m2[0][0] = 1;
        }
      }

      memcpy(matrix, m2, sizeof(matrix));
    }
    DEBUG_PRINT("DONE\n");
    MPI_Send(&info, 1, mpi_info_type, MASTER_WORKER, DONE_TAG, MPI_COMM_WORLD);
    MPI_Send(&matrix[info.offset][0], info.row*n, MPI_FLOAT, MASTER_WORKER, DONE_TAG, MPI_COMM_WORLD);
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

void init_types() {
  int blocklengths[4] = {1, 1, 1, 1};
  MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
  MPI_Aint offsets[4];
  offsets[0] = offsetof(struct Info, offset);
  offsets[1] = offsetof(struct Info, row);
  offsets[2] = offsetof(struct Info, left);
  offsets[3] = offsetof(struct Info, right);
  MPI_Type_create_struct(4, blocklengths, offsets, types, &mpi_info_type);
  MPI_Type_commit(&mpi_info_type);
}
