#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutexSum;

typedef struct
{
	int start;
	int end;
	long long int *num_hit; // to access global value of hit amount
} 

Arg_create_t; // arguments pass to pthread_create() 

void* gen_pi(void *args) {

	Arg_create_t *arg = (Arg_create_t *)args;
        int start = arg->start;
	int end = arg->end;
        long long int *num_hit = arg->num_hit;
        long long int local_sum_hit = 0;
        unsigned int seed = 999;
	for (int i = start;i < end;i++) {
	        double x = ((double) rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
	        double y = ((double) rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;	        
	        double dist = x*x + y*y;
	        if (dist <= 1.0)
		{
    	            local_sum_hit++;
		}
	}
	// access share variable
	pthread_mutex_lock(&mutexSum);
	*num_hit += local_sum_hit;
	pthread_mutex_unlock(&mutexSum);

	pthread_exit((void *)0);
}

int main(int argc, char** argv) {
	//parse arguments
	int num_thread = atoi(argv[1]);//convert string to integer
	long long int num_tosses = atoll(argv[2]);//convert string to long long integer
	int perPartitionSize = (num_tosses / num_thread);   
	pthread_t threads[num_thread];
	Arg_create_t arg[num_thread]; 
	// Initialize mutex lock
	pthread_mutex_init(&mutexSum, NULL);
	// Set pthread attribute and let pthread joinable
	pthread_attr_t attr;
   	pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        long long int *num_hit = (long long int *) malloc(sizeof(*num_hit));
        *num_hit = 0; // Init the number of hit as zero
	for (int i = 0;i < num_thread;i++) {
		/* Set argueemnts to each thread */
		arg[i].start = perPartitionSize * i;
		arg[i].end = perPartitionSize * (i+1);
		arg[i].num_hit = num_hit; // All threads point to the same memory address (Shared by all threads)  

	 	//Create a new thread and run calPi() function with correspoding arg[i] arguments
         	pthread_create(&threads[i], &attr, gen_pi, (void *) &arg[i]);
         }
                 
         /* Free attribute*/
         pthread_attr_destroy(&attr);                    
         /* Wait for the other threads*/
         void *status;
         for (int i = 0;i < num_thread;i++) {
         	pthread_join(threads[i], &status);
         }

         pthread_mutex_destroy(&mutexSum);
         double pi = 4 * ((*num_hit) / (double)num_tosses);
         printf("%.7lf\n", pi);
         return 0;
}
