__kernel void convolution(int filterWidth, __constant float * filter,
			  int imageHeight, int imageWidth,
			  __global float * inputImage, __global float * outputImage) 
{
    int halffilterSize = filterWidth / 2;
    float sum = 0.0;
    int k, l;
    int xx = get_global_id(0);
    int yy = get_global_id(1);

        for (k = -halffilterSize; k <= halffilterSize; k++){
	for (l = -halffilterSize; l <= halffilterSize; l++){
	    if (yy + k >= 0 && yy + k < imageHeight &&
		xx + l >= 0 && xx + l < imageWidth){
                sum += inputImage[(yy + k) * imageWidth + xx + l] *
                       filter[(k + halffilterSize) * filterWidth +
                              l + halffilterSize];
	    }
	}
    }
    outputImage[yy * imageWidth + xx] = sum;
   
}
