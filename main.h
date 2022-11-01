#ifndef MAIN
#define MAIN

#include <stdlib.h>
#include "stdio.h"
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "couloir.h"

float rand_sgn() {
    int tmp = rand() % 2;
    switch (tmp)
    {
    case 0:
        return (-1.0);
        break;
    case 1:
        return 1.0;
        break;
    return 1.0;
        break;
    }
}

float calculer_T_wall(int i, int j, int left_right, float lambda, float mu, float tau, surface_temp s_t, f_matrix* air_temp, float K, float m_i, float Vitesse_air) {
    
    
    if (j > 0 && (j%air_temp->cols != 0)) {
        return (Vitesse_air/3.6*air_temp[left_right].data[idx(i, j-1, air_temp[left_right].cols)]/lambda + air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)]/tau - K/m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp)/(Vitesse_air/3.6/lambda + 1/tau - K*(s_t.surf.length * s_t.surf.width)/m_i);
    } else {
        return (Vitesse_air/3.6*air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)]/lambda + air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)]/tau - K/m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp)/(Vitesse_air/3.6/lambda + 1/tau - K*(s_t.surf.length * s_t.surf.width)/m_i);
    }

}

/*
float calculer_h(float delta_t, float L) {
    return 1.32 * sqrt(sqrt(abs(delta_t/L)));
}
*/

float* simuler(float T_e, float Vitesse_air, float Volume_air, float L, float l, int n, float K, float D, float offset_floor, float offset_l_wall, float offset_r_wall, bool continuer_meme_si_fini, int nb_iterations_supplementaires, char* save_air_temp_filename, char* air_temp_last_first_file, char* masses_last_first_file) { // TODO : custom la temp du sol et des murs

    f_matrix* air_temp = (f_matrix*)malloc(n*sizeof(f_matrix));
    f_matrix* masses = (f_matrix*)malloc(n*sizeof(f_matrix));

    s_t_matrix* floor_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix));
    s_t_matrix* left_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix));
    s_t_matrix* right_wall_temp = (s_t_matrix*)malloc(sizeof(s_t_matrix));

    const float lambda = L/n; 
    const float mu = l/n;
    const float height_tot = Volume_air/(l*lambda);
    const float h_n = height_tot/n;

    const float tau = lambda/(Vitesse_air/3.6); // s
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

    surface_temp* floor = (surface_temp*)malloc(n*n*sizeof(surface_temp));
    surface_temp* l_wall = (surface_temp*)malloc(n*n*sizeof(surface_temp));
    surface_temp* r_wall = (surface_temp*)malloc(n*n*sizeof(surface_temp));
    
    floor_temp->data = floor;
    left_wall_temp->data = l_wall;
    right_wall_temp->data = r_wall;

    for (int j = 0; j < floor_temp->rows; j++) {        // On suppose que murs et sol ont la même dimension
        for (int k = 0; k < floor_temp->cols; k++) {
            surface new = { .width = mu, .length = lambda};

            floor_temp->data[idx(j,k,floor_temp->cols)].temp = T_e + offset_floor;
            floor_temp->data[idx(j,k,floor_temp->cols)].surf = new;

            left_wall_temp->data[idx(j,k,floor_temp->cols)].temp = T_e + offset_l_wall;
            left_wall_temp->data[idx(j,k,floor_temp->cols)].surf = new;

            right_wall_temp->data[idx(j,k,floor_temp->cols)].temp = T_e + offset_r_wall;
            right_wall_temp->data[idx(j,k,floor_temp->cols)].surf = new;

            /*
            if (k > 25 && k < n - 20) {
                right_wall_temp->data[idx(j,k,floor_temp->cols)].temp = T_e+500;
                right_wall_temp->data[idx(j,k,floor_temp->cols)].surf = new;
            } else {
                right_wall_temp->data[idx(j,k,floor_temp->cols)].temp = T_e -100;
                right_wall_temp->data[idx(j,k,floor_temp->cols)].surf = new;
            }
            */
        } 
    }

    // TEMPERATURES DE L'AIR

    for (int i = 0; i < n; i++) {
        air_temp[i].cols = n;
        air_temp[i].rows = n;

        float* temp = (float*)malloc(n*n*sizeof(float));
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

        float* temp = (float*)malloc(n*n*sizeof(float));
        masses[i].data = temp;

        for (int j = 0; j < masses[i].rows; j++) {
            for (int k = 0; k < masses[i].cols; k++) {
                masses[i].data[idx(j, k, masses[i].cols)] = 101325.0 * floor_temp->data[idx(j,k,floor_temp->cols)].surf.length * floor_temp->data[idx(j,k,floor_temp->cols)].surf.width * h_n * 0.0029 / (8.314*air_temp[i].data[idx(i,j,air_temp->cols)]);
            }
        }
    }

    // LA SIMULATION

    idx_couple idx_c;
    idx_c.fst = n-1;
    idx_c.snd = n-1;

    FILE* f = fopen(save_air_temp_filename,"w");

    fprintf(f,"%i*%i*%i\n", n, n, n); // nb mat, rows, cols

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < air_temp[i].rows; j++) {
            for (int k = 0; k < air_temp[i].cols - 1; k++) {
                fprintf(f, "%.6f;", air_temp[i].data[idx(j,k,air_temp[i].cols)]-273.0);
            }
            fprintf(f, "%.6f\n", air_temp[i].data[idx(j,air_temp[i].cols - 1,air_temp[i].cols)]-273.0);

        }
    }

    FILE* masses_last_first = fopen(masses_last_first_file,"w");
    FILE* air_temp_last_first = fopen(air_temp_last_first_file,"w");

    fprintf(masses_last_first,"%i*%i*%i\n", n, n, n); // nb_sub / rows / cols
    fprintf(air_temp_last_first,"%i*%i*%i\n", n, n, n); // nb_sub / rows / cols

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

    float min_temp = T_e;
    float max_temp = T_e;

    int compteur = 0;
    while (idx_c.snd != 0 || (continuer_meme_si_fini && compteur < nb_iterations_supplementaires)) {

        if (continuer_meme_si_fini && idx_c.snd <= 0) {
            compteur++;
        }

        // COPIE POUR AVOIR ITERATION PRECEDENTE

        f_matrix* last_air_temp = (f_matrix*)malloc(n*sizeof(f_matrix));
        
        for (int i = 0; i < n; i++) {
            last_air_temp[i].cols = air_temp->cols;
            last_air_temp[i].rows = air_temp->rows;
            float* temp = (float*)malloc(n*n*sizeof(float));
            last_air_temp[i].data = temp;
            //memcpy(&last_air_temp[i].data, &air_temp[i].data, n*n*sizeof(float));
            for (int j = 0; j < last_air_temp[i].rows; j++) {
                for (int k = 0; k < last_air_temp[i].cols; k++) {
                    last_air_temp[i].data[idx(j,k,last_air_temp[i].cols)] = air_temp[i].data[idx(j,k,last_air_temp[i].cols)];
                }
            }
        }
        
        for (int i = 0; i < n; i++ ) {
            int c = 0;
            for (int j = idx_c.fst; j <= idx_c.snd; j++) {

                // LA CONDUCTION
                
                //if (j != 0 && j != n-1 && i != 0 && i != n-1) {    
                    float new_T_floor = calculer_T_floor(i,j,lambda,mu,tau,floor_temp->data[idx(i,n-1-idx_c.snd+c,floor_temp->cols)],last_air_temp,K,masses[i].data[idx(air_temp->rows - 1, j, masses[i].cols)], Vitesse_air);
                    //float new_T_floor = calculer_T_floor(i,j,lambda,mu,tau,floor_temp->data[idx(i,n-1-idx_c.snd+c,floor_temp->cols)],last_air_temp,calculer_h(floor_temp->data[idx(i,n-1-idx_c.snd+c,floor_temp->cols)].temp - air_temp[i].data[idx(air_temp[i].rows-1, j, air_temp[i].cols)], floor_temp->data[idx(i,n-1-idx_c.snd+c,floor_temp->cols)].surf.length) / 1256.0, masses[i].data[idx(air_temp->rows - 1, j, air_temp->cols)], Vitesse_air);
                    //printf("i = %i, j = %i, old_temp = %.6f, new_temp = %.6f\n",i,j,air_temp[i].data[idx(air_temp->rows - 1, j, air_temp->cols)],new_T_floor);
                    air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] = new_T_floor;
                    float new_T_l_wall = calculer_T_wall(i,j,0,lambda,mu,tau,left_wall_temp->data[idx(i,n-1-idx_c.snd+c,left_wall_temp->cols)],last_air_temp,K,masses[0].data[idx(i, j, masses[0].cols)], Vitesse_air);
                    float new_T_r_wall = calculer_T_wall(i,j,n-1,lambda,mu,tau,right_wall_temp->data[idx(i,n-1-idx_c.snd+c,right_wall_temp->cols)],last_air_temp,K,masses[n-1].data[idx(i, j, masses[n-1].cols)], Vitesse_air);
                    //printf("New_T_floor = %.6f, new_T_l_wall = %.6f, new_T_r_wall =%.6f - i = %i, j = %i\n",new_T_floor, new_T_l_wall, new_T_r_wall, i, j);              

                    air_temp[0].data[idx(i, j, air_temp[0].cols)] = new_T_l_wall;
                    air_temp[n-1].data[idx(i, j, air_temp[n-1].cols)] = new_T_r_wall;
                    //printf("Encountered on right wall : %.6f | gave : %.6f", right_wall_temp->data[idx(i,n-1-idx_c.snd+c,right_wall_temp->cols)], j)

                    if (new_T_floor < min_temp) { min_temp = new_T_floor; }
                    if (new_T_floor > max_temp) { max_temp = new_T_floor; }
                    if (new_T_l_wall < min_temp) { min_temp = new_T_l_wall; }
                    if (new_T_l_wall > max_temp) { max_temp = new_T_l_wall; }
                    if (new_T_r_wall < min_temp) { min_temp = new_T_r_wall; }
                    if (new_T_r_wall > max_temp) { max_temp = new_T_r_wall; }
                //}    
                c++;
            }

            // LA CONVECTION

            for (int j = 1; j < n-1; j++) {

                for (int m = 1; m < air_temp[i].rows -1; m++) { 
                    if (i != 0 && i != n-1) {
                        float new_T_air = calculer_T_air(i,j,m,lambda,mu,h_n,n,tau,last_air_temp,D);
                        air_temp[i].data[idx(m, j, air_temp[i].cols)] = new_T_air;

                        if (new_T_air < min_temp) { min_temp = new_T_air; }
                        if (new_T_air > max_temp) { max_temp = new_T_air; }
                    }
                }

            }

            /*
            for (int j = 0; j < masses[i].rows; j++) {
                for (int k = 0; k < masses[i].cols; k++) {
                    masses[i].data[idx(j, k, masses[i].cols)] = 101325.0 * floor_temp->data[idx(j,k,floor_temp->cols)].surf.length * floor_temp->data[idx(j,k,floor_temp->cols)].surf.width * h_n * 0.0029 / (8.314*air_temp[i].data[idx(i,j,air_temp->cols)]);
                }
            }
            */
            free(last_air_temp[i].data);

            
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < air_temp[i].rows; j++) {
                for (int k = 0; k < air_temp[i].cols - 1; k++) {
                    fprintf(f, "%.6f;", air_temp[i].data[idx(j,k,air_temp[i].cols)]-273.0);
                }
                fprintf(f, "%.6f\n", air_temp[i].data[idx(j,air_temp[i].cols - 1,air_temp[i].cols)]-273.0);

            }
        }
        idx_c.fst = idx_c.fst - 1;
            

        if (idx_c.fst <= 0) {
            idx_c.fst = 0;
            idx_c.snd = idx_c.snd - 1;
            if (idx_c.snd <= 0) { idx_c.snd= 0; }
        }
        
        
        free(last_air_temp);
    }

    fprintf(f,"%.6f;%.6f\n", min_temp-273.0, max_temp-273.0);
    fclose(f);
    printf("Min_temp = %.6f; Max_temp = %.6f\n", min_temp-273.0, max_temp-273.0);
    

    
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
    
    fclose(masses_last_first);
    fclose(air_temp_last_first);

    float* res = malloc(sizeof(float)*2);
    res[0] = min_temp - 273.0;
    res[1] = max_temp - 273.0;

    return res;

}

#endif