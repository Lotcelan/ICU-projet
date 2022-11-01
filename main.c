#include "main.h"

int main() {
    /*
    
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - y'en a trop
    */
    float K = 3000/1256.0;
    float D = 0.03;

    float* res = simuler(288, 5, 7.5, 5, 5, 50, K, D, 10,10,10,false,100,"air_temp.tipe","air_temp_last_first.tipe","masses_last_first.tipe");

    return 0;
}
