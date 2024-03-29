#ifndef SIM_UTILS
#define SIM_UTILS

#include "def.h"

char* file_id_ext(char* name, int id) {
    // Génère un nom de fichier de la forme : {name}{id}.tipe
    
    char nb[8];
    sprintf(nb,"%d",id);
    char* res = (char*)malloc(sizeof(char) * (strlen(name) + 1 + 9 + 6));
    return strcat(strcat(strcpy(res,name),nb),".tipe");
}

void init_surface_temp(s_t_matrix* tab, int rows, int cols, char* config_surface_temp_filename, char* config_surface_h_filename, double mu, double lambda, double albedo_asphalte, double albedo_beton, double albedo_brique, double albedo_ciment) {
    // Permet d'initialiser les éléments de surface des murs et du sol
    
    tab->cols = cols;
    tab->rows = rows;
    
    surface_temp* surfaces = (surface_temp*)malloc(rows * cols * sizeof(surface_temp));
    if (surfaces == NULL) { exit(EXIT_FAILURE); }

    tab->data = surfaces;

    FILE* config_surface_temp = fopen(config_surface_temp_filename, "r");
    FILE* config_surface_h = fopen(config_surface_h_filename, "r");

    assert(config_surface_temp != NULL);
    assert(config_surface_h != NULL);

    double temp;
    double h;

     for (int j = 0; j < tab->rows; j++) {        // On suppose que murs et sol ont la même dimension
        for (int k = 0; k < tab->cols; k++) {
            surface new_surf = { .width = mu, .length = lambda, .height = -1, .masse_vol = -1, .capacite_thermique = -1 };

            // Lecture dans les fichiers de configuration

            fscanf(config_surface_temp, "%lf", &temp);
            tab->data[idx(j, k, tab->cols)].temp = temp;
            fscanf(config_surface_h, "%lf", &h);
            tab->data[idx(j, k, tab->cols)].h = h;
            tab->data[idx(j, k, tab->cols)].surf = new_surf;

            double chosen_albedo = 0;

            if (h - 2.16 <= 0.001 && h - 2.16 >= -0.001) {
                // Béton (sol)
                chosen_albedo = albedo_beton;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.425;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2000;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 880;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1.75;
            }
            else if (h - 2.195 <= 0.001 && h - 2.195 >= -0.001) {
                // Béton (murs)

                chosen_albedo = albedo_beton;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.5;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2000;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 880;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1.75;
            }
            else if (h - 2.544 <= 0.001 && h - 2.544 >= -0.001) {
                // Asphalte
                
                chosen_albedo = albedo_asphalte;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.3;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2400;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 1021;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1;
            }
            else if (h - 4.356 <= 0.001 && h - 4.356 >= -0.001) {
                // Non utilisé au final, mais brique
                
                chosen_albedo = albedo_brique;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.02;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 1750;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 840;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 0.84;

            } else if (h - 4.023 <= 0.001 && h - 4.023 >= - 0.001) {
                // Ciment

                chosen_albedo = albedo_ciment;

                tab->data[idx(j, k, tab->cols)].surf.height = 0.016;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 1900;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 864;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 0.8;
                
            }
            
            // Matériaux custom pour faciliter étude de chaque paramètre

            else if (-h - 1 <= 0.001 && -h - 1 >= - 0.001) {
                chosen_albedo = 0.5;
                tab->data[idx(j, k, tab->cols)].h = 2;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.5;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2000;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 1000;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1;
            } else if (-h - 2 <= 0.001 && -h - 2 >= - 0.001) {
                chosen_albedo = 1;
                tab->data[idx(j, k, tab->cols)].h = 3;
                tab->data[idx(j, k, tab->cols)].surf.height = 0.5;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2000;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 1000;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1;
            } else if (-h - 3 <= 0.001 && -h - 3 >= - 0.001) {
                chosen_albedo = 1;
                tab->data[idx(j, k, tab->cols)].h = 3; 
                tab->data[idx(j, k, tab->cols)].surf.height = 0.5;
                tab->data[idx(j, k, tab->cols)].surf.masse_vol = 2000;
                tab->data[idx(j, k, tab->cols)].surf.capacite_thermique = 1000;
                tab->data[idx(j, k, tab->cols)].surf.conductivite_thermique = 1;
            }
            else {
                chosen_albedo = albedo_beton;
            }
            
            tab->data[idx(j, k, tab->cols)].surf.albedo = chosen_albedo;
        }
    }
    fclose(config_surface_temp);
}       

void init_cell_mat_val(cell_matrix* mat, int n, int cols, int rows, float value) {
    // Initialisation des blocs d'air (et de masse)
    
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
    // Explicite
    
    for (int y = 0; y < n; y++) {
        for (int z = 0; z < mat[y].rows; z++) {
            for (int x = 0; x < mat[y].cols - 1; x++) {
                fprintf(file, "%.6f;", mat[y].data[idx(z, x, mat[y].cols)].value + modifier);
            }
            
            fprintf(file, "%.6f\n", mat[y].data[idx(z, mat[y].cols - 1, mat[y].cols)].value + modifier);
        }
    }
}

void write_surf_temp_mat(FILE* file, s_t_matrix* mat, float modifier) {
    // Explicite
    
    for (int y = 0; y < mat->rows; y++) {
        for (int x = 0; x < mat->cols; x++) {
            fprintf(file, "%.6f;", mat->data[idx(y, x, mat->cols)].temp + modifier);
        }
        fprintf(file, "%.6f\n", mat->data[idx(y, mat->cols - 1, mat->cols)].temp + modifier);
    }
}

void copy_cell_mat(cell_matrix* dest, cell_matrix* src, int n) {
    // Explicite
    
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
    // Indique si oui ou non les coordonnées sont dans une bounding box
    // x, y, z = -1 => ignorer la collision selon cette coordoonnée
    
    return  ( ( (x < bb.start_x + bb.length && x >= bb.start_x) || !(consider_x) ) &&
              ( (y < bb.start_y + bb.width  && y >= bb.start_y) || !(consider_y) ) &&
              ( (z < bb.start_z + bb.height && z >= bb.start_z) || !(consider_z) )
            );
}

void fluid_offset(cell_matrix* c_mat, int n) {
    // Faire avancer l'air
    
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < c_mat[y].cols; x++) {
            for (int z = 0; z < c_mat[y].rows; z++) {
                c_mat[y].data[idx(z, x, c_mat[y].cols)].global_x = c_mat[y].data[idx(z, x, c_mat[y].cols)].global_x + 1;
            }
        }
    }
}

int global_x_to_street(int x, int n) {
    // Changement de repère
    
    return x - n;
}

#endif