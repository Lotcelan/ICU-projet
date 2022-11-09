#ifndef MAIN
#define MAIN

#include "calc.h"


char* file_id_ext(char* name, int id) {
    /*
        Génère un nom de fichier de la forme : {name}{id}.tipe
    */
    char nb[8];
    sprintf(nb,"%d",id);
    char* res = (char*)malloc(sizeof(char) * (strlen(name) + 1 + 9 + 6));
    return strcat(strcat(strcpy(res,name),nb),".tipe");
}

/* Possibilité pour calculer h
float calculer_h(float delta_t, float L) {
    return 1.32 * sqrt(sqrt(abs(delta_t/L)));
}
*/

float* simulation(float T_e, float fluid_speed, float fluid_volume, float L, float l,
    int n, float c_p, float D, float offset_floor, float offset_l_wall, float offset_r_wall,
    bool continuer_meme_si_fini, int nb_iterations_supplementaires, char* save_air_temp_filename,
    char* air_temp_last_first_file, char* masses_last_first_file, bool flask, bool print_to_file)
{
    /*
        Effectue la simulation (arguments détaills dans main.c)
    */

    f_matrix* air_temp = (f_matrix*)malloc(n * sizeof(f_matrix)); // Contiendra en i_e case la matrice de températures de la i_e subdivision selon la largeur
    f_matrix* masses = (f_matrix*)malloc(n * sizeof(f_matrix)); // Contiendra en i_e case la matrice de masses de la i_e subdivision selon la largeur

    s_t_matrix* floor_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du sol
    s_t_matrix* left_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur gauche
    s_t_matrix* right_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur droit

    const float lambda = L / n; // Longueur infinitésimale
    const float mu = l / n; // Largeur infinitésimale
    const float height_tot = fluid_volume / (l * lambda); // On peut alors trouver la hauteur que va occuper le fluide
    const float h_n = height_tot / n; // Soit la hauteur infinitésimale

    const float tau = lambda / (fluid_speed / 3.6); // temps (en s) de simulation à tour de simulation

    printf("Lambda = %.6f; mu = %.6f; h_n = %.6f; tau = %.6f\n", lambda, mu, h_n, tau);

    if (floor_temp == NULL || air_temp == NULL || masses == NULL || left_wall_temp == NULL || right_wall_temp == NULL) {
        exit(EXIT_FAILURE);
    }

    // TEMPERATURES DU SOL ET DES MURS

    floor_temp->cols = n;
    floor_temp->rows = n;
    left_wall_temp->cols = n;
    left_wall_temp->rows = n;
    right_wall_temp->cols = n;
    right_wall_temp->rows = n;

    surface_temp* floor = (surface_temp*)malloc(n * n * sizeof(surface_temp));
    surface_temp* l_wall = (surface_temp*)malloc(n * n * sizeof(surface_temp));
    surface_temp* r_wall = (surface_temp*)malloc(n * n * sizeof(surface_temp));
    
    floor_temp->data = floor;
    left_wall_temp->data = l_wall;
    right_wall_temp->data = r_wall;

    for (int j = 0; j < floor_temp->rows; j++) {        // On suppose que murs et sol ont la même dimension
        for (int k = 0; k < floor_temp->cols; k++) {
            surface new = { .width = mu, .length = lambda};

            floor_temp->data[idx(j, k, floor_temp->cols)].temp = T_e + offset_floor;
            floor_temp->data[idx(j, k, floor_temp->cols)].h = 1/0.06 + 1/1.75; // 1/h_i + e/lambda (1/hi dépend e = épaisseur surface en (m) et lambda = conductivité thermique (ici celle du béton)); cf https://fr.wikipedia.org/wiki/Coefficient_de_convection_thermique
            floor_temp->data[idx(j, k, floor_temp->cols)].surf = new;

            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].temp = T_e + offset_l_wall;
            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].h = 1/0.06 + 1/1.75;
            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].surf = new;

            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].temp = T_e + offset_r_wall;
            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].h = 1/0.06 + 1/1.75;
            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].surf = new;

        } 
    }

    // TEMPERATURES DE L'AIR

    for (int i = 0; i < n; i++) {
        air_temp[i].cols = n;
        air_temp[i].rows = n;

        float* temp = (float*)malloc(n * n * sizeof(float));
        air_temp[i].data = temp;

        for (int j = 0; j < air_temp[i].rows; j++) {
            for (int k = 0; k < air_temp[i].cols; k++) {
                air_temp[i].data[idx(j, k, air_temp[i].cols)] = T_e;
            }
        }
    }

    // MASSES

    for (int i = 0; i < n; i++) {
        masses[i].cols = n;
        masses[i].rows = n;

        float* temp = (float*)malloc(n * n * sizeof(float));
        masses[i].data = temp;

        for (int j = 0; j < masses[i].rows; j++) {
            for (int k = 0; k < masses[i].cols; k++) {
                // Formule qui provient de l'équation des gaz parfaits
                masses[i].data[idx(j, k, masses[i].cols)] = 101325.0 * floor_temp->data[idx(j,k,floor_temp->cols)].surf.length * floor_temp->data[idx(j,k,floor_temp->cols)].surf.width * h_n * 0.0029 / (8.314*air_temp[i].data[idx(i,j,air_temp->cols)]);
            }
        }
    }


    FILE* f = fopen(save_air_temp_filename, "w");

    FILE* masses_last_first = fopen(masses_last_first_file, "w");
    FILE* air_temp_last_first = fopen(air_temp_last_first_file, "w");


    if (print_to_file) {

        fprintf(f,"%i*%i*%i\n", n, n, n); // nb mat, rows, cols

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < air_temp[i].rows; j++) {
                for (int k = 0; k < air_temp[i].cols - 1; k++) {
                    fprintf(f, "%.6f;", air_temp[i].data[idx(j, k, air_temp[i].cols)] - 273.0);
                }
                fprintf(f, "%.6f\n", air_temp[i].data[idx(j, air_temp[i].cols - 1, air_temp[i].cols)] - 273.0);

            }
        }

        fprintf(masses_last_first,"%i*%i*%i\n", n, n, n); // nb_sub / rows / cols
        fprintf(air_temp_last_first,"%i*%i*%i*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f\n", n, n, n, T_e, fluid_speed, fluid_volume, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall); // nb_sub / rows / cols / | pour l'instant seul ce fichier contiendra toutes les informations de la simulation pour éviter la redodnance

    // Inscription du premier tour de simulation dans les fichiers
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < masses[i].rows; j++) {
                for (int k = 0; k < masses[i].cols - 1; k++) {
                    fprintf(masses_last_first, "%.6f;", masses[i].data[idx(j, k, masses[i].cols)]);
                    fprintf(air_temp_last_first, "%.6f;", air_temp[i].data[idx(j, k, air_temp[i].cols)]);
                }
                fprintf(masses_last_first, "%.6f\n", masses[i].data[idx(j, masses[i].cols - 1, masses[i].cols)]);
                fprintf(air_temp_last_first, "%.6f\n", air_temp[i].data[idx(j, air_temp[i].cols - 1, air_temp[i].cols)]);
            }
        }
    }
    // LA SIMULATION

    idx_couple idx_c;
    idx_c.fst = n-1;
    idx_c.snd = n-1;

    float min_temp = T_e;
    float max_temp = T_e;

    int count = 0;

    // Pour avoir l'itération précédente

    f_matrix* last_air_temp = (f_matrix*)malloc(n*sizeof(f_matrix));
    
    for (int i = 0; i < n; i++) {
        last_air_temp[i].cols = air_temp->cols;
        last_air_temp[i].rows = air_temp->rows;
        float* temp = (float*)malloc(n * n * sizeof(float));
        last_air_temp[i].data = temp;
        for (int j = 0; j < last_air_temp[i].rows; j++) {
            for (int k = 0; k < last_air_temp[i].cols; k++) {
                last_air_temp[i].data[idx(j, k, last_air_temp[i].cols)] = air_temp[i].data[idx(j, k, last_air_temp[i].cols)];
            }
        }
    }


    while (idx_c.snd != 0 || (continuer_meme_si_fini && count < nb_iterations_supplementaires)) {

        if (continuer_meme_si_fini && idx_c.snd <= 0) {
            count++;
        }
        
        for (int i = 0; i < n; i++ ) {
            int c = 0;
            for (int j = idx_c.fst; j <= idx_c.snd; j++) {

                // LA CONDUCTION
                
                float new_T_floor = floor_temp_calc(i, j, lambda, mu, tau, floor_temp->data[idx(i, n - 1 - idx_c.snd + c, floor_temp->cols)], last_air_temp, c_p, masses[i].data[idx(air_temp[i].rows - 1, j, masses[i].cols)], fluid_speed);
                air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] = new_T_floor;

                float new_T_l_wall = wall_temp_calc(i, j, 0, lambda, mu, tau, left_wall_temp->data[idx(i, n - 1 - idx_c.snd + c, left_wall_temp->cols)], last_air_temp, c_p, masses[0].data[idx(i, j, masses[0].cols)], fluid_speed);
                float new_T_r_wall = wall_temp_calc(i, j, n - 1, lambda, mu, tau, right_wall_temp->data[idx(i, n - 1 - idx_c.snd + c, right_wall_temp->cols)], last_air_temp, c_p, masses[n-1].data[idx(i, j, masses[n-1].cols)], fluid_speed);

                air_temp[0].data[idx(i, j, air_temp[0].cols)] = new_T_l_wall;
                air_temp[n-1].data[idx(i, j, air_temp[n-1].cols)] = new_T_r_wall;

                if (new_T_floor < min_temp) { min_temp = new_T_floor; }
                if (new_T_floor > max_temp) { max_temp = new_T_floor; }
                if (new_T_l_wall < min_temp) { min_temp = new_T_l_wall; }
                if (new_T_l_wall > max_temp) { max_temp = new_T_l_wall; }
                if (new_T_r_wall < min_temp) { min_temp = new_T_r_wall; }
                if (new_T_r_wall > max_temp) { max_temp = new_T_r_wall; }

                c++;
            }

            // LA CONVECTION

            for (int j = 1; j < n-  1; j++) {
                for (int m = 1; m < air_temp[i].rows - 1; m++) { 
                    if (i != 0 && i != n - 1) {
                        float new_T_air = air_temp_calc(i, j, m, lambda, mu, h_n, n, tau, last_air_temp, D);
                        air_temp[i].data[idx(m, j, air_temp[i].cols)] = new_T_air;

                        if (new_T_air < min_temp) { min_temp = new_T_air; }
                        if (new_T_air > max_temp) { max_temp = new_T_air; }
                    }
                }

            }
        }

        // Inscription des données de température de ce tour de simulation dans le fichier

        if (print_to_file) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < air_temp[i].rows; j++) {
                    for (int k = 0; k < air_temp[i].cols - 1; k++) {
                        fprintf(f, "%.6f;", air_temp[i].data[idx(j, k, air_temp[i].cols)] - 273.0);
                    }
                    fprintf(f, "%.6f\n", air_temp[i].data[idx(j, air_temp[i].cols - 1, air_temp[i].cols)] - 273.0);

                }
            }
        }
        // Gestion des indices pour faire comme si le fluide se déplaçait de gauche à droite sur la surface

        idx_c.fst = idx_c.fst - 1;    

        if (idx_c.fst <= 0) {
            idx_c.fst = 0;
            idx_c.snd = idx_c.snd - 1;
            if (idx_c.snd <= 0) { idx_c.snd= 0; }
        }
        
        // COPIE POUR AVOIR ITERATION PRECEDENTE
        
        for (int i = 0; i < n; i++) {

            last_air_temp[i].cols = air_temp[i].cols;
            last_air_temp[i].rows = air_temp[i].rows;
            
            for (int j = 0; j < last_air_temp[i].rows; j++) {
                for (int k = 0; k < last_air_temp[i].cols; k++) {
                    last_air_temp[i].data[idx(j, k, last_air_temp[i].cols)] = air_temp[i].data[idx(j, k, last_air_temp[i].cols)];
                }
            }
        }
    }

    if (print_to_file) {
        // A la toute fin du fichier contenant toute la simulation on ajoute la température min et max
        fprintf(f,"%.6f;%.6f\n", min_temp-273.0, max_temp-273.0);
        fclose(f);

        printf("Min_temp = %.6f; Max_temp = %.6f\n", min_temp-273.0, max_temp-273.0);
        
        // Inscription du dernier tour de simulation dans les fichiers dédiés

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < masses[i].rows; j++) {
                for (int k = 0; k < masses[i].cols - 1; k++) {
                    fprintf(masses_last_first, "%.6f;", masses[i].data[idx(j,k,masses[i].cols)]);
                    fprintf(air_temp_last_first, "%.6f;", air_temp[i].data[idx(j,k,air_temp[i].cols)]);
                }
                fprintf(masses_last_first, "%.6f\n", masses[i].data[idx(j,masses[i].cols-1,masses[i].cols)]);
                fprintf(air_temp_last_first, "%.6f\n", air_temp[i].data[idx(j,air_temp[i].cols-1,air_temp[i].cols)]);
            }
        }
        
    }

    fclose(masses_last_first);
    fclose(air_temp_last_first);

    float* res = malloc(sizeof(float)*2);
    res[0] = min_temp - 273.0;
    res[1] = max_temp - 273.0;

    if (flask) {
        char* res = (char*)malloc(sizeof(char)*2096);
        sprintf(res, "curl -X POST -d 'T_e=%.6f&Vit_air=%.6f&Vol_air=%.6f&L=%.6f&l=%.6f&n=%i&c_p=%.6f&D=%.6f&offset_floor=%.6f&offset_l_wall=%.6f&offset_r_wall=%.6f&continuer_meme_si_fini=%i&nb_it_supp=%i&min_temp=%.6f&max_temp=%.6f' http://127.0.0.1:5000/", T_e, fluid_speed, fluid_volume, L, l, n, c_p, D, offset_floor, offset_l_wall, offset_r_wall, (int)continuer_meme_si_fini, nb_iterations_supplementaires, min_temp -273.0 , max_temp - 273.0);
        system(res);
    }

    return res;

}

#endif