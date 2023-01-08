#ifndef DEF
#define DEF

#include <stdlib.h>
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


typedef struct f_matrix
{
    int cols; // Ce sera normalement toujours n
    int rows; // Ce sera normalement toujours n
    double* data; //1 dim double array
} f_matrix;

typedef struct surface
{
    double width;
    double length;

} surface;

typedef struct surface_temp
{
    surface surf;
    double temp;
    double h;
} surface_temp;

typedef struct s_t_matrix
{
    int rows; // Ce sera normalement toujours n
    int cols; // Ce sera normalement toujours n
    surface_temp* data; //1 dim surface_temp array
} s_t_matrix;

typedef struct bounding_box
{
    // Donne le point de départ du pavé, puis sa largeur selon x, longueur selon y et hauteur selon z
    int start_x;
    int start_y;
    int start_z;
    int width;
    int length;
    int height;
} bounding_box;

typedef struct tree
{
    bounding_box bb;
} tree;

typedef struct forest
{
    tree* tree_list;
    int size;
} forest;



typedef struct idx_couple
{
    int fst;
    int snd;
} idx_couple;

int idx(int i, int j, int size) { 
    if (j > size || i > size) { printf("Index error : i=%i; j=%i; size =%i", i, j, size); } // matrice carrée
    return i*size + j;    
}

#endif