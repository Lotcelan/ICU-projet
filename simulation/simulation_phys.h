#ifndef SIM_PHYS
#define SIM_PHYS

#include "simulation_utils.h"

void conduction_all_surfaces(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, double* min_temp, double* max_temp, double mu, double lambda, double tau, double fluid_speed, double c_p) {
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

    // L'AIR AU NIVEAU DES MURS
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
                double new_T_r_wall = wall_temp_calc(z, x, n-1, lambda, mu, tau, get_surf(right_wall_temp, z, global_x_to_street(get_cell(air_temp, x, n-1, air_temp[n-1].rows - 1).global_x, n)), last_air_temp, c_p, get_cell(masses, x, n-1, z).value, fluid_speed);
                update_cell_value(air_temp, x, n-1, z, new_T_r_wall);
                update_min_max_temp(new_T_r_wall, min_temp, max_temp);
            }
        }
    }
}

    
    
void heat_surface_floor(int n, forest fr, s_t_matrix* floor_temp, double tau, cell_matrix* air_temp, double dx, double dy) {
    double phi = 0;
    int g_x = 0;
    surface_temp surf_x_plus_1, surf_x_moins_1, surf_y_plus_1, surf_y_moins_1;
    for (int y = 0; y < floor_temp->rows; y++) { 
        for (int x = 0; x < floor_temp->cols; x++) {
            bool is_under_tree = false;
            g_x = get_cell(air_temp, x, y, air_temp[y].rows - 1).global_x;
            if (g_x >= n && g_x < 2*n) {

                for (int t = 0; t < fr.size; t++) {
                    if (is_colliding(global_x_to_street(g_x, n),y,0, true, true, false, fr.tree_list[t].bb)) { // potentiellement raycasting ici plus tard
                        //printf("Under tree : %i, %i, %i et absoulete_y + y = %i, idx_fst > 0 : %i\n", x, y, z, absolute_y + y, (idx_c.fst > 0) ? 1 : (-1));
                        is_under_tree = true;
                        //printf("Je suis sous un arbre : x = %i; y = %i\n", x, y);
                    }
                }
                if (!is_under_tree) {
                    phi = 502;
                }
                else {
                    phi = 0;
                }
                surface_temp current_surf = get_surf(floor_temp, y, global_x_to_street(g_x, n));
                
            surf_y_moins_1 = (y != 0 )                                                     ? get_surf(floor_temp, y-1, global_x_to_street(g_x, n)) : current_surf;
            surf_y_plus_1  = (y != floor_temp->rows - 1)                                   ? get_surf(floor_temp, y+1, global_x_to_street(g_x, n)) : current_surf;
                surf_x_moins_1 = (global_x_to_street(g_x, n) != 0)                         ? get_surf(floor_temp, y, global_x_to_street(g_x, n)-1) : current_surf;
                surf_x_plus_1  = (global_x_to_street(g_x, n) != floor_temp->cols - 1)      ? get_surf(floor_temp, y, global_x_to_street(g_x, n)+1) : current_surf;
                
                double new_value = solar_radiation_conduction_floor(current_surf.surf, tau, phi, current_surf.temp, get_cell(air_temp, global_x_to_street(g_x, n), y, n-1).value, current_surf.h, dx, dy, surf_x_moins_1, surf_x_plus_1, surf_y_moins_1, surf_y_plus_1);
                
                //printf("En x = %i, y = %i : prev_temp = %.6f; new_temp =%.6f\n", x, y, floor_temp->data[idx(y, x, floor_temp->cols)].temp, new);
                floor_temp->data[idx(y, global_x_to_street(g_x, n), floor_temp->cols)].temp = new_value;
            }
        }
    }
}

void therm_ray(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, double coeff_absorption_thermique_air) {
    //double radiation_absorbee = 1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6)))) * coeff_absorption_thermique_air * h_n;
    double radiation_absorbee = lambda * mu * 1.36 * 1000 * coeff_absorption_thermique_air;
    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {
                double temp_x_moins_1 = (x != 0) ? get_cell(last_air_temp, x-1, y, z).value : get_cell(last_air_temp, x, y, z).value;
                bool is_under_tree = false;
                cell current_cell = get_cell(air_temp, x, y, z);
                for (int t = 0; t < fr.size; t++) {
                    if ( (is_colliding(global_x_to_street(current_cell.global_x, n), current_cell.global_y, -1, true, true, false, fr.tree_list[t].bb) && current_cell.global_z >= fr.tree_list[t].bb.start_z )) { // potentiellement raycasting ici plus tard
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


void therm_stefan(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, double coeff_absorption_thermique_air, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp) {
    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {

                // Version : tout le monde en prend plein la gueule, seulement entre les murs set sols
                cell current_cell = get_cell(air_temp, x, y, z);
                if (current_cell.global_x >= n && current_cell.global_x < 2*n) {
                    double temp_x_moins_1 = (x != 0) ? get_cell(last_air_temp, x-1,   y,   z).value : get_cell(last_air_temp, x, y, z).value;

                    bool is_above_tree = false;
                    bool colliding_from_left = false; // Faux si PEUT recevoir rayonnement du mur gauche
                    bool colliding_from_right = false; // resp mur droit

                    for (int t = 0; t < fr.size; t++) {
                        if ( is_colliding(global_x_to_street(current_cell.global_x, n), current_cell.global_y, -1, true, true, false, fr.tree_list[t].bb) &&  current_cell.global_z <= fr.tree_list[t].bb.start_z + fr.tree_list[t].bb.height ) { // potentiellement raycasting ici plus tard
                            is_above_tree = true;
                        }
                        if ( is_colliding(global_x_to_street(current_cell.global_x, n), -1, current_cell.global_z, true, false, true, fr.tree_list[t].bb) && current_cell.global_y >= fr.tree_list[t].bb.start_y ) {
                            colliding_from_left = true;
                        }
                        if ( is_colliding(global_x_to_street(current_cell.global_x, n), -1, current_cell.global_z, true, false, true, fr.tree_list[t].bb) && current_cell.global_y <= fr.tree_list[t].bb.start_y + fr.tree_list[t].bb.length ) {
                            colliding_from_right = true;
                        }
                    }
                    double new_T_air = air_temp_calc_stefan(x, y, z, lambda, tau, c_p, get_cell(masses, x, y, z).value, get_cell(last_air_temp, x, y, z).value, temp_x_moins_1, fluid_speed, floor_temp, left_wall_temp, right_wall_temp, is_above_tree, colliding_from_left, colliding_from_right);

                    update_cell_value(air_temp, x, y, z, new_T_air);
                    update_min_max_temp(new_T_air, min_temp, max_temp);       
                }
            }
        }
    }
    

}
    

void conduction_air(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double D, double fluid_speed, double* temp_x_plus_1, double* temp_x_moins_1, double* temp_y_plus_1, double* temp_y_moins_1, double* temp_z_plus_1, double* temp_z_moins_1, s_t_matrix* floor_temp) {
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