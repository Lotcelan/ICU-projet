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
double calculer_h(double delta_t, double L) {
    return 1.32 * sqrt(sqrt(abs(delta_t/L)));
}
*/

double* simulation(double T_e, double fluid_speed, double fluid_volume, double L, double l,
    int n, double c_p, double D, char* config_l_wall_temp, char*  config_floor_temp, char*  config_r_wall_temp, char*  config_l_wall_h, char*  config_floor_h, char*  config_r_wall_h,
    bool continuer_meme_si_fini, int nb_iterations_supplementaires, char* save_air_temp_filename,
    char* air_temp_last_first_file, char* masses_last_first_file, bool flask, bool print_to_file, int id)
{
    /*
        Effectue la simulation (arguments détaills dans main.c)
    */

    f_matrix* air_temp = (f_matrix*)malloc(n * sizeof(f_matrix)); // Contiendra en i_e case la matrice de températures de la i_e subdivision selon la largeur
    f_matrix* masses = (f_matrix*)malloc(n * sizeof(f_matrix)); // Contiendra en i_e case la matrice de masses de la i_e subdivision selon la largeur

    s_t_matrix* floor_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du sol
    s_t_matrix* left_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur gauche
    s_t_matrix* right_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur droit

    const double lambda = L / n; // Longueur infinitésimale
    const double mu = l / n; // Largeur infinitésimale
    const double height_tot = fluid_volume / (l * L); // On peut alors trouver la hauteur que va occuper le fluide
    const double h_n = height_tot / n; // Soit la hauteur infinitésimale

    const double tau = lambda / (fluid_speed / 3.6); // temps (en s) de simulation à tour de simulation

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

    if (floor == NULL || l_wall == NULL || r_wall == NULL) {
        exit(EXIT_FAILURE);
    }
    
    floor_temp->data = floor;
    left_wall_temp->data = l_wall;
    right_wall_temp->data = r_wall;

    FILE* l_wall_temp_file = fopen(config_l_wall_temp, "r");
    FILE* floor_temp_file = fopen(config_floor_temp, "r");
    FILE* r_wall_temp_file = fopen(config_r_wall_temp, "r");

    FILE* l_wall_h_file = fopen(config_l_wall_h, "r");
    FILE* floor_h_file = fopen(config_floor_h, "r");
    FILE* r_wall_h_file = fopen(config_r_wall_h, "r");

    float temp;
    float h;
    for (int j = 0; j < floor_temp->rows; j++) {        // On suppose que murs et sol ont la même dimension
        for (int k = 0; k < floor_temp->cols; k++) {
            surface new = { .width = mu, .length = lambda};

            fscanf(floor_temp_file, "%f", &temp);
            floor_temp->data[idx(j, k, floor_temp->cols)].temp = temp;
            fscanf(floor_h_file, "%f", &h);
            floor_temp->data[idx(j, k, floor_temp->cols)].h = h; // 1/h_i + e/lambda (1/hi dépend e = épaisseur surface en (m) et lambda = conductivité thermique (ici celle du béton)); cf https://fr.wikipedia.org/wiki/Coefficient_de_convection_thermique
            floor_temp->data[idx(j, k, floor_temp->cols)].surf = new;

            fscanf(l_wall_temp_file, "%f", &temp);
            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].temp = temp;
            fscanf(l_wall_h_file, "%f", &h);
            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].h = h;
            left_wall_temp->data[idx(j, k, left_wall_temp->cols)].surf = new;

            fscanf(r_wall_temp_file, "%f", &temp);
            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].temp = temp;
            fscanf(r_wall_h_file, "%f", &h);
            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].h = h;
            right_wall_temp->data[idx(j, k, right_wall_temp->cols)].surf = new;
        } 
    }

    fclose(l_wall_temp_file);
    fclose(floor_temp_file);
    fclose(r_wall_temp_file);
    fclose(l_wall_h_file);
    fclose(floor_h_file);
    fclose(r_wall_h_file);

    // TEMPERATURES DE L'AIR

    for (int i = 0; i < n; i++) {
        air_temp[i].cols = n;
        air_temp[i].rows = n;

        double* temp = (double*)malloc(n * n * sizeof(double));
        if (temp == NULL) {
            exit(EXIT_FAILURE);
        }
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

        double* temp = (double*)malloc(n * n * sizeof(double));
        if (temp == NULL) {
            exit(EXIT_FAILURE);
        }
        masses[i].data = temp;

        for (int j = 0; j < masses[i].rows; j++) {
            for (int k = 0; k < masses[i].cols; k++) {
                // Formule qui provient de l'équation des gaz parfaits
                masses[i].data[idx(j, k, masses[i].cols)] = 101325.0 * floor_temp->data[idx(j,k,floor_temp->cols)].surf.length * floor_temp->data[idx(j,k,floor_temp->cols)].surf.width * h_n * 0.0029 / (8.314 * air_temp[i].data[idx(i,j,air_temp[i].cols)]);
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
        fprintf(air_temp_last_first,"%i*%i*%i*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%i\n", n, n, n, T_e, fluid_speed, fluid_volume, L, l, c_p, D, id); // nb_sub / rows / cols / vitesse / volume / Longueur de la surface / largeur / c_p / D / l'id de la simulation| pour l'instant seul ce fichier contiendra toutes les informations de la simulation pour éviter la redodnance

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

    double min_temp = T_e;
    double max_temp = T_e;

    int count = 0;

    // Pour avoir l'itération précédente

    f_matrix* last_air_temp = (f_matrix*)malloc(n*sizeof(f_matrix));
    for (int i = 0; i < n; i++) {
        last_air_temp[i].cols = air_temp->cols;
        last_air_temp[i].rows = air_temp->rows;
        double* temp = (double*)malloc(n * n * sizeof(double));
        if (temp == NULL) {
            exit(EXIT_FAILURE);
        }
        last_air_temp[i].data = temp;
        for (int j = 0; j < last_air_temp[i].rows; j++) {
            for (int k = 0; k < last_air_temp[i].cols; k++) {
                last_air_temp[i].data[idx(j, k, last_air_temp[i].cols)] = air_temp[i].data[idx(j, k, last_air_temp[i].cols)];
            }
        }
    }

    float iteration = 1;
    float nb_total_iteration = (n - 1) * 2;
    float nb_decoupage = 20;
    printf("\n");
    while (idx_c.snd != 0 || (continuer_meme_si_fini && count < nb_iterations_supplementaires)) {
        if ((int)iteration % (100 / (int)nb_decoupage) == 0) {
            printf("\x1b[A\r%.6f%%          \n", iteration * 100 / nb_total_iteration);
        }
        if (continuer_meme_si_fini && idx_c.snd <= 0) {
            count++;
        }
        
        for (int i = 0; i < n; i++ ) {
            int c = 0;
            for (int j = idx_c.fst; j <= idx_c.snd; j++) {

                // LA CONDUCTION
                
                double new_T_floor = floor_temp_calc(i, j, lambda, mu, tau, floor_temp->data[idx(i, n - 1 - idx_c.snd + c, floor_temp->cols)], last_air_temp, c_p, masses[i].data[idx(air_temp[i].rows - 1, j, masses[i].cols)], fluid_speed);
                air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] = new_T_floor;

                double new_T_l_wall = wall_temp_calc(i, j, 0, lambda, mu, tau, left_wall_temp->data[idx(i, n - 1 - idx_c.snd + c, left_wall_temp->cols)], last_air_temp, c_p, masses[0].data[idx(i, j, masses[0].cols)], fluid_speed);
                double new_T_r_wall = wall_temp_calc(i, j, n - 1, lambda, mu, tau, right_wall_temp->data[idx(i, n - 1 - idx_c.snd + c, right_wall_temp->cols)], last_air_temp, c_p, masses[n-1].data[idx(i, j, masses[n-1].cols)], fluid_speed);

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

            for (int j = 1; j < n - 1; j++) {
                for (int m = 1; m < air_temp[i].rows - 1; m++) {
                    if (i != 0 && i != n - 1) {
                        double new_T_air = air_temp_calc(i, j, m, lambda, mu, h_n, n, tau, last_air_temp, D);
                        air_temp[i].data[idx(m, j, air_temp[i].cols)] = new_T_air;

                        if (new_T_air < min_temp) { min_temp = new_T_air; }
                        if (new_T_air > max_temp) { max_temp = new_T_air; }
                    }
                }
            }
        }
        iteration++;

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
            for (int j = 0; j < last_air_temp[i].rows; j++) {
                for (int k = 0; k < last_air_temp[i].cols; k++) {
                    last_air_temp[i].data[idx(j, k, last_air_temp[i].cols)] = air_temp[i].data[idx(j, k, last_air_temp[i].cols)];
                }
            }
        }
    }

    printf("100%%\n");

    printf("Min_temp = %.6f; Max_temp = %.6f\n", min_temp-273.0, max_temp-273.0);

    if (print_to_file) {
        // A la toute fin du fichier contenant toute la simulation on ajoute la température min et max
        fprintf(f,"%.6f;%.6f\n", min_temp-273.0, max_temp-273.0);
        fclose(f);

        
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

    double* res = malloc(sizeof(double)*2);
    res[0] = min_temp - 273.0;
    res[1] = max_temp - 273.0;

    if (flask) {
        char* res = (char*)malloc(sizeof(char)*2096);
        sprintf(res, "curl -X POST -d 'T_e=%.6f&Vit_air=%.6f&Vol_air=%.6f&L=%.6f&l=%.6f&n=%i&c_p=%.6f&D=%.6f&continuer_meme_si_fini=%i&nb_it_supp=%i&min_temp=%.6f&max_temp=%.6f&id=%i' http://127.0.0.1:5000/", T_e, fluid_speed, fluid_volume, L, l, n, c_p, D, (int)continuer_meme_si_fini, nb_iterations_supplementaires, min_temp -273.0 , max_temp - 273.0, id);
        system(res);
    }

    return res;

}

#endif