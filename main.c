#include "main.h"

int main() {
    /*
    
    Objectif : Simuler la température  d'une masse d'air entre des murs et un sol dont on peut paramétrer les températures
    Paramètres :
        - T_e (K) : Température initiale de la masse d'air
        - Vitesse_air (km/h) : Explicite
        - Vitesse_air (m³) : Explicite
        - L (m) : Longueur de la surface de type "sol" (et des murs)
        - l (m) : largeur de cette surface
        - n : nombre de subdivisions en petit pavés de la masse d'air
        - K : constante représentant h/c_p avec h le coefficient convectif en W.m^(-2).K^(-1) et c_p la capacité calorifique de l'air (ou autre fluide considéré) en J.m^(-3).K^(-1)
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

    float c_p = 1256.0;
    float D = 0.03;

    int c = 0;
    for (int i = -50; i <= 50; i+=10){
        float* res = simulation(288, 5, 7.5, 5, 5, 50, c_p, D, i, i, i, false, 100, file_id_ext("./results/air_temp_", c), file_id_ext("./results/air_temp_last_first_",c), file_id_ext("./results/masses_last_first_",c));
        c++;
    }
    return 0;
}
