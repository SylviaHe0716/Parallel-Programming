#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence)
{

  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs

  int numNodes = num_nodes(g);
  double equal_prob = 1.0 / numNodes;
  double *old_solution;
  old_solution = (double*)malloc(sizeof(double) * g->num_nodes);
  //double sum_no_out = 0.0;
  bool converge = false;


  /*
     For PP students: Implement the page rank algorithm here.  You
     are expected to parallelize the algorithm using openMP.  Your
     solution may need to allocate (and free) temporary arrays.

     Basic page rank pseudocode is provided below to get you started:

     // initialization: see example code above
     score_old[vi] = 1/numNodes;

     while (!converged) {

       // compute score_new[vi] for all nodes vi:
       score_new[vi] = sum over all nodes vj reachable from incoming edges
                          { score_old[vj] / number of edges leaving vj  }
       score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;

       score_new[vi] += sum over all nodes v in graph with no outgoing edges
                          { damping * score_old[v] / numNodes }

       // compute how much per-node scores have changed
       // quit once algorithm has converged

       global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
       converged = (global_diff < convergence)
     }

   */

  #pragma omp parallel for
  for (int i = 0; i < numNodes; ++i)
  {
    solution[i] = equal_prob;
  }


  while(!converge) {
    double global_diff = 0.0;
    double no_outgoing_score = 0.0;
    memcpy(old_solution, solution, g->num_nodes * sizeof(double));

    #pragma omp parallel for reduction(+:no_outgoing_score)
    for (int j = 0; j < numNodes; j++){
	if (outgoing_size(g, j) == 0) {
		no_outgoing_score += damping * old_solution[j] / numNodes;
        }
    }
    #pragma omp parallel for reduction(+:global_diff)
    for (int i = 0; i < numNodes; i++){
	    double new_score = 0.0;
	    const Vertex* in_start = incoming_begin(g, i);
	    const Vertex* in_end = incoming_end(g, i);
	    for (const Vertex* v = in_start; v != in_end; v++){
		    new_score += old_solution[*v] / (double)outgoing_size(g,*v);
	    }

	    new_score = (damping * new_score) + (1.0 - damping)/numNodes;
	    new_score += no_outgoing_score;
	    solution[i] = new_score;
	    global_diff += fabs(new_score - old_solution[i]);
    }
    converge = global_diff < convergence;
    printf("%lf \n", global_diff);
  }

  delete old_solution;

/*
  #pragma omp parallel for reduction (+:global_diff, sum_no_out)
    for (int i = 0 ; i < numNodes ; ++i) {
  	    global_diff += abs(new_score[i] - solution[i]);
	    solution[i] = new_score[i];
	    if (outgoing_size(g,i) == 0) {
		   sum_no_out += solution[i];
	    }
            converge = (global_diff < converge);
    }
  }*/
}
