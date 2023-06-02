#ifndef DEF
#define DEF

#include <stdlib.h>
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct cell {
    double value;
    // Coordonnées : origine en haut, au début de la partie gauche du fluide, x selon où va le fluide, y vers le mur droit et z vers le bas : BOUGE AVEC LE FLUIDE
    int local_x;
    int local_y;
    int local_z;
    // Coordonnées : la même MAIS NE BOUGE PAS AVEC LE FLUIDE (x peut donc aller de 0 à 3n)
    int global_x;
    int global_y;
    int global_z;
} cell;

typedef struct cell_option {
    cell some;
    bool valid;
} cell_option;

typedef struct cell_matrix
{
    int cols; // Ce sera normalement toujours n
    int rows; // Ce sera normalement toujours n
    cell* data; //1 dim double array
} cell_matrix;

typedef struct surface
{
    double width;
    double length;
    double height;
    double masse_vol; // rho
    double capacite_thermique; // c
    double conductivite_thermique; // lambda
    double albedo;


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

surface_temp get_surf(s_t_matrix* surf_mat, int x, int y) {
    return surf_mat->data[idx(x, y, surf_mat->cols)];
}

cell get_cell(cell_matrix* c_mat, int x, int y, int z) {
    return c_mat[y].data[idx(z, x, c_mat[y].cols)];
}

cell_option get_cell_from_street(cell_matrix* c_mat, int street_x, int street_y, int street_z) {
    // Pour passer du repère de la rue à une case dans l'air
    int offset = get_cell(c_mat, 0, 0, 0).global_x;
    int l_x = street_x + c_mat[0].cols - offset;
    if (l_x >= 0 && l_x < c_mat[0].cols) {
        cell_option temp = { .some = get_cell(c_mat, l_x, street_y, street_z), .valid = true };
        return temp;
    } else {
        cell_option claque_au_sol = { .some = { -1, -1, -1, -1, -1, -1, -1}, .valid = false };
        return claque_au_sol;
    }
}

void update_cell_value(cell_matrix* c_mat, int x, int y, int z, double new_value) {
    c_mat[y].data[idx(z, x, c_mat[y].cols)].value = new_value;
}

void update_min_max_temp(double temp, double* min_temp, double* max_temp) {
    if (temp < *min_temp) { *min_temp = temp; }
    if (temp > *max_temp) { *max_temp = temp; }
}

#endif