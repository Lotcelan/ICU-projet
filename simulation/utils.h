#ifndef UTILS
#define UTILS

#include <stdlib.h>
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>


typedef struct f_matrix
{
    int cols; // Ce sera normalement toujours n
    int rows; // Ce sera normalement toujours n
    float* data; //1 dim float array
} f_matrix;

typedef struct surface
{
    float width;
    float length;

} surface;

typedef struct surface_temp
{
    surface surf;
    float temp;
    float h;
} surface_temp;

typedef struct s_t_matrix
{
    int rows; // Ce sera normalement toujours n
    int cols; // Ce sera normalement toujours n
    surface_temp* data; //1 dim surface_temp array
} s_t_matrix;

typedef struct idx_couple
{
    int fst;
    int snd;
} idx_couple;

int idx(int i, int j, int size) { return i*size + j; }


#endif