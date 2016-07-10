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

// #define DEBUG 0

#ifdef DEBUG
#  define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#else
#  define DEBUG_PRINT(...) do{ } while (0)
#endif

void chaleur_seq(int m, int n, int np, double tdh2);
void chaleur_par(int m, int n, int np, double tdh2);

void matrix_init(int m, int n, double matrix[2][m][n]);
void matrix_print(int m, int n, double matrix[m][n]);

void types_init();
double get_current_time();

MPI_Datatype mpi_message_type;

struct Message {
  int x_offset;
  int y_offset;
  int left;
  int right;
};

int main(int argc, char **argv) {
  int err, rank, m, n, np;
  double start, end, td, h, tdh2;

  err = MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  types_init();

  m    = atoi(argv[1]);
  n    = atoi(argv[2]);
  np   = atoi(argv[3]);
  td   = atof(argv[4]);
  h    = atof(argv[5]);
  tdh2 = td / (h * h);


  if (rank == 0) {
    printf("m=%d, n=%d, np=%d, td=%.2f, h=%.2f, tdh2=%.2f\n", m, n, np, td, h, tdh2);

    printf("Version parallèle\n");
    start = get_current_time();
    chaleur_par(m, n, np, tdh2);
    end = get_current_time();
    double dt_par = end - start;
    printf("Temps d'éxecution : %f\n", dt_par);

    printf("Version séquentielle\n");
    start = get_current_time();
    chaleur_seq(m, n, np, tdh2);
    end = get_current_time();
    double dt_seq = end - start;
    printf("Temps d'éxecution : %f\n", dt_seq);
  } else {
    chaleur_par(m, n, np, tdh2);
  }

  err = MPI_Finalize();
  return err;
}

void chaleur_seq(int m, int n, int np, double tdh2) {
  double matrix[2][m][n];
  int i, j, k;
  int current = 0;

  matrix_init(m, n, matrix);

  printf("init\n====\n");
  matrix_print(m, n, matrix[current]);

  for(k = 1; k < np; k++) {
    current = k % 2;

    for(i = 1; i < m - 1; i++) {
      for(j = 1; j < n - 1; j++) {
        usleep(SLEEP_TIME);

        DEBUG_PRINT("matrix[current=%d][i=%d][j=%d]=%.2f\n", current, i, j, matrix[current][i][j]);
        DEBUG_PRINT("up   =%.2f\n", matrix[1-current][i - 1][j]);
        DEBUG_PRINT("down =%.2f\n", matrix[1-current][i + 1][j]);
        DEBUG_PRINT("left =%.2f\n", matrix[1-current][i][j - 1]);
        DEBUG_PRINT("right=%.2f\n", matrix[1-current][i][j + 1]);

        matrix[current][i][j] = (1.0 - (4.0 * tdh2)) * matrix[1-current][i][j] +
          tdh2 * (matrix[1-current][i - 1][j] +
                  matrix[1-current][i + 1][j] +
                  matrix[1-current][i][j - 1] +
                  matrix[1-current][i][j + 1]);
      }
    }
  }
  printf("eval\n====\n");
  matrix_print(m, n, matrix[current]);
}

void chaleur_par(int m, int n, int np, double tdh2) {
  // int processors, rank, i, j, start, end, offset;
  // int current = 0;
  // double matrix[2][m][n];
  // MPI_Status status;
  // MPI_Comm_size(MPI_COMM_WORLD, &processors);
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //
  // if(rank == MASTER_WORKER) {
  //   matrix_init(m, n, matrix);
  //
  //   printf("init\n====\n");
  //   matrix_print(m, n, matrix[current]);
  //
  //   int workers, average_row, extra, destination;
  //   workers     = processors - 1;
  //   average_row = m / workers;
  //   extra       = m % workers;
  //   offset      = 0;
  //
  //   DEBUG_PRINT("workers=%d average_row=%d extra=%d\n", workers, average_row, extra);
  //   for(i = 1; i <= workers; i++) {
  //     struct Message msg;
  //     msg.x_offset = offset;
  //     msg.y_offset = (i <= extra) ? average_row + 1 : average_row;
  //     msg.left     = (i == 1 ? -1 : i - 1);
  //     msg.right    = (i == workers ? -1 : i + 1);
  //
  //     // destination
  //     destination = i;
  //
  //     MPI_Send(&msg, 1, mpi_message_type, destination, START_TAG, MPI_COMM_WORLD);
  //     MPI_Send(&matrix[msg.x_offset][0], msg.y_offset * n, MPI_DOUBLE, destination, START_TAG, MPI_COMM_WORLD);
  //     DEBUG_PRINT(
  //       "send => dest=%d, x_offset=%d, y_offset=%d, left=%d, right=%d\n",
  //       destination, msg.x_offset, msg.y_offset, msg.left, msg.right
  //     );
  //
  //     offset += msg.y_offset;
  //   }
  //
  //   for(i = 1; i <= workers; i++) {
  //     struct Message msg;
  //     MPI_Recv(&msg, 1, mpi_message_type, i, DONE_TAG, MPI_COMM_WORLD, &status);
  //     MPI_Recv(&matrix[msg.x_offset][0], msg.y_offset * n, MPI_DOUBLE, i, DONE_TAG, MPI_COMM_WORLD, &status);
  //   }
  //
  //   printf("eval\n====\n");
  //   matrix_print(m, n, matrix[current]);
  // } else {
  //   struct Message msg;
  //   MPI_Recv(&msg, 1, mpi_message_type, MASTER_WORKER, START_TAG, MPI_COMM_WORLD, &status);
  //   MPI_Recv(&matrix[msg.x_offset][0], msg.y_offset * n, MPI_DOUBLE, MASTER_WORKER, START_TAG, MPI_COMM_WORLD, &status);
  //
  //   start = msg.x_offset;
  //   end   = msg.x_offset + msg.y_offset - 1;
  //   if(msg.x_offset == 0) start = 1;
  //   if(msg.x_offset + msg.y_offset == m) end -= 1;
  //
  //   DEBUG_PRINT("recv => worker=%d, start=%d, end=%d\n", rank, start, end);
  //   for(i = 0; i < np; i++) {
  //     current = i % 2;
  //     // left neighbor
  //     if(msg.left != -1) {
  //       MPI_Send(&matrix[current][msg.x_offset][0], n, MPI_FLOAT, msg.left, RIGHT_TAG, MPI_COMM_WORLD);
  //       MPI_Recv(&matrix[current][msg.x_offset-1][0], n, MPI_FLOAT, msg.left, LEFT_TAG, MPI_COMM_WORLD, &status);
  //     }
  //
  //     // right neighbor
  //     if(msg.right != -1) {
  //       MPI_Send(&matrix[current][msg.x_offset+msg.y_offset-1][0], n, MPI_FLOAT, msg.right, LEFT_TAG, MPI_COMM_WORLD);
  //       MPI_Recv(&matrix[current][msg.x_offset+msg.y_offset][0], n, MPI_FLOAT, msg.right, RIGHT_TAG, MPI_COMM_WORLD, &status);
  //     }
  //
  //     // we are now able to set the value
  //     for(int a = start; a <= end; a++) {
  //       for(int b = 1; b < n - 2; b++) {
  //         matrix[current][a][b] = 1;
  //       }
  //     }
  //   }
  //   DEBUG_PRINT("DONE\n");
  //   MPI_Send(&msg, 1, mpi_message_type, MASTER_WORKER, DONE_TAG, MPI_COMM_WORLD);
  //   MPI_Send(&matrix[msg.x_offset][0], msg.y_offset*n, MPI_FLOAT, MASTER_WORKER, DONE_TAG, MPI_COMM_WORLD);
  // }
}

void matrix_init(int m, int n, double matrix[2][m][n]) {
  int i, j;

  for(i = 0; i < m; i++) {
    for(j = 0; j < n; j++) {
      matrix[0][i][j] = (i * (m - i - 1)) * (j * (n - j - 1));
      matrix[1][i][j] = matrix[0][i][j];
    }
  }
}

void matrix_print(int m, int n, double matrix[m][n]) {
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

void types_init() {
  int blocklengths[4] = {1, 1, 1, 1};
  MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
  MPI_Aint offsets[4];
  offsets[0] = offsetof(struct Message, x_offset);
  offsets[1] = offsetof(struct Message, y_offset);
  offsets[2] = offsetof(struct Message, left);
  offsets[3] = offsetof(struct Message, right);
  MPI_Type_create_struct(4, blocklengths, offsets, types, &mpi_message_type);
  MPI_Type_commit(&mpi_message_type);
}
