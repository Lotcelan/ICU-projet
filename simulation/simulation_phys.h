#ifndef SIM_PHYS
#define SIM_PHYS

#include "simulation_utils.h"

void conduction_all_surfaces(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, double* min_temp, double* max_temp, idx_couple idx_c, double mu, double lambda, double tau, double fluid_speed, double c_p) {
    // Conducto-convection au niveau du sol et des murs : la i^e tranche de sol en partant de la gauche ou la i^e tranche de mur en partant du haut pour la gauche et du bas pour la droite

    // LE SOL
    for (int y = 0; y < n; y++) { 
        for (int x = 0; x < air_temp[y].cols; x++) {
            if (get_cell(air_temp, x, y, air_temp[y].rows - 1).global_x >= n && get_cell(air_temp, x, y, air_temp[y].rows - 1).global_x < 2*n) {

                double new_T_floor = floor_temp_calc(y, x, lambda, mu, tau, get_surf(floor_temp, y, global_x_to_street(get_cell(air_temp, x, y, air_temp[y].rows - 1).global_x, n)), last_air_temp, c_p, get_cell(masses, x, y, masses[y].rows - 1).value, fluid_speed);
                //printf("New_T_floor = %.6f pour x = %i, y = %i\n", new_T_floor, x, y);
                update_cell_value(air_temp, x, y, air_temp[y].rows - 1, new_T_floor);
                update_min_max_temp(new_T_floor, min_temp, max_temp); 
            } 
        }
    }

    // LES MURS
    for (int z = 0; z < air_temp[0].rows; z++) { 
        for (int x = 0; x < air_temp[0].cols; x++) {
            // GAUCHE
            if (get_cell(air_temp, x, 0, z).global_x >= n && get_cell(air_temp, x, 0, z).global_x < 2*n) {
                double new_T_l_wall = wall_temp_calc(z, x, 0, lambda, mu, tau, get_surf(left_wall_temp, z, global_x_to_street(get_cell(air_temp, x, 0, air_temp[0].rows - 1).global_x, n)), last_air_temp, c_p, get_cell(masses, x, 0, z).value, fluid_speed);
                update_cell_value(air_temp, x, 0, z, new_T_l_wall);
                update_min_max_temp(new_T_l_wall, min_temp, max_temp);
            }
            // DROIT
            if (get_cell(air_temp, x, n-1, z).global_x >= n && get_cell(air_temp, x, n-1, z).global_x < 2*n) {
                double new_T_r_wall = wall_temp_calc(z, x, n-1, lambda, mu, tau, get_surf(left_wall_temp, z, global_x_to_street(get_cell(air_temp, x, n-1, air_temp[n-1].rows - 1).global_x, n)), last_air_temp, c_p, get_cell(masses, x, n-1, z).value, fluid_speed);
                update_cell_value(air_temp, x, n-1, z, new_T_r_wall);
                update_min_max_temp(new_T_r_wall, min_temp, max_temp);
            }
        }
    }
}

void therm_ray_refl(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, idx_couple idx_c, double coeff_absorption_thermique_air, double albedo) {
    double radiation_absorbee = albedo * 1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6)))) * coeff_absorption_thermique_air * h_n;
    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {
                double temp_x_moins_1 = (x != 0) ? get_cell(last_air_temp, x-1, y, z).value : get_cell(last_air_temp, x, y, z).value;
                bool is_under_tree = false;
                cell current_cell = get_cell(air_temp, x, y, z);
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(current_cell.global_x, current_cell.global_y, -1, true, true, false, fr.tree_list[t].bb) && current_cell.global_z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                }
                if (!is_under_tree) {
                    double new_T_air = air_temp_calc_ray(x, y, z, lambda, tau, c_p, get_cell(masses, x, y, z).value, get_cell(last_air_temp, x, y, z).value, temp_x_moins_1, radiation_absorbee, fluid_speed);
                    update_cell_value(air_temp, x, y, z, new_T_air);
                    update_min_max_temp(new_T_air, min_temp, max_temp);
                }
            }
        }
    }
}

void therm_ray(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, idx_couple idx_c, double coeff_absorption_thermique_air) {
    double radiation_absorbee = 1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6)))) * coeff_absorption_thermique_air * h_n;
    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {
                double temp_x_moins_1 = (x != 0) ? get_cell(last_air_temp, x-1, y, z).value : get_cell(last_air_temp, x, y, z).value;
                bool is_under_tree = false;
                cell current_cell = get_cell(air_temp, x, y, z);
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(current_cell.global_x, current_cell.global_y, -1, true, true, false, fr.tree_list[t].bb) && current_cell.global_z >= - (fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height) + last_air_temp[x].rows - 1)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                    }
                }
                if (!is_under_tree) {
                    double new_T_air = air_temp_calc_ray(x, y, z, lambda, tau, c_p, get_cell(masses, x, y, z).value, get_cell(last_air_temp, x, y, z).value, temp_x_moins_1, radiation_absorbee, fluid_speed);
                    update_cell_value(air_temp, x, y, z, new_T_air);
                    update_min_max_temp(new_T_air, min_temp, max_temp);
                }
            }
        }
    }
}

/*
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
    * /
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
            * /
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
*/
    

void convection(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double D, double fluid_speed, double* temp_x_plus_1, double* temp_x_moins_1, double* temp_y_plus_1, double* temp_y_moins_1, double* temp_z_plus_1, double* temp_z_moins_1, s_t_matrix* floor_temp) {
    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {
                //if (x != 0 && x < n - 1 && y > 0 && y < last_air_temp[x].cols - 1 && z > 0 && z < last_air_temp[x].rows - 1) {

                    *temp_y_moins_1 = (y != 0 )                        ? get_cell(last_air_temp,   x, y-1,   z).value : get_cell(last_air_temp, x, y, z).value;
                    *temp_y_plus_1  = (y != n - 1)                     ? get_cell(last_air_temp,   x, y+1,   z).value : get_cell(last_air_temp, x, y, z).value;
                    *temp_x_moins_1 = (x != 0)                         ? get_cell(last_air_temp, x-1,   y,   z).value : get_cell(last_air_temp, x, y, z).value;
                    *temp_x_plus_1  = (x != last_air_temp[x].cols - 1) ? get_cell(last_air_temp, x+1,   y,   z).value : get_cell(last_air_temp, x, y, z).value;
                    *temp_z_moins_1 = (z != 0)                         ? get_cell(last_air_temp,   x,   y, z-1).value : get_cell(last_air_temp, x, y, z).value;
                    *temp_z_plus_1  = (z != last_air_temp[x].rows - 1) ? get_cell(last_air_temp,   x,   y, z+1).value : get_cell(last_air_temp, x, y, z).value; //floor_temp->data[idx(x, y, floor_temp->cols)].temp;
                    double new_T_air = air_temp_calc_args(x, y, z, lambda, mu, h_n, n, tau, last_air_temp, D, fluid_speed, *temp_x_plus_1, *temp_x_moins_1, *temp_y_plus_1, *temp_y_moins_1, *temp_z_plus_1, *temp_z_moins_1);
                    update_cell_value(air_temp, x, y, z, new_T_air);
                    update_min_max_temp(new_T_air, min_temp, max_temp);

                //}
            }
        }
    }
}


double calcul_enthalpie(int n, cell_matrix* masses, cell_matrix* air_temp, cell_matrix* first_iteration, double c_p) {
    double variation_enthalpie_totale = 0;
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < masses[y].cols; x++) {
            for (int z = 0; z < masses[y].rows; z++) {
                //if (x != 0 && x < n - 1 && y > 0 && y < last_air_temp[x].cols - 1 && z > 0 && z < last_air_temp[x].rows - 1) {
                variation_enthalpie_totale += get_cell(masses, x, y ,z).value * c_p * (get_cell(air_temp, x ,y, z).value - get_cell(first_iteration, x, y ,z).value);
                //}
            }
        }
    }
    return variation_enthalpie_totale;
}



#endif