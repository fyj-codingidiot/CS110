#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <algorithm>
#include <sys/time.h>
#include <time.h>
#include <immintrin.h>
//inplement dymanic

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PI 3.14159
#define SQRT_2PI 2.506627216

typedef struct FVec
{
    unsigned int length;
    unsigned int min_length;
    unsigned int min_deta;
    float* data;
    float* sum;
    float* data3;
    float* sum_data;
} FVec;

typedef struct Image
{
    unsigned int dimX, dimY, numChannels;
    float* data;
} Image;

void normalize_FVec(FVec v)
{
    // float sum = 0.0;
    unsigned int i,j,k;
    int ext = v.length / 2;
    v.sum[0] = v.data[ext];
    for (i = ext+1,j=1; i < v.length; i++,j++)
    {
        v.sum[j] = v.sum[j-1] + v.data[i]*2;
    }
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i <= ext; i++) {
        v.sum[i] = 1/v.sum[i];
    }
    // for (i = 0; i <= ext; i++)
    // {
    //      v.data[i] /= v.sum[v.length - ext - 1 ] ;
    //      printf("%lf ",v.sum[i]);
    // }

}


float gd(float a, float b, float x)
{
    float c = (x-b) / a;
    return exp((-.5) * c * c) / (a * SQRT_2PI);
}

FVec make_gv(float a, float x0, float x1, unsigned int length, unsigned int min_length)
{
    FVec v;
    v.length = length;
    v.min_length = min_length;
    if(v.min_length > v.length){
        v.min_deta = 0;
    }else{
        v.min_deta = ((v.length - v.min_length) / 2);
    }
    v.data = malloc(length * sizeof(float));
    v.sum_data = malloc((length+1) * sizeof(float));
    v.sum = malloc((length / 2 + 1)* sizeof(float));
    v.data3 = _mm_malloc(length * 3 * sizeof(float),32);
    float step = (x1 - x0) / ((float)length);
    int offset = length/2;
    v.sum_data[0] = 0;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < length; i++)
    {
        v.data[i] = gd(a, 0.0f, (i-offset)*step);
        v.data3[3*i] = v.data[i];
        v.data3[3*i+1] = v.data[i];
        v.data3[3*i+2] = v.data[i];
    }
    for (int i = 1; i <= length; i++) {
        v.sum_data[i] = v.sum_data[i - 1] + v.data[i - 1];
    }
    normalize_FVec(v);
    return v;
}


Image img_sc(Image a)
{
    Image b = a;
    b.data = malloc(b.dimX * b.dimY * 3 * sizeof(float));
    return b;
}

Image gb_h(Image a, FVec gv)
{
    Image b = img_sc(a);

    int ext = gv.length / 2;
    // int offset;
    unsigned int x, y;
    float *pc;
    float s0[8] __attribute__((aligned(32)));
    float s1[8] __attribute__((aligned(32)));
    float s2[8] __attribute__((aligned(32)));
    float sum_array[3] __attribute__((aligned(16)));
    int i;
    #pragma omp parallel for schedule(dynamic) private(x,y,pc,i,s0,s1,s2,sum_array)
    for (y = 0; y < a.dimY; ++y)
    {

        for (x = 0; x < a.dimX; ++x)
        {
            pc = b.data + 3 * (y * b.dimX + x);
            // pc = b.data + 3 * (x * b.dimY + y);
            unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
            unsigned int l_boundary,r_boundary;
            deta = fmin(deta, gv.min_deta);
            __m256 pix_a0, pix_a1, pix_a2, sum0, sum1, sum2;
            __m256 gv_data0, gv_data1, gv_data2;
            __m128 gv_data, pixel;
            __m128 sum;
            sum0 = _mm256_setzero_ps();
            sum1 = _mm256_setzero_ps();
            sum2 = _mm256_setzero_ps();
            sum = _mm_setzero_ps();
            l_boundary = deta;
            r_boundary = gv.length - deta;
            if (x + l_boundary < ext) {
                // printf("l:%f\n",gv.sum_data[ext - x] - gv.sum_data[l_boundary]);
                gv_data = _mm_set_ps1(gv.sum_data[ext - x] - gv.sum_data[l_boundary]);
                pixel  = _mm_loadu_ps(a.data + 3 * y * a.dimX);
                sum = _mm_add_ps(sum,_mm_mul_ps(gv_data,pixel));
                l_boundary = ext - x;
            }
            if (x + r_boundary > a.dimX + ext) {
                // printf("r:%f\n",gv.sum_data[r_boundary] - gv.sum_data[a.dimX + ext - x]);
                gv_data = _mm_set_ps1(gv.sum_data[r_boundary] - gv.sum_data[a.dimX + ext - x]);
                pixel = _mm_loadu_ps(a.data + 3 * (y * a.dimX + a.dimX - 1));
                sum = _mm_add_ps(sum,_mm_mul_ps(gv_data,pixel));
                r_boundary = a.dimX + ext - x;
            }
            for (i = l_boundary; i <= r_boundary-8; i+=8)
            {
                // offset = i - ext;
                // div_add = gv.div3 + 3 * ((ext - deta) * gv.length + i);
                // img_add = ex_img + 3 * (y * (a.dimX + 2 * ext) + x + i);
                pix_a0 = _mm256_loadu_ps(a.data + 3 * (y * a.dimX  + x + i - ext));
                pix_a1 = _mm256_loadu_ps(a.data + 3 * (y * a.dimX + x + i - ext) + 8);
                pix_a2 = _mm256_loadu_ps(a.data + 3 * (y * a.dimX + x + i - ext) + 16);
                gv_data0 = _mm256_loadu_ps(gv.data3 + 3 * i);
                gv_data1 = _mm256_loadu_ps(gv.data3 + 3 * i + 8);
                gv_data2 = _mm256_loadu_ps(gv.data3 + 3 * i + 16);
                sum0 = _mm256_add_ps(sum0,_mm256_mul_ps(gv_data0,pix_a0));
                sum1 = _mm256_add_ps(sum1,_mm256_mul_ps(gv_data1,pix_a1));
                sum2 = _mm256_add_ps(sum2,_mm256_mul_ps(gv_data2,pix_a2));
            }
            _mm256_store_ps(s0, sum0);
            _mm256_store_ps(s1, sum1);
            _mm256_store_ps(s2, sum2);
            pc[0] = pc[0] + s0[0] + s0[3] + s0[6] + s1[1] + s1[4] + s1[7] + s2[2] + s2[5];
            pc[1] = pc[1] + s0[1] + s0[4] + s0[7] + s1[2] + s1[5] + s2[0] + s2[3] + s2[6];
            pc[2] = pc[2] + s0[2] + s0[5] + s1[0] + s1[3] + s1[6] + s2[1] + s2[4] + s2[7];
            for (i; i < r_boundary; ++i) {
                // offset = i - ext;
                gv_data = _mm_set_ps1(gv.data[i]);
                pixel  = _mm_loadu_ps(a.data + 3 * ( y * a.dimX + x + i - ext));
                sum = _mm_add_ps(sum,_mm_mul_ps(gv_data,pixel));
            }
            _mm_store_ps(sum_array,sum);
            pc[0] += sum_array[0];
            pc[1] += sum_array[1];
            pc[2] += sum_array[2];
            pc[0] *= gv.sum[ext-deta];
            pc[1] *= gv.sum[ext-deta];
            pc[2] *= gv.sum[ext-deta];
        }
    }
    return b;
}

Image transpose(Image src) {
    Image dst = img_sc(src);
    # pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < src.dimX; ++x) {
        for (int y = 0; y < src.dimY; ++y) {
            memcpy(dst.data + 3 * (x * src.dimY + y),src.data +  3 * (y * src.dimX + x),3 * sizeof(float));
        }
    }
    dst.dimX = src.dimY;
    dst.dimY = src.dimX;
    return dst;
}

Image apply_gb(Image a, FVec gv)
{
    Image b = gb_h(a, gv);
    Image c = transpose(b);
    Image d = gb_h(c, gv);
    Image e = transpose(d);
    free(b.data);
    free(c.data);
    free(d.data);
    return e;
}

int main(int argc, char** argv)
{
    struct timeval start_time, stop_time, elapsed_time, mid_time, temp_time; 
    gettimeofday(&start_time,NULL);
    if (argc < 6)
    {
        printf("Usage: ./gb.exe <inputjpg> <outputname> <float: a> <float: x0> <float: x1> <unsigned int: dim>\n");
        exit(0);
    }

    float a, x0, x1;
    unsigned int dim, min_dim;

    sscanf(argv[3], "%f", &a);
    sscanf(argv[4], "%f", &x0);
    sscanf(argv[5], "%f", &x1);
    sscanf(argv[6], "%u", &dim);
    sscanf(argv[7], "%u", &min_dim);

    FVec v = make_gv(a, x0, x1, dim, min_dim);
    // print_fvec(v);
    Image img;
    img.data = stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);

    Image imgOut = apply_gb(img, v);
    stbi_write_jpg(argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels, imgOut.data, 90);
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    free(imgOut.data);
    free(v.data);
    free(v.sum);
    return 0;
}
