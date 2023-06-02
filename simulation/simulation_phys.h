#ifndef SIM_PHYS
#define SIM_PHYS

#include "simulation_utils.h"

void conduction_all_surfaces(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, double* min_temp, double* max_temp, double mu, double lambda, double tau, double fluid_speed, double c_p) {
    // Conducto-convection au niveau du sol et des murs : la i^e tranche de sol en partant de la gauche ou la i^e tranche de mur en partant du haut pour la gauche et du bas pour la droite

    // LE SOL
    int g_x = 0;
    double new_T_floor = 0;
    for (int y = 0; y < n; y++) { 
        for (int x = 0; x < air_temp[y].cols; x++) {
            g_x = get_cell(air_temp, x, y, air_temp[y].rows - 1).global_x;
            if (g_x >= n && g_x < 2*n) {

                new_T_floor= floor_temp_calc(y, x, lambda, mu, tau, get_surf(floor_temp, y, global_x_to_street(g_x, n)), last_air_temp, c_p, get_cell(masses, x, y, masses[y].rows - 1).value, fluid_speed);
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

    
    
void heat_surface_walls(int n, forest fr, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, double tau, cell_matrix* air_temp, double dx, double dy) {
    //  Conducto convection sur LA SURFACE au niveau des murs
    
    int g_x = 0;
    surface prev_temp_surf;
    double prev_temp = 0;
    double V = 0;
    double new_T_wall = 0;
    double h = 0;
    for (int z = 0; z < left_wall_temp->rows; z++) { 
        for (int x = 0; x < left_wall_temp->cols; x++) {
            // GAUCHE
            g_x = get_cell(air_temp, x, 0, z).global_x;
            if (g_x >= n && g_x < 2*n) {
                prev_temp_surf = get_surf(left_wall_temp, global_x_to_street(g_x, n), z).surf;
                h = get_surf(left_wall_temp, global_x_to_street(g_x, n), z).h;
                prev_temp = left_wall_temp->data[idx(global_x_to_street(g_x, n), z, left_wall_temp->cols)].temp;
                V = prev_temp_surf.height * prev_temp_surf.length * prev_temp_surf.width;
                new_T_wall = tau * prev_temp_surf.width * prev_temp_surf.length * h / (prev_temp_surf.masse_vol * V * prev_temp_surf.capacite_thermique) * (prev_temp - get_cell(air_temp, x, 0, z).value) + prev_temp;

                left_wall_temp->data[idx(global_x_to_street(g_x, n), z, left_wall_temp->cols)].temp = new_T_wall;          
            }

            // DROIT
            get_cell(air_temp, x, n-1, z).global_x;
            if (g_x>= n && g_x< 2*n) {
                prev_temp_surf = get_surf(right_wall_temp, global_x_to_street(g_x, n), z).surf;
                h = get_surf(right_wall_temp, global_x_to_street(g_x, n), z).h;
                prev_temp = right_wall_temp->data[idx(global_x_to_street(g_x, n), z, right_wall_temp->cols)].temp;
                V = prev_temp_surf.height * prev_temp_surf.length * prev_temp_surf.width;
                new_T_wall = tau * prev_temp_surf.width * prev_temp_surf.length * h / (prev_temp_surf.masse_vol * V * prev_temp_surf.capacite_thermique) * (prev_temp - get_cell(air_temp, x, n-1, z).value) + prev_temp;
                right_wall_temp->data[idx(global_x_to_street(g_x, n), z, right_wall_temp->cols)].temp = new_T_wall;          

            }
        }
    }
}


    
void heat_surface_floor(int n, forest fr, s_t_matrix* floor_temp, double tau, cell_matrix* air_temp, double dx, double dy) {
    //  Conducto convection + rayonnement solaire sur LE SOL au niveau des murs

    
    double phi = 0;
    cell_option above_cell;
    surface_temp surf_x_plus_1, surf_x_moins_1, surf_y_plus_1, surf_y_moins_1;
    for (int y = 0; y < floor_temp->rows; y++) { 
        for (int x = 0; x < floor_temp->cols; x++) {
            bool is_under_tree = false;
            above_cell = get_cell_from_street(air_temp, x, y, n-1);
            for (int t = 0; t < fr.size; t++) {
                if (is_colliding(x,y,0, true, true, false, fr.tree_list[t].bb)) { // potentiellement raycasting ici plus tard
                    is_under_tree = true;
                }
            }
            if (!is_under_tree) {
                phi = 502;
            }
            else {
                phi = 0;
            }
            surface_temp current_surf = get_surf(floor_temp, y, x);
            
            surf_y_moins_1 = (y != 0 )                                                 ? get_surf(floor_temp, y-1, x) : current_surf;
            surf_y_plus_1  = (y != floor_temp->rows - 1)                               ? get_surf(floor_temp, y+1, x) : current_surf;
            surf_x_moins_1 = (x != 0)                         ? get_surf(floor_temp, y, x-1) : current_surf;
            surf_x_plus_1  = (x != floor_temp->cols - 1)      ? get_surf(floor_temp, y, x+1) : current_surf;
            
            double new_value = solar_radiation_conduction_floor(current_surf.surf, tau, phi, current_surf.temp, above_cell, current_surf.h, dx, dy, surf_x_moins_1, surf_x_plus_1, surf_y_moins_1, surf_y_plus_1);
            
            floor_temp->data[idx(y, x, floor_temp->cols)].temp = new_value;
        }
    }
}

void therm_stefan(int n, cell_matrix* air_temp, cell_matrix* last_air_temp, cell_matrix* masses, double* min_temp, double* max_temp, double lambda, double mu, double h_n, double tau, double fluid_speed, double c_p, forest fr, double coeff_absorption_thermique_air, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp) {
    // Fonction qui aurait servi à réchauffer l'air avec rayonnement stefan du sol et des murs

    for (int y = 0; y < n; y++ ) {
        for (int x = 0; x < last_air_temp[y].cols; x++) {
            for (int z = 0; z < last_air_temp[y].rows; z++) {

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
    // Diffusion de la température avec équation de chaleur
    
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
    // Explicite

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