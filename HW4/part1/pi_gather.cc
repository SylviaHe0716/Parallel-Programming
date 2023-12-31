#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Status status;
    long long int num_task = tosses / world_size;
    int SEED = 78;
    int tag = 0;
    long long int count = 0;
    unsigned int seed = world_rank * SEED, dest = 0;
    long long int local_cnt = 0;
    srand(seed);
    for (long long int t = 0; t < num_task; t++){
	    double x = (double) rand_r(&seed) / RAND_MAX;
	    double y = (double) rand_r(&seed) / RAND_MAX;
	    double d = x * x + y * y;
	    if (d <= 1){
		    local_cnt++;
	    }
    }

    // TODO: use MPI_Gather

    long long int *recvbuf;
    if (world_rank == 0){
	    recvbuf = (long long int*)malloc(sizeof(long long int) * world_size);
    }
    MPI_Gather(&local_cnt, 1, MPI_UNSIGNED_LONG, recvbuf, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);


    if (world_rank == 0)
    {
        // TODO: PI result
        for (int i = 0; i < world_size; i++){
		count += recvbuf[i];
	}
	pi_result = 4 * count / (double) tosses;
	
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
