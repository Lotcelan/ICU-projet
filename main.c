#include "main.h"

int main() {
    /*
    
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - y'en a trop
    */
    float K = 3000/1256.0;
    float D = 0.03;

    int c = 0;
    for (int i = -50; i <= 50; i+=10){
        float* res = simuler(288, 5, 7.5, 5, 5, 50, K, D, i,10,10,false,100,file_id_ext("./results/air_temp_", c),file_id_ext("./results/air_temp_last_first_",c),file_id_ext("./results/masses_last_first_",c));
        c++;
    }
    return 0;
}
