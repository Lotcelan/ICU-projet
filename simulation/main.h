#ifndef MAIN
#define MAIN

#include "calc.h"
#include "simulation_utils.h"
#include "simulation_phys.h"

double* simulation(double T_e, double fluid_speed, double fluid_volume, double L, double l,
    int n, double c_p, double D, char* config_l_wall_temp, char*  config_floor_temp, char*  config_r_wall_temp, char*  config_l_wall_h, char*  config_floor_h, char*  config_r_wall_h,
    bool continuer_meme_si_fini, int nb_iterations_supplementaires, char* save_air_temp_filename,
    char* air_temp_last_first_file, char* masses_last_first_file, bool flask, bool print_to_file, int id)
{
    /*
        Effectue la simulation (arguments détaills dans main.c)
    */

    /********************\
    |                    |
    |      Phase 1       |
    |                    |
    |    Initialisation  |
    |                    |
    \********************/
    

    cell_matrix* air_temp = (cell_matrix*)malloc(n * sizeof(cell_matrix)); // Contiendra en i_e case la matrice de températures de la i_e subdivision selon la largeur
    cell_matrix* first_iteration = (cell_matrix*)malloc(n * sizeof(cell_matrix)); // Pour la variation d'enthalpie
    
    cell_matrix* masses = (cell_matrix*)malloc(n * sizeof(cell_matrix)); // Contiendra en i_e case la matrice de masses de la i_e subdivision selon la largeur

    s_t_matrix* floor_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du sol
    s_t_matrix* left_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur gauche
    s_t_matrix* right_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix)); // Matrice contenant les températures du mur droit

    const double lambda = L / n; // Longueur infinitésimale
    const double mu = l / n; // Largeur infinitésimale
    const double height_tot = fluid_volume / (l * L); // On peut alors trouver la hauteur que va occuper le fluide
    const double h_n = height_tot / n; // Soit la hauteur infinitésimale

    double variation_enthalpie_totale = 0;
    double temp_x_plus_1, temp_x_moins_1, temp_y_plus_1, temp_y_moins_1, temp_z_plus_1, temp_z_moins_1; // Représentent à l'itération précédente les températures décalées de + ou - 1 selon x, y ou z

    const double tau = lambda / (fluid_speed / 3.6); // temps (en s) de simulation à chaque tour de simulation

    printf("Lambda = %.6f; mu = %.6f; h_n = %.6f; tau = %.6f\n", lambda, mu, h_n, tau);

    // Paramètres qui ne sont pas en paramètre de l'exécutable (plus le temps de faire ça)

    // x : du début vers la fin; y : du mur gauche; z : de haut en bas
    bounding_box tree1_bb = { .start_x = 10, .start_y = 10, .start_z = 20, .width = 15, .length = 15, .height = 25 };
    tree tree1 = { .bb = tree1_bb };
    
    bounding_box tree2_bb = { .start_x = 55, .start_y = 10, .start_z = 20, .width = 15, .length = 15, .height = 25 };
    tree tree2 = { .bb = tree2_bb };

    bounding_box tree3_bb = { .start_x = 10, .start_y = 55, .start_z = 20, .width = 15, .length = 15, .height = 25 };
    tree tree3 = { .bb = tree3_bb };

    bounding_box tree4_bb = { .start_x = 55, .start_y = 55, .start_z = 20, .width = 15, .length = 15, .height = 25 };
    tree tree4 = { .bb = tree4_bb };

    tree* list = (tree*)malloc(4 * sizeof(tree));
    list[0] = tree1;
    list[1] = tree2;
    list[2] = tree3;
    list[3] = tree4;
    forest fr = { .tree_list = list, .size = 4 };

    double albedo_beton = 0.4;
    double albedo_asphalte = 0.1;
    double albedo_brique = 0.45;
    double albedo_ciment = 0.45;

    if (floor_temp == NULL || air_temp == NULL || masses == NULL || left_wall_temp == NULL || right_wall_temp == NULL) {
        exit(EXIT_FAILURE);
    }

    // TEMPERATURES DU SOL ET DES MURS

    init_surface_temp(floor_temp, n, n, config_floor_temp, config_floor_h, mu, lambda, albedo_asphalte, albedo_beton, albedo_brique, albedo_ciment);
    init_surface_temp(left_wall_temp, n, n, config_l_wall_temp, config_l_wall_h, mu, lambda, albedo_asphalte, albedo_beton, albedo_brique, albedo_ciment);
    init_surface_temp(right_wall_temp, n, n, config_r_wall_temp, config_r_wall_h, mu, lambda, albedo_asphalte, albedo_beton, albedo_brique, albedo_ciment);

    // TEMPERATURES DE L'AIR

    init_cell_mat_val(air_temp, n, n, n, T_e);
    
    // MASSES (pour l'instant pas avec init_f_mat_val car POTENTIELLEMENT pas les cellules toutes les mêmes)

    for (int y = 0; y < n; y++) {
        masses[y].cols = n;
        masses[y].rows = n;

        cell* masses_vals = (cell*)malloc(masses[y].cols * masses[y].rows * sizeof(cell));
        if (masses_vals== NULL) {
            exit(EXIT_FAILURE);
        }
        masses[y].data = masses_vals;

        for (int z = 0; z < masses[y].rows; z++) {
            for (int x = 0; x < masses[y].cols; x++) {
                // Formule qui provient de l'équation des gaz parfaits
                cell new_cell = { .value = 101325.0 * get_surf(floor_temp, z, x).surf.length * get_surf(floor_temp, z, x).surf.width * h_n * 0.0029 / (8.314 * get_cell(air_temp, x, y, z).value),
                                  .local_x = x, .local_y = y, .local_z = z, .global_x = x, .global_y = y, .global_z = z
                                }; 
                masses[y].data[idx(z, x, masses[y].cols)] = new_cell;
            }
        }
    }

    // PREMIERE ITERATION

    init_cell_mat_val(first_iteration, n, n, n, T_e);
    
    // Les fichiers de sauvegarde

    FILE* save_air_temp = fopen(save_air_temp_filename, "w");
    FILE* masses_last_first = fopen(masses_last_first_file, "w");
    FILE* air_temp_last_first = fopen(air_temp_last_first_file, "w");

    FILE* save_floor_temp = fopen("../results/floor_temp_0.tipe", "w");
    fprintf(save_floor_temp,"%i*%i\n", n, n); 

    if (print_to_file) {

        fprintf(save_air_temp,"%i*%i*%i\n", n, n, n); // nb mat, rows, cols
        write_cell_mat(save_air_temp, air_temp, (-273.0), n);

        fprintf(masses_last_first,"%i*%i*%i\n", n, n, n); // nb_sub / rows / cols
        fprintf(air_temp_last_first,"%i*%i*%i*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%.6f*%i\n", n, n, n, T_e, fluid_speed, fluid_volume, L, l, c_p, D, id); // nb_sub / rows / cols / vitesse / volume / Longueur de la surface / largeur / c_p / D / l'id de la simulation| pour l'instant seul ce fichier contiendra toutes les informations de la simulation pour éviter la redodnance

        // Inscription du premier tour de simulation dans les fichiers
        write_cell_mat(masses_last_first, masses, 0, n);
        write_cell_mat(air_temp_last_first, air_temp, 0, n);
        
    }

    // LA SIMULATION

    double min_temp = T_e;
    double max_temp = T_e;

    // Pour avoir l'itération précédente

    cell_matrix* last_air_temp = (cell_matrix*)malloc(n * sizeof(cell_matrix));
    init_cell_mat_val(last_air_temp, n, air_temp[0].cols, air_temp[0].rows, T_e);

    // Pour le pourcentage 

    float iteration = 1;
    float nb_total_iteration = (n - 1) * 2;
    float nb_decoupage = 20;
    printf("\n");

     /********************\
    |                    |
    |      Phase 2       |
    |                    |
    |  Boucle principale |
    |                    |
    \********************/

    while (iteration <= 2*n) {
        
        // Pourcentage

        if ((int)iteration % (100 / (int)nb_decoupage) == 0) {
            printf("\x1b[A\r%.6f%%          \n", iteration * 100 / nb_total_iteration);
        }

        // DEBUT SIMULATION
        write_surf_temp_mat(save_floor_temp, floor_temp, -273);
        heat_surface_floor(n, fr, floor_temp, tau, air_temp, lambda, mu);
        heat_surface_walls(n, fr, left_wall_temp, right_wall_temp, tau, air_temp, lambda, mu);
        // Potentiellement (si fonctionnel) réchauffemnt de l'air par rayonnement stefan du sol et des murs
        conduction_all_surfaces(n, air_temp, last_air_temp, masses, floor_temp, left_wall_temp, right_wall_temp, &min_temp, &max_temp, mu, lambda, tau, fluid_speed, c_p);
        copy_cell_mat(last_air_temp, air_temp, n);
        conduction_air(n, air_temp, last_air_temp, &min_temp, &max_temp, lambda, mu, h_n, tau, D, fluid_speed, &temp_x_plus_1, &temp_x_moins_1, &temp_y_plus_1, &temp_y_moins_1, &temp_z_plus_1, &temp_z_moins_1, floor_temp);
        
        // FIN SIMULATION

        iteration++;
        
        // Inscription des données de température de ce tour de simulation dans le fichier

        write_cell_mat(save_air_temp, air_temp, (-273.0), n);
        
        // Gestion des indices pour faire comme si le fluide se déplaçait de gauche à droite sur la surface

        fluid_offset(air_temp, n);
        
        // COPIE POUR AVOIR ITERATION PRECEDENTE
        
        copy_cell_mat(last_air_temp, air_temp, n);
        
    }

    printf("100%%\n");

    /*********************\
    |                     |
    |      Phase 3        |
    |                     |
    | Actions auxiliaires |
    |                     |
    \*********************/

    // Calcul de la variation d'enthalpie

    variation_enthalpie_totale = calcul_enthalpie(n, masses, air_temp, first_iteration, c_p);

    if (print_to_file) {
        // A la toute fin du fichier contenant toute la simulation on ajoute la température min et max
        
        fprintf(save_air_temp,"%.6f;%.6f\n", min_temp-273.0, max_temp-273.0);
        fclose(save_air_temp);
        
        // Inscription du dernier tour de simulation dans les fichiers dédiés

        write_cell_mat(masses_last_first, masses, 0, n);
        write_cell_mat(air_temp_last_first, air_temp, 0, n);
        
    }

    fclose(masses_last_first);
    fclose(air_temp_last_first);

    printf("Min_temp = %.6f °C; Max_temp = %.6f °C; Var enth = %.6f J\n", min_temp-273.0, max_temp-273.0, variation_enthalpie_totale);

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