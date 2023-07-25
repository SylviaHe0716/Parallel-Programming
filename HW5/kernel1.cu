#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

__global__ void mandelKernel(int* d_out, int width, float stepX, float stepY, float lowerX, float lowerY, int count) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;

    int thisX = blockIdx.x * blockDim.x + threadIdx.x;
    int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    float c_re = lowerX + thisX * stepX;
    float c_im = lowerY + thisY * stepY;
    float z_re = c_re;
    float z_im = c_im;

    int iter;
    for (iter = 0; iter < count; ++iter){
	if (z_re * z_re + z_im * z_im > 4.f) {
        break;
    }

	float new_re = z_re * z_re - z_im * z_im;
	float new_im = 2.f * z_re * z_im;
	z_re = c_re + new_re;
	z_im = c_im + new_im;
    }

    int idx = thisX + thisY * width;
    d_out[idx] = iter;
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    //int N = resX * resY;
    int size = resX * resY * sizeof(int);

    // allocate the host memory
    int *data;
    data = (int*) malloc(size);
    // allocate GPU memory
    int *d_out;
    cudaMalloc(&d_out, size);

    // CUDA function
    dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 numBlocks(resX / threadsPerBlock.x, resY / threadsPerBlock.y);
    mandelKernel<<<numBlocks, threadsPerBlock>>>(d_out, resX, stepX, stepY, lowerX, lowerY, maxIterations);

    cudaMemcpy(data, d_out, size, cudaMemcpyDeviceToHost);
    memcpy(img, data, size);

    // Free allocated memory
    cudaFree(d_out);
    free(data);
}