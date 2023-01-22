#ifndef SIM_PHYS
#define SIM_PHYS

#include "simulation_utils.h"

void conduction_all_surfaces(int n, f_matrix* air_temp, f_matrix* last_air_temp, f_matrix* masses, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, double* min_temp, double* max_temp, idx_couple idx_c, double mu, double lambda, double tau, double fluid_speed, double c_p) {
    // Conducto-convection au niveau du sol et des murs : la i^e tranche de sol en partant de la gauche ou la i^e tranche de mur en partant du haut pour la gauche et du bas pour la droite
    for (int x = 0; x < n; x++ ) {
        int c = 0; // pour la l'effet 'gauche à droite'
        for (int y = idx_c.fst; y <= idx_c.snd; y++) {
            
            // Sans Rayonnement solaire
            //double new_T_floor = floor_temp_calc(x, y, lambda, mu, tau, floor_temp->data[idx(x, n - 1 - idx_c.snd + c, floor_temp->cols)], last_air_temp, c_p, masses[x].data[idx(masses[x].rows - 1, y, masses[x].cols)], fluid_speed);

            // Avec Rayonnement solaire
            double new_T_floor = floor_temp_calc(x, y, lambda, mu, tau, floor_temp->data[idx(x, n - 1 - idx_c.snd + c, floor_temp->cols)], last_air_temp, c_p, masses[x].data[idx(masses[x].rows - 1, y, masses[x].cols)], fluid_speed);

            air_temp[x].data[idx(air_temp[x].rows - 1, y, air_temp[x].cols)] = new_T_floor;

            double new_T_l_wall = wall_temp_calc(x, y, 0, lambda, mu, tau, left_wall_temp->data[idx(x, n - 1 - idx_c.snd + c, left_wall_temp->cols)], last_air_temp, c_p, masses[0].data[idx(x, y, masses[0].cols)], fluid_speed);
            double new_T_r_wall = wall_temp_calc(x, y, n - 1, lambda, mu, tau, right_wall_temp->data[idx(x, n - 1 - idx_c.snd + c, right_wall_temp->cols)], last_air_temp, c_p, masses[n-1].data[idx(x, y, masses[n-1].cols)], fluid_speed);

            air_temp[0].data[idx(x, y, air_temp[0].cols)] = new_T_l_wall;
            air_temp[n-1].data[idx(x, y, air_temp[n-1].cols)] = new_T_r_wall;

            if (new_T_floor < *min_temp) { *min_temp = new_T_floor; }
            if (new_T_floor > *max_temp) { *max_temp = new_T_floor; }
            if (new_T_l_wall < *min_temp) { *min_temp = new_T_l_wall; }
            if (new_T_l_wall > *max_temp) { *max_temp = new_T_l_wall; }
            if (new_T_r_wall < *min_temp) { *min_temp = new_T_r_wall; }
            if (new_T_r_wall > *max_temp) { *max_temp = new_T_r_wall; }

            c++;
        }
    }
}

void therm_ray_refl(int n, f_matrix* air_temp, f_matrix* last_air_temp, f_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, idx_couple idx_c, double coeff_absorption_thermique_air, double albedo) {
    double radiation_absorbee = albedo * 1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6)))) * coeff_absorption_thermique_air * h_n;
    int absolute_y = (idx_c.fst > 0) ? -last_air_temp[0].cols + idx_c.snd - idx_c.fst : last_air_temp[0].cols - idx_c.snd + idx_c.fst;
    for (int x = 0; x < n; x++ ) {
        int c = 0; // pour la l'effet 'gauche à droite'
        for (int y = idx_c.fst; y <= idx_c.snd; y++) {
            for (int z = 0; z < last_air_temp[x].rows; z++) {
                double temp_y_moins_1 = (y != 0) ? last_air_temp[ x ].data[idx( z ,y-1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                bool is_under_tree = false;
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(x, absolute_y + y, -1, true, true, false, fr.tree_list[t].bb) && z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                }
                if (!is_under_tree) {
                    double new_T_air = air_temp_calc_ray(x, y, z, lambda, tau, c_p, masses[x].data[idx(z, y, masses[x].cols)], last_air_temp[x].data[idx(z, y, last_air_temp[x].cols)], temp_y_moins_1, radiation_absorbee, fluid_speed);
                    air_temp[x].data[idx(z, y, air_temp[x].cols)] = new_T_air;
                    if (new_T_air < *min_temp) { *min_temp = new_T_air; };
                    if (new_T_air > *max_temp) { *max_temp = new_T_air; };
                }

                c++;
            }
        }
    }

}

void therm_ray(int n, f_matrix* air_temp, f_matrix* last_air_temp, f_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, idx_couple idx_c, double coeff_absorption_thermique_air) {
    double radiation_absorbee = 1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6)))) * coeff_absorption_thermique_air * h_n;
    int absolute_y = (idx_c.fst > 0) ? -last_air_temp[0].cols + idx_c.snd - idx_c.fst : last_air_temp[0].cols - idx_c.snd + idx_c.fst;
    for (int x = 0; x < n; x++ ) {
        for (int y = 0; y < last_air_temp[x].cols; y++) {
            for (int z = 0; z < last_air_temp[x].rows; z++) {
                double temp_y_moins_1 = (y != 0) ? last_air_temp[ x ].data[idx( z ,y-1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                bool is_under_tree = false;
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(x, absolute_y + y, -1, true, true, false, fr.tree_list[t].bb) && z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                }
                if (!is_under_tree) {
                    double new_T_air = air_temp_calc_ray(x, y, z, lambda, tau, c_p, masses[x].data[idx(z, y, masses[x].cols)], last_air_temp[x].data[idx(z, y, last_air_temp[x].cols)], temp_y_moins_1, radiation_absorbee, fluid_speed);
                    air_temp[x].data[idx(z, y, air_temp[x].cols)] = new_T_air;
                    if (new_T_air < *min_temp) { *min_temp = new_T_air; };
                    if (new_T_air > *max_temp) { *max_temp = new_T_air; };
                }
            }
        }
    }
}

void therm_stefan(int n, f_matrix* air_temp, f_matrix* last_air_temp, f_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, idx_couple idx_c, double coeff_absorption_thermique_air, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp) {
    int absolute_y = (idx_c.fst > 0) ? -last_air_temp[0].cols + idx_c.snd - idx_c.fst : last_air_temp[0].cols - idx_c.snd + idx_c.fst;
    /* Version tout partout y'en a trop
    for (int x = 0; x < n; x++ ) {
        for (int y = 0; y < last_air_temp[x].cols; y++) {
            for (int z = 0; z < last_air_temp[x].rows; z++) {
                double temp_y_moins_1 = (y != 0) ? last_air_temp[ x ].data[idx( z ,y-1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                bool colliding_from_left = false; // faux si peut recevoir flux du mur gauche, vrai sinon
                bool colliding_from_right = false;
                bool is_under_tree = false;
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(x, absolute_y + y, -1, true, true, false, fr.tree_list[t].bb) && z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                    if ( (is_colliding(-1, absolute_y + y, z, false, true, true, fr.tree_list[t].bb) && x >= (fr.tree_list[t].bb.start_x))) {
                        colliding_from_left = true;
                    }
                    if ( (is_colliding(-1, absolute_y + y, z, false, true, true, fr.tree_list[t].bb) && n - x <= (fr.tree_list[t].bb.start_x + fr.tree_list[t].bb.width))) {
                        colliding_from_right = true;
                    }
                }
                double new_T_air = air_temp_calc_stefan(x, y, z, lambda, tau, c_p, masses[x].data[idx(z, y, masses[x].cols)], last_air_temp[x].data[idx(z, y, last_air_temp[x].cols)], temp_y_moins_1, fluid_speed, floor_temp, left_wall_temp, right_wall_temp, is_under_tree, colliding_from_left, colliding_from_right);
                air_temp[x].data[idx(z, y, air_temp[x].cols)] = new_T_air;
                if (new_T_air < *min_temp) { *min_temp = new_T_air; };
                if (new_T_air > *max_temp) { *max_temp = new_T_air; };
    
            }
        }
    }
    */
    for (int x = 0; x < n; x++ ) {
        int c = 0; // pour la l'effet 'gauche à droite'
        for (int y = idx_c.fst; y <= idx_c.snd; y++) {
            /* Version que les murs/sols

            double temp_y_moins_1_floor  = (n - 1 - idx_c.snd + c != 0) ? last_air_temp[ x ].data[idx( air_temp[x].rows - 1 , n - 1 - idx_c.snd + c - 1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( air_temp[x].rows - 1 , n - 1 - idx_c.snd + c , last_air_temp[x].cols)];
            double temp_y_moins_1_l_wall = (n - 1 - idx_c.snd + c != 0) ? last_air_temp[ 0 ].data[idx( x , n - 1 - idx_c.snd + c - 1, last_air_temp[x].cols)] : last_air_temp[ 0 ].data[idx( x , n - 1 - idx_c.snd + c , last_air_temp[0].cols)];
            double temp_y_moins_1_r_wall = (n - 1 - idx_c.snd + c != 0) ? last_air_temp[ x ].data[idx( x , n - 1 - idx_c.snd + c - 1, last_air_temp[n-1].cols)] : last_air_temp[ n-1 ].data[idx( x , n - 1 - idx_c.snd + c , last_air_temp[n-1].cols)];
            bool colliding_from_left = false; // faux si peut recevoir flux du mur gauche, vrai sinon
            bool colliding_from_right = false;
            bool is_under_tree = false;
            for (int t = 0; t < fr.size; t++) {
                if ( (is_colliding(x, absolute_y + y, -1, true, true, false, fr.tree_list[t].bb) && air_temp[x].rows - 1 >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                    //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                    is_under_tree = true;
                }
                if ( (is_colliding(-1, absolute_y + y, x, false, true, true, fr.tree_list[t].bb) && 0 >= (fr.tree_list[t].bb.start_x))) {
                    colliding_from_left = true;
                }
                if ( (is_colliding(-1, absolute_y + y, x, false, true, true, fr.tree_list[t].bb) && 1 <= (fr.tree_list[t].bb.start_x + fr.tree_list[t].bb.width))) {
                    colliding_from_right = true;
                }
            }

            
            double new_T_floor = air_temp_calc_floor_stefan(x, y, air_temp[x].rows - 1, lambda, tau, c_p, masses[x].data[idx(masses[x].rows - 1, n - 1 - idx_c.snd + c, masses[x].cols)], last_air_temp[x].data[idx(air_temp[x].rows - 1, n - 1 - idx_c.snd + c, last_air_temp[x].cols)], temp_y_moins_1_floor, fluid_speed, floor_temp->data[idx(x, n - 1 - idx_c.snd + c, floor_temp->cols)], is_under_tree);

            air_temp[x].data[idx(air_temp[x].rows - 1, y, air_temp[x].cols)] = new_T_floor;

            double new_T_l_wall = air_temp_calc_floor_stefan(0, y, x, lambda, tau, c_p, masses[0].data[idx(x, n - 1 - idx_c.snd + c, masses[x].cols)], last_air_temp[0].data[idx(x, n - 1 - idx_c.snd + c, last_air_temp[0].cols)], temp_y_moins_1_l_wall, fluid_speed, left_wall_temp->data[idx(x, n - 1 - idx_c.snd + c, floor_temp->cols)], colliding_from_left);
            double new_T_r_wall = air_temp_calc_floor_stefan(0, y, n-1, lambda, tau, c_p, masses[n-1].data[idx(x, n - 1 - idx_c.snd + c, masses[x].cols)], last_air_temp[n-1].data[idx(x, n - 1 - idx_c.snd + c, last_air_temp[n-1].cols)], temp_y_moins_1_r_wall, fluid_speed, right_wall_temp->data[idx(x, n - 1 - idx_c.snd + c, floor_temp->cols)], colliding_from_right);

            air_temp[0].data[idx(x, y, air_temp[0].cols)] = new_T_l_wall;
            air_temp[n-1].data[idx(x, y, air_temp[n-1].cols)] = new_T_r_wall;

            if (new_T_floor < *min_temp) { *min_temp = new_T_floor; }
            if (new_T_floor > *max_temp) { *max_temp = new_T_floor; }
            if (new_T_l_wall < *min_temp) { *min_temp = new_T_l_wall; }
            if (new_T_l_wall > *max_temp) { *max_temp = new_T_l_wall; }
            if (new_T_r_wall < *min_temp) { *min_temp = new_T_r_wall; }
            if (new_T_r_wall > *max_temp) { *max_temp = new_T_r_wall; }

            c++;
            */
            for (int z = 0; z < last_air_temp[x].rows; z++) {
                double temp_y_moins_1_floor  = (n - 1 - idx_c.snd + c != 0) ? last_air_temp[ x ].data[idx( z, n - 1 - idx_c.snd + c - 1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z, n - 1 - idx_c.snd + c , last_air_temp[x].cols)];
                            
                bool colliding_from_left = false; // faux si peut recevoir flux du mur gauche, vrai sinon
                bool colliding_from_right = false;
                bool is_under_tree = false;
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(x, absolute_y + y, -1, true, true, false, fr.tree_list[t].bb) && z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                    if ( (is_colliding(-1, absolute_y + y, z, false, true, true, fr.tree_list[t].bb) && x >= (fr.tree_list[t].bb.start_x))) {
                        colliding_from_left = true;
                    }
                    if ( (is_colliding(-1, absolute_y + y, z, false, true, true, fr.tree_list[t].bb) && n - x <= (fr.tree_list[t].bb.start_x + fr.tree_list[t].bb.width))) {
                        colliding_from_right = true;
                    }
                }
                double new_T_air = air_temp_calc_stefan(x, y, z, lambda, tau, c_p, masses[x].data[idx(z, y, masses[x].cols)], last_air_temp[x].data[idx(z, y, last_air_temp[x].cols)], temp_y_moins_1, fluid_speed, floor_temp, left_wall_temp, right_wall_temp, is_under_tree, colliding_from_left, colliding_from_right);
                air_temp[x].data[idx(z, y, air_temp[x].cols)] = new_T_air;
                if (new_T_air < *min_temp) { *min_temp = new_T_air; };
                if (new_T_air > *max_temp) { *max_temp = new_T_air; };
    
            }
           c++
        }
    }
}
    

void convection(int n, f_matrix* air_temp, f_matrix* last_air_temp, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double D, double fluid_speed, double* temp_x_plus_1, double* temp_x_moins_1, double* temp_y_plus_1, double* temp_y_moins_1, double* temp_z_plus_1, double* temp_z_moins_1, s_t_matrix* floor_temp) {
    for (int x = 0; x < n; x++ ) {
        for (int y = 0; y < last_air_temp[x].cols; y++) {
            for (int z = 0; z < last_air_temp[x].rows; z++) {
                //if (x != 0 && x < n - 1 && y > 0 && y < last_air_temp[x].cols - 1 && z > 0 && z < last_air_temp[x].rows - 1) {

                    *temp_x_moins_1 = (x != 0 )                        ? last_air_temp[x-1].data[idx( z , y , last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                    *temp_x_plus_1  = (x != n - 1)                     ? last_air_temp[x+1].data[idx( z , y , last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                    *temp_y_moins_1 = (y != 0)                         ? last_air_temp[ x ].data[idx( z ,y-1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                    *temp_y_plus_1  = (y != last_air_temp[x].cols - 1) ? last_air_temp[ x ].data[idx( z ,y+1, last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                    *temp_z_moins_1 = (z != 0)                         ? last_air_temp[ x ].data[idx(z-1, y , last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)];
                    *temp_z_plus_1  = (z != last_air_temp[x].rows - 1) ? last_air_temp[ x ].data[idx(z+1, y , last_air_temp[x].cols)] : last_air_temp[ x ].data[idx( z , y , last_air_temp[x].cols)]; //floor_temp->data[idx(x, y, floor_temp->cols)].temp;
                    double new_T_air = air_temp_calc_args(x, y, z, lambda, mu, h_n, n, tau, last_air_temp, D, fluid_speed, *temp_x_plus_1, *temp_x_moins_1, *temp_y_plus_1, *temp_y_moins_1, *temp_z_plus_1, *temp_z_moins_1);
                    air_temp[x].data[idx(z, y, air_temp[x].cols)] = new_T_air;
                    if (new_T_air < *min_temp) { *min_temp = new_T_air; };
                    if (new_T_air > *max_temp) { *max_temp = new_T_air; };
                //}
            }
        }
    }
}


double calcul_enthalpie(int n, f_matrix* masses, f_matrix* air_temp, f_matrix* first_iteration, double c_p) {
    double variation_enthalpie_totale = 0;
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < masses[x].cols; y++) {
            for (int z = 0; z < masses[x].rows; z++) {
                //if (x != 0 && x < n - 1 && y > 0 && y < last_air_temp[x].cols - 1 && z > 0 && z < last_air_temp[x].rows - 1) {
                    variation_enthalpie_totale += masses[x].data[idx(y, z, masses[x].cols)] * c_p * (air_temp[x].data[idx(y, z, air_temp[x].cols)] - first_iteration[x].data[idx(y, z, first_iteration[x].cols)]);
                //}
            }
        }
    }
    return variation_enthalpie_totale;
}



#endif