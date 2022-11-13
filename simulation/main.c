#include "main.h"

int main(int argc, char **argv) {
    /*
    Objectif : Simuler la température  d'une masse d'air entre des murs et un sol dont on peut paramétrer les températures
    Paramètres :
        - T_e (K) : Température initiale de la masse d'air
        - Vitesse_air (km/h) : Explicite
        - Volume_air (m³) : Explicite
        - L (m) : Longueur de la surface de type "sol" (et des murs)
        - l (m) : largeur de cette surface
        - n : nombre de subdivisions en petit pavés de la masse d'air
        - c_p : la capacité calorifique de l'air (ou autre fluide considéré) en J.m^(-3).K^(-1)
        - D (m^2.s^(-1)): coefficient de diffusion de l'air (ou du fluide considéré)
        - config_l_wall_temp : 
        - config_floor_temp :  
        - config_r_wall_temp : 
        - config_l_wall_h : 
        - config_floor_h : 
        - config_r_wall_h : 
        - continuer_meme_si_fini : comme la simulation s'arrête à l'instant ou l'entièreté du fluide est sorti de la zone, il peut être intéressant de regarder plus longtemps l'évolution de la température, ce booléen s'occupe de ça
        - nb_iterations_supplementaires : nombre de tours de simulation faits en plus si le booléen précédent est 'vrai'
        - save_air_temp_filename : nom du fichier contenant l'entièreté de la simulation
        - air_temp_last_first_file : nom du fichier contenant les données du premier et dernier tour de simulation pour les températures de l'air
        - masses_last_first_file : nom du fichier contenant les données du premier et dernier tour de simulation pour les masses
        - print_to_file : indique si la simulation doit être log dans les fichiers donnés précédemment
        - id : sert juste à identifier la simulation, inutile si lancée à la main (mettez la valeur que vous voulez)
    Valeur de retour:
        - Un tuple de flottants correspondants à la température max et min, surtout utile pour le DEBUG
    */

   // Exemple d'utilisation ./main 288 5 7.5 5 5 50 1256 0.03 ../config/left_wall_temp_0.conf ../config/floor_temp_0.conf ../config/right_wall_temp_0.conf ../config/left_wall_h_0.conf ../config/floor_h_0.conf ../config/right_wall_h_0.conf 0 10 ../results/air_temp.tipe ../results/air_temp_last_first.tipe ../results/masses_temp_last_first.tipe 1 1 42

    float T_e, Vitesse_air, Volume_air, L, l, c_p, D;
    int n, continuer_meme_si_fini, nb_it_supp, flask, print_to_file, id;

    char* save_air_temp_filename_buff = (char*)malloc(sizeof(char)*64);
    char* save_air_temp_filename;
    char* air_temp_last_first_file_buff = (char*)malloc(sizeof(char)*64);
    char* air_temp_last_first_file;
    char* masses_temp_last_first_file_buff = (char*)malloc(sizeof(char)*64);
    char* masses_temp_last_first_file;

    char* config_l_wall_temp_buff = (char*)malloc(sizeof(char)*64);
    char* config_l_wall_temp;
    char* config_floor_temp_buff = (char*)malloc(sizeof(char)*64);
    char* config_floor_temp;
    char* config_r_wall_temp_buff = (char*)malloc(sizeof(char)*64);
    char* config_r_wall_temp;

    char* config_l_wall_h_buff = (char*)malloc(sizeof(char)*64);
    char* config_l_wall_h;
    char* config_floor_h_buff = (char*)malloc(sizeof(char)*64);
    char* config_floor_h;
    char* config_r_wall_h_buff = (char*)malloc(sizeof(char)*64);
    char* config_r_wall_h;


    T_e = atof(argv[1]);
    Vitesse_air = atof(argv[2]);
    Volume_air = atof(argv[3]);
    L = atof(argv[4]);
    l = atof(argv[5]);
    n = atoi(argv[6]);
    c_p = atof(argv[7]);
    D = atof(argv[8]);
    config_l_wall_temp = strcpy(config_l_wall_temp_buff, argv[9]);
    config_floor_temp = strcpy(config_floor_temp_buff, argv[10]);
    config_r_wall_temp = strcpy(config_r_wall_temp_buff, argv[11]);
    config_l_wall_h = strcpy(config_l_wall_h_buff, argv[12]);
    config_floor_h = strcpy(config_floor_h_buff, argv[13]);
    config_r_wall_h = strcpy(config_r_wall_h_buff, argv[14]);
    continuer_meme_si_fini = atoi(argv[15]);
    nb_it_supp = atoi(argv[16]);
    save_air_temp_filename = strcpy(save_air_temp_filename_buff, argv[17]);
    air_temp_last_first_file = strcpy(air_temp_last_first_file_buff, argv[18]);
    masses_temp_last_first_file = strcpy(masses_temp_last_first_file_buff, argv[19]);
    flask = atoi(argv[20]);
    print_to_file = atoi(argv[21]);
    id = atoi(argv[22]);
    

    simulation(T_e, Vitesse_air, Volume_air, L, l, n, c_p, D, config_l_wall_temp, config_floor_temp, config_r_wall_temp, config_l_wall_h, config_floor_h, config_r_wall_h, (bool)continuer_meme_si_fini, nb_it_supp, save_air_temp_filename, air_temp_last_first_file, masses_temp_last_first_file, (bool)flask, (bool)print_to_file, id);

    return 0;
}
