#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x, result;
  __pp_vec_int exp, exp_counter;
  __pp_vec_float Maxresult =  _pp_vset_float(9.999999f);
  __pp_vec_float fl_zero = _pp_vset_float(0.f);
  __pp_vec_float fl_one = _pp_vset_float(1.f);
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one = _pp_vset_int(1);
  __pp_mask maskAll, maskElse, maskExp, maskExpZero, maskMaxresult;


  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
	  // for remaining elements when N % VECTPR_WIDTH != 0
	  if ( i + VECTOR_WIDTH > N)
	  { 
	    maskAll = _pp_init_ones(N-i);
	  }
	  else
	  {
	    // for all elements
	    maskAll = _pp_init_ones();
	  }

	  maskExp = _pp_init_ones(0);
	  maskExpZero = _pp_init_ones(0);
	  maskMaxresult = _pp_init_ones(0);

	  // load values and exponents
	  _pp_vload_float(x, values + i, maskAll);
	  _pp_vload_int(exp, exponents + i, maskAll);
	  
	  // if exp = 0
	  _pp_veq_int(maskExpZero, exp, zero, maskAll);
	  // result = 1
	  _pp_vset_float(result, 1.0f, maskAll);

	  // else
	  maskElse = _pp_mask_not(maskExpZero);
	  // result = x
	  _pp_vload_float(result, values + i, maskElse);
	  // exp_counter = exp-1
	  _pp_vsub_int(exp_counter, exp, one, maskElse);
	  // maskExp = exp_counter > 0
	  _pp_vgt_int(maskExp, exp_counter, zero, maskElse);
	  
	  while (_pp_cntbits(maskExp) != 0)
	  {
	    // result *= x
	    _pp_vmult_float(result, result, x, maskExp);
	    // exp_counter--
	    _pp_vsub_int(exp_counter, exp_counter, one, maskExp);
	    //maskExp = exp_counter > 0
	    _pp_vgt_int(maskExp, exp_counter, zero, maskExp);
	  }

	  // if result > 9.999999
	  _pp_vgt_float(maskMaxresult, result, Maxresult, maskAll); 
	  // result = 9.999999
	  _pp_vset_float(result,9.999999f,maskMaxresult);
	  
	  // output = result
	  _pp_vstore_float(output +i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  __pp_vec_float x, result;
  __pp_mask maskAll;

  float vect[VECTOR_WIDTH];
  int counter = VECTOR_WIDTH;
  maskAll = _pp_init_ones();
  
  // init result to 0
  result = _pp_vset_float(0.0f);

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // load x
    _pp_vload_float(x, values + i, maskAll);
    // add x
    _pp_vadd_float(result, result, x, maskAll);
  }
  
  // for log(VECTOR_WIDTH) times
  while(counter != 1) 
  {
    // Adds up adjacent pairs of elements
    _pp_hadd_float(result, result);
    // Performs an even-odd interleaving where all even-indexed elements move to front half of the array and odd-indexed to the back half   
    _pp_interleave_float(result, result);
    // counter = VECTOR_WIDTH = k * power of 2
    // counter ^ (1/2)
    counter >>= 1;
  }
  _pp_vstore_float(vect, result, maskAll);
  return vect[0];
}  

