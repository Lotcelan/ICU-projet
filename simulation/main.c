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
        - offset_floor (K) : la tempérture du sol sera de T_e + offset_floor 
        - offset_l_wall (K) : la tempérture du mur gauche sera de T_e + offset_l_wall
        - offset_r_wall (K) : la tempérture du mur droit sera de T_e + offset_r_wall
        - continuer_meme_si_fini : comme la simulation s'arrête à l'instant ou l'entièreté du fluide est sorti de la zone, il peut être intéressant de regarder plus longtemps l'évolution de la température, ce booléen s'occupe de ça
        - nb_iterations_supplementaires : nombre de tours de simulation faits en plus si le booléen précédent est 'vrai'
        - save_air_temp_filename : nom du fichier contenant l'entièreté de la simulation
        - air_temp_last_first_file : nom du fichier contenant les données du premier et dernier tour de simulation pour les températures de l'air
        - masses_last_first_file : nom du fichier contenant les données du premier et dernier tour de simulation pour les masses
    Valeur de retour:
        - Un tuple de flottants correspondants à la température max et min, surtout utile pour le DEBUG
    */

   // ./main 288 5 7.5 5 5 50 1256 30.03 10 10 10 0 10 "air_temp.tipe" "air_temp_last_first.tipe" "masses_temp_last_first.tipe" 0

    float T_e, Vitesse_air, Volume_air, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall;
    int n, continuer_meme_si_fini, nb_it_supp, flask, print_to_file;
    char* save_air_temp_filename_buff = (char*)malloc(sizeof(char)*64);
    char* save_air_temp_filename;
    char* air_temp_last_first_file_buff = (char*)malloc(sizeof(char)*64);
    char* air_temp_last_first_file;
    char* masses_temp_last_first_file_buff = (char*)malloc(sizeof(char)*64);
    char* masses_temp_last_first_file;

    T_e = atof(argv[1]);
    Vitesse_air = atof(argv[2]);
    Volume_air = atof(argv[3]);
    L = atof(argv[4]);
    l = atof(argv[5]);
    n = atoi(argv[6]);
    c_p = atof(argv[7]);
    D = atof(argv[8]);
    offset_floor = atof(argv[9]);
    offset_l_wall = atof(argv[10]);
    offset_r_wall = atof(argv[11]);
    continuer_meme_si_fini = atoi(argv[12]);
    nb_it_supp = atoi(argv[13]);
    save_air_temp_filename = strcpy(save_air_temp_filename_buff, argv[14]);
    air_temp_last_first_file = strcpy(air_temp_last_first_file_buff, argv[15]);
    masses_temp_last_first_file = strcpy(masses_temp_last_first_file_buff, argv[16]);
    flask = atoi(argv[17]);
    print_to_file = atoi(argv[18]);
    

    simulation(T_e, Vitesse_air, Volume_air, L, l, n, c_p, D, offset_floor, offset_l_wall, offset_r_wall, (bool)continuer_meme_si_fini, nb_it_supp, save_air_temp_filename, air_temp_last_first_file, masses_temp_last_first_file, (bool)flask, (bool)print_to_file);

    /*
    float c_p = 1256.0;
    float D = 0.03;

    int c = 0;
    for (int i = -50; i <= 50; i+=10){
        float* res = simulation(288, 5, 7.5, 5, 5, 50, c_p, D, i, i, i, false, 100, file_id_ext("../results/air_temp_", c), file_id_ext("../results/air_temp_last_first_",c), file_id_ext("./results/masses_last_first_",c));
        c++;
    }
    */
    return 0;
}
