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

void init_cell_mat_val(cell_matrix* mat, int n, int cols, int rows, float value) {
    for (int y = 0; y < n; y++) {
        mat[y].cols = cols;
        mat[y].rows = rows;

        cell* cell_temp = (cell*)malloc(cols * rows * sizeof(cell));
        if (cell_temp== NULL) {
            exit(EXIT_FAILURE);
        }
        mat[y].data = cell_temp;

        for (int z = 0; z < mat[y].rows; z++) {
            for (int x = 0; x < mat[y].cols; x++) {
                cell new_cell = { .value = value, .local_x = x, .local_y = y, .local_z = z, .global_x = x, .global_y = y, .global_z = z}; 
                mat[y].data[idx(z, x, mat[y].cols)] = new_cell;
            }
        }
    }
}

void write_cell_mat(FILE* file, cell_matrix* mat, float modifier, int n) {
    for (int y = 0; y < n; y++) {
        for (int z = 0; z < mat[y].rows; z++) {
            for (int x = 0; x < mat[y].cols - 1; x++) {
                fprintf(file, "%.6f;", mat[y].data[idx(z, x, mat[y].cols)].value + modifier);
            }
            
            fprintf(file, "%.6f\n", mat[y].data[idx(z, mat[y].cols - 1, mat[y].cols)].value + modifier);
        }
    }
}

void copy_cell_mat(cell_matrix* dest, cell_matrix* src, int n) {
    assert(dest[0].cols == src[0].cols && dest[0].rows == src[0].rows);
    for (int y = 0; y < n; y++) {
        for (int z = 0; z < dest[y].rows; z++) {
            for (int x = 0; x < dest[y].cols; x++) {
                dest[y].data[idx(z, x, dest[y].cols)] = src[y].data[idx(z, x, src[y].cols)];
            }
        }
    }
}

bool is_colliding(int x, int y, int z, bool consider_x, bool consider_y, bool consider_z, bounding_box bb) {
    // x, y, z = -1 => ignorer la collision selon cette coordoonnée
    return  ( ( (x < bb.start_x + bb.length  && x > bb.start_x) || !(consider_x) ) &&
              ( (y < bb.start_y + bb.width && y > bb.start_y) || !(consider_y) ) &&
              ( (z < bb.start_z + bb.height && z > bb.start_z) || !(consider_z) )
            );
}

void fluid_offset(cell_matrix* c_mat, int n) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < c_mat[y].cols; x++) {
            for (int z = 0; z < c_mat[y].rows; z++) {
                c_mat[y].data[idx(z, x, c_mat[y].cols)].global_x = c_mat[y].data[idx(z, x, c_mat[y].cols)].global_x + 1;
            }
        }
    }
}

int global_x_to_street(int x, int n) {
    return x - n;
}

#endif