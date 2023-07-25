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

    // TODO: init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Status status;
    long long int num_task = tosses / world_size;
    int SEED = 78;
    int tag = 0;
    long long int count = 0;
    
    //for generating random num
    unsigned int seed = world_rank*SEED, dest = 0;
    long long int local_cnt = 0;
    srand(seed);
    
    for (long long int t = 0; t < num_task; t++) {
	double x = (double)rand_r(&seed)/RAND_MAX;
	double y = (double)rand_r(&seed)/RAND_MAX;
  	double d = x*x + y*y;
	if(d <= 1) {
	    local_cnt++;
	}
    }
    if (world_rank > 0)
    {
        // TODO: handle workers
    	MPI_Send(&local_cnt, 1,MPI_UNSIGNED_LONG, dest, tag, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: master
	count = local_cnt;
	long long int buf;
	for(int s =1; s < world_size; s++) {
		MPI_Recv(&buf, 1, MPI_UNSIGNED_LONG, s, tag, MPI_COMM_WORLD, &status);
		count += buf;
	}
    }

    if (world_rank == 0)
    {
        // TODO: process PI result
	pi_result = 4*count/(double)tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
