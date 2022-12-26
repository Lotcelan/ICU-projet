#ifndef MAIN
#define MAIN

#include "calc.h"
#include "simulation_utils.h"
#include "simulation_phys.h"


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
    f_matrix* first_iteration = (f_matrix*)malloc(n * sizeof(f_matrix)); // Pour la variation d'enthalpie
    
    f_matrix* masses = (f_matrix*)malloc(n * sizeof(f_matrix)); // Contiendra en i_e case la matrice de masses de la i_e subdivision selon la largeur

    s_t_matrix* floor_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du sol
    s_t_matrix* left_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur gauche
    s_t_matrix* right_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur droit

    const double lambda = L / n; // Longueur infinitésimale
    const double mu = l / n; // Largeur infinitésimale
    const double height_tot = fluid_volume / (l * L); // On peut alors trouver la hauteur que va occuper le fluide
    const double h_n = height_tot / n; // Soit la hauteur infinitésimale

    double variation_enthalpie_totale = 0;
    double temp_x_plus_1, temp_x_moins_1, temp_y_plus_1, temp_y_moins_1, temp_z_plus_1, temp_z_moins_1; // Représentent à l'itération précédente les températures décalées de + ou - 1 selon x, y ou z


    const double tau = lambda / (fluid_speed / 3.6); // temps (en s) de simulation à tour de simulation

    printf("Lambda = %.6f; mu = %.6f; h_n = %.6f; tau = %.6f\n", lambda, mu, h_n, tau);

    if (floor_temp == NULL || air_temp == NULL || masses == NULL || left_wall_temp == NULL || right_wall_temp == NULL) {
        exit(EXIT_FAILURE);
    }

    // TEMPERATURES DU SOL ET DES MURS

    init_surface_temp(floor_temp, n, n, config_floor_temp, config_floor_h, mu, lambda);
    init_surface_temp(left_wall_temp, n, n, config_l_wall_temp, config_l_wall_h, mu, lambda);
    init_surface_temp(right_wall_temp, n, n, config_r_wall_temp, config_r_wall_h, mu, lambda);

    // TEMPERATURES DE L'AIR

    init_f_mat_val(air_temp, n, n, n, T_e);
    
    // MASSES (pour l'instant pas avec init_f_mat_val car POTENTIELLEMENT pas les cellules toutes les mêmes)

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

    // PREMIERE ITERATION

    init_f_mat_val(first_iteration, n, n, n, T_e);
    
    // Les fichiers de sauvegarde

    FILE* save_air_temp = fopen(save_air_temp_filename, "w");
    FILE* masses_last_first = fopen(masses_last_first_file, "w");
    FILE* air_temp_last_first = fopen(air_temp_last_first_file, "w");

    if (print_to_file) {

        fprintf(save_air_temp,"%i*%i*%i\n", n, n, n); // nb mat, rows, cols
        write_f_mat(save_air_temp, air_temp, (-273.0), n);

        fprintf(masses_last_first,"%i*%i*%i\n", n, n, n); // nb_sub / rows / cols
        fprintf(air_temp_last_first,"%i*%i*%i*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%i\n", n, n, n, T_e, fluid_speed, fluid_volume, L, l, c_p, D, id); // nb_sub / rows / cols / vitesse / volume / Longueur de la surface / largeur / c_p / D / l'id de la simulation| pour l'instant seul ce fichier contiendra toutes les informations de la simulation pour éviter la redodnance

        // Inscription du premier tour de simulation dans les fichiers
        write_f_mat(masses_last_first, masses, 0, n);
        write_f_mat(air_temp_last_first, air_temp, 0, n);
        
    }

    // LA SIMULATION

    idx_couple idx_c;
    idx_c.fst = n-1;
    idx_c.snd = n-1;

    double min_temp = T_e;
    double max_temp = T_e;

    int count = 0;

    // Pour avoir l'itération précédente

    f_matrix* last_air_temp = (f_matrix*)malloc(n * sizeof(f_matrix));
    init_f_mat_val(last_air_temp, n, air_temp[0].cols, air_temp[0].rows, T_e);

    // Pour le pourcentage 

    float iteration = 1;
    float nb_total_iteration = (n - 1) * 2;
    float nb_decoupage = 20;
    printf("\n");

    while (idx_c.snd != 0 || (continuer_meme_si_fini && count < nb_iterations_supplementaires)) {
        
        // Pourcentage

        if ((int)iteration % (100 / (int)nb_decoupage) == 0) {
            printf("\x1b[A\r%.6f%%          \n", iteration * 100 / nb_total_iteration);
        }
        if (continuer_meme_si_fini && idx_c.snd <= 0) {
            count++;
        }
        
        // DEBUT SIMULATION

        conduction_all_surfaces(n, air_temp, last_air_temp, masses, floor_temp, left_wall_temp, right_wall_temp, &min_temp, &max_temp, idx_c, mu, lambda, tau, fluid_speed, c_p);
        copy_f_mat(last_air_temp, air_temp, n);
        convection(n, air_temp, last_air_temp, &min_temp, &max_temp, lambda, mu, h_n, tau, D, fluid_speed, &temp_x_plus_1, &temp_x_moins_1, &temp_y_plus_1, &temp_y_moins_1, &temp_z_plus_1, &temp_z_moins_1);

        // FIN SIMULATION

        iteration++;
        
        // Inscription des données de température de ce tour de simulation dans le fichier

        write_f_mat(save_air_temp, air_temp, (-273.0), n);
        
        // Gestion des indices pour faire comme si le fluide se déplaçait de gauche à droite sur la surface

        idx_c.fst = idx_c.fst - 1;    

        if (idx_c.fst <= 0) {
            idx_c.fst = 0;
            idx_c.snd = idx_c.snd - 1;
            if (idx_c.snd <= 0) { idx_c.snd= 0; }
        }
        
        // COPIE POUR AVOIR ITERATION PRECEDENTE
        
        copy_f_mat(last_air_temp, air_temp, n);
        
    }

    printf("100%%\n");

    // Calcul de la variation d'enthalpie

    variation_enthalpie_totale = calcul_enthalpie(n, masses, air_temp, first_iteration, c_p);

    printf("Min_temp = %.6f; Max_temp = %.6f; Var enth = %.6f\n", min_temp-273.0, max_temp-273.0, variation_enthalpie_totale);


    
    
    // EQUILIBRE THERMIQUE (fonction temporaire !)
    
    /*
    double e = 1;
    int nb_it_eq = 0;
    while (e >= 0.001) {
        nb_it_eq++;
        copy_f_mat(last_air_temp, air_temp, n);
    
        // LA CONVECTION
        convection(n, air_temp, last_air_temp, &min_temp, &max_temp, lambda, mu, h_n, tau, D, fluid_speed, &temp_x_plus_1, &temp_x_moins_1, &temp_y_plus_1, &temp_y_moins_1, &temp_z_plus_1, &temp_z_moins_1);
        iteration++;
        
        // Inscription des données de température de ce tour de simulation dans le fichier

        write_f_mat(save_air_temp, air_temp, (-273.0), n);

        // Calcul de "l'écart" e 
        double compteur_e = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < air_temp[i].rows; j++) {
                for (int k = 0; k < air_temp[i].cols; k++) {                
                    compteur_e += pow(air_temp[i].data[idx(j, k, air_temp[i].cols)] - last_air_temp[i].data[idx(j, k, air_temp[i].cols)], 2);
                }
            }
        }
        e = sqrt(compteur_e / (n * air_temp[0].rows * air_temp[0].cols));    
    }

    printf("L'équilibre a été atteint en %i itérations\n", nb_it_eq);

    */

    if (print_to_file) {
        // A la toute fin du fichier contenant toute la simulation on ajoute la température min et max
        
        fprintf(save_air_temp,"%.6f;%.6f\n", min_temp-273.0, max_temp-273.0);
        fclose(save_air_temp);
        
        // Inscription du dernier tour de simulation dans les fichiers dédiés

        write_f_mat(masses_last_first, masses, 0, n);
        write_f_mat(air_temp_last_first, air_temp, 0, n);
        
    }

    fclose(masses_last_first);
    fclose(air_temp_last_first);


    // On recalcule après la mise à l'équilibre

    variation_enthalpie_totale = calcul_enthalpie(n, masses, air_temp, first_iteration, c_p);
    

    printf("Min_temp = %.6f; Max_temp = %.6f; Var enth = %.6f\n", min_temp-273.0, max_temp-273.0, variation_enthalpie_totale);

    double* res = (double*)malloc(sizeof(double)*2);
    res[0] = min_temp - 273.0;
    res[1] = max_temp - 273.0;

    if (flask) {
        char* res = (char*)malloc(sizeof(char)*2096);
        sprintf(res, "curl -X POST -d 'T_e=%.6f&Vit_air=%.6f&Vol_air=%.6f&L=%.6f&l=%.6f&n=%i&c_p=%.6f&D=%.6f&continuer_meme_si_fini=%i&nb_it_supp=%i&var_enth=%6f&min_temp=%.6f&max_temp=%.6f&id=%i' http://127.0.0.1:5000/", T_e, fluid_speed, fluid_volume, L, l, n, c_p, D, (int)continuer_meme_si_fini, nb_iterations_supplementaires, variation_enthalpie_totale, min_temp -273.0 , max_temp - 273.0, id);
        system(res);
    }

    return res;

}

#endif