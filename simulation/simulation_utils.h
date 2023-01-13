#ifndef SIM_UTILS
#define SIM_UTILS

#include "def.h"

char* file_id_ext(char* name, int id) {
    /*
        Génère un nom de fichier de la forme : {name}{id}.tipe
    */
    char nb[8];
    sprintf(nb,"%d",id);
    char* res = (char*)malloc(sizeof(char) * (strlen(name) + 1 + 9 + 6));
    return strcat(strcat(strcpy(res,name),nb),".tipe");
}

void init_surface_temp(s_t_matrix* tab, int rows, int cols, char* config_surface_temp_filename, char* config_surface_h_filename, double mu, double lambda) {
    tab->cols = cols;
    tab->rows = rows;
    
    surface_temp* surfaces = (surface_temp*)malloc(rows * cols * sizeof(surface_temp));
    if (surfaces == NULL) { exit(EXIT_FAILURE); }

    tab->data = surfaces;

    FILE* config_surface_temp = fopen(config_surface_temp_filename, "r");
    FILE* config_surface_h = fopen(config_surface_h_filename, "r");
    float temp;
    float h;

     for (int j = 0; j < tab->rows; j++) {        // On suppose que murs et sol ont la même dimension
        for (int k = 0; k < tab->cols; k++) {
            surface new_surf = { .width = mu, .length = lambda};

            fscanf(config_surface_temp, "%f", &temp);
            tab->data[idx(j, k, tab->cols)].temp = temp;
            fscanf(config_surface_h, "%f", &h);
            tab->data[idx(j, k, tab->cols)].h = h; // 1/h_i + e/lambda (1/hi dépend e = épaisseur surface en (m) et lambda = conductivité thermique (ici celle du béton)); cf https://fr.wikipedia.org/wiki/Coefficient_de_convection_thermique
            tab->data[idx(j, k, tab->cols)].surf = new_surf;
        }
    }
    fclose(config_surface_temp);
}       

void init_f_mat_val(f_matrix* mat, int n, int cols, int rows, float value) {
    for (int i = 0; i < n; i++) {
        mat[i].cols = cols;
        mat[i].rows = rows;

        double* temp = (double*)malloc(cols * rows * sizeof(double));
        if (temp == NULL) {
            exit(EXIT_FAILURE);
        }
        mat[i].data = temp;

        for (int j = 0; j < mat[i].rows; j++) {
            for (int k = 0; k < mat[i].cols; k++) {
                mat[i].data[idx(j, k, mat[i].cols)] = value;
            }
        }
    }
}

void write_f_mat(FILE* file, f_matrix* mat, float modifier, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < mat[i].rows; j++) {
            for (int k = 0; k < mat[i].cols - 1; k++) {
                fprintf(file, "%.6f;", mat[i].data[idx(j, k, mat[i].cols)] + modifier);
            }
            
            fprintf(file, "%.6f\n", mat[i].data[idx(j, mat[i].cols - 1, mat[i].cols)] + modifier);
        }
    }
}

void copy_f_mat(f_matrix* dest, f_matrix* src, int n) {
    assert(dest[0].cols == src[0].cols && dest[0].rows == src[0].rows);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < dest[i].rows; j++) {
            for (int k = 0; k < dest[i].cols; k++) {
                dest[i].data[idx(j, k, dest[i].cols)] = src[i].data[idx(j, k, src[i].cols)];
            }
        }
    }
}

bool is_colliding(int x, int y, int z, bool consider_x, bool consider_y, bool consider_z, bounding_box bb) {
    // x, y, z = -1 => ignorer la collision selon cette coordoonnée
    return  ( ( (x < bb.start_x + bb.width  && x > bb.start_x) || !(consider_x) ) &&
              ( (y < bb.start_y + bb.length && y > bb.start_y) || !(consider_y) ) &&
              ( (z < bb.start_z + bb.height && z > bb.start_z) || !(consider_z) )
            );
}

#endif