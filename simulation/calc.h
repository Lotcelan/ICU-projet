#ifndef CALC
#define CALC

#include "def.h"

double floor_temp_calc(int y, int x, double lambda, double mu, double tau, surface_temp s_t, cell_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
    if (x > 0 && (x % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * get_cell(air_temp, x-1, y, air_temp[y].rows - 1).value / lambda + get_cell(air_temp, x, y, air_temp[y].rows - 1).value / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * get_cell(air_temp,   x, y, air_temp[y].rows - 1).value / lambda + get_cell(air_temp, x, y, air_temp[y].rows - 1).value / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }   
}

double air_temp_calc_ray(int x, int y, int z, double lambda, double tau, double c_p, double m_i, double prev_temp, double temp_x_moins_1, double radiation_absorbee, double fluid_speed) {
    return ( fluid_speed / 3.6 / lambda * temp_x_moins_1 + prev_temp / tau + radiation_absorbee / m_i / c_p ) /  ( fluid_speed / 3.6 / lambda + 1 / tau );
    // return prev_temp + radiation_absorbee * tau / c_p / m_i;
}

double air_temp_calc_stefan(int x, int y, int z, double lambda, double tau, double c_p, double m_i, double prev_temp, double temp_x_moins_1, double fluid_speed, s_t_matrix* floor_temp, s_t_matrix* left_wall_temp, s_t_matrix* right_wall_temp, bool is_under_tree, bool colliding_from_left, bool colliding_from_right) {
    double sigma = 5.67 / 100000000;
    double sum_T_4_surf = 0;
    if (!is_under_tree)        { sum_T_4_surf += pow(floor_temp     ->data[idx(y, x, floor_temp->cols)].temp,      4) * floor_temp->data[idx(y, x, floor_temp->cols)].surf.width * floor_temp->data[idx(y, x, floor_temp->cols)].surf.length; }
    if (!colliding_from_left)  { sum_T_4_surf += pow(left_wall_temp ->data[idx(z, x, left_wall_temp->cols)].temp,  4) * left_wall_temp ->data[idx(z, x, left_wall_temp->cols)].surf.width * left_wall_temp ->data[idx(z, x, left_wall_temp->cols)].surf.length; }
    if (!colliding_from_right) { sum_T_4_surf += pow(right_wall_temp->data[idx(z, x, right_wall_temp->cols)].temp, 4) * right_wall_temp->data[idx(z, x, right_wall_temp->cols)].surf.width * right_wall_temp->data[idx(z, x, right_wall_temp->cols)].surf.length; }

    return ( fluid_speed / 3.6 / lambda * temp_x_moins_1 + prev_temp / tau + sigma * sum_T_4_surf / m_i / c_p ) /  ( fluid_speed / 3.6 / lambda + 1 / tau );
}


//double floor_temp_calc_ray(int i, int j, double lambda, double mu, double tau, surface_temp s_t, f_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
//    double ray_solaire_surf = 341; //1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6))));
//    if (j > 0 && (j % air_temp->cols != 0)) {
//        return (0.2 * ray_solaire_surf * s_t.surf.length * s_t.surf.width / m_i / c_p + fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j - 1, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
//    } else {
//        return (0.2 * ray_solaire_surf * s_t.surf.length * s_t.surf.width / m_i / c_p + fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
//    }   
//}

double wall_temp_calc(int y, int x, int wall_idx, double lambda, double mu, double tau, surface_temp s_t, cell_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
    if (x > 0 && (x % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * get_cell(air_temp, x-1, wall_idx, air_temp[wall_idx].rows - 1).value / lambda + get_cell(air_temp, x, wall_idx, air_temp[wall_idx].rows - 1).value / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * get_cell(air_temp,   x, wall_idx, air_temp[wall_idx].rows - 1).value / lambda + get_cell(air_temp, x, wall_idx, air_temp[wall_idx].rows - 1).value / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }   
}

double air_temp_calc_args(int x, int y, int z, double lambda, double mu, double h_n, int nb_subd, double tau, cell_matrix* prev_temp, double D, double fluid_speed, double temp_x_plus_1, double temp_x_moins_1, double temp_y_plus_1, double temp_y_moins_1, double temp_z_plus_1, double temp_z_moins_1) {
    double prev_xyz_temp = get_cell(prev_temp, x, y ,z).value;
    return prev_xyz_temp
        + D * tau * ( (temp_x_plus_1 - 2 * prev_xyz_temp + temp_x_moins_1 ) / (mu * mu)
                    + (temp_y_plus_1 - 2 * prev_xyz_temp + temp_y_moins_1 ) / (lambda * lambda)
                    + (temp_z_plus_1 - 2 * prev_xyz_temp + temp_z_moins_1 ) / (h_n * h_n) );
    //return 
    //    tau * (D * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
    //                + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
    //                + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) )
    //    - fluid_speed / 3.6  * (temp_y_plus_1 - prev_temp[x].data[idx(z, y, prev_temp[x].cols)]) / lambda)
    //    + prev_temp[x].data[idx(z, y, prev_temp[x].cols)];   
}

void calculer_flux_conduction_surface(surface curr_surf, surface_temp surf_x_moins_1, surface_temp surf_x_plus_1, surface_temp surf_y_moins_1, surface_temp surf_y_plus_1, double* flux_x_moins_1, double* flux_x_plus_1, double* flux_y_moins_1, double* flux_y_plus_1, double prev_temp, double dx, double dy) {
    double height_contact = 0;
    
    if (curr_surf.height >  surf_x_moins_1.surf.height) {
        height_contact = surf_x_moins_1.surf.height;
    } else {
        height_contact = curr_surf.height;
    }
    *flux_x_moins_1 =  - 2 * dy * height_contact / dx * (1 / curr_surf.conductivite_thermique + 1 / surf_x_moins_1.surf.conductivite_thermique) * (prev_temp - surf_x_moins_1.temp);

    
    if (curr_surf.height >  surf_x_plus_1.surf.height) {
        height_contact = surf_x_plus_1.surf.height;
    } else {
        height_contact = curr_surf.height;
    }
    *flux_x_plus_1 =  - 2 * dy * height_contact / dx * (1 / curr_surf.conductivite_thermique + 1 / surf_x_plus_1.surf.conductivite_thermique) * (prev_temp  - surf_x_plus_1.temp);
    
    if (curr_surf.height >  surf_y_moins_1.surf.height) {
        height_contact = surf_y_moins_1.surf.height;
    } else {
        height_contact = curr_surf.height;
    }
    *flux_y_moins_1 =  - 2 * dx * height_contact / dy * (1 / curr_surf.conductivite_thermique + 1 / surf_y_moins_1.surf.conductivite_thermique) * (prev_temp - surf_y_moins_1.temp);
    
    if (curr_surf.height >  surf_y_plus_1.surf.height) {
        height_contact = surf_y_plus_1.surf.height;
    } else {
        height_contact = curr_surf.height;
    }
    *flux_y_plus_1 = -  2 * dx * height_contact / dy * (1 / curr_surf.conductivite_thermique + 1 / surf_y_plus_1.surf.conductivite_thermique) * (prev_temp - surf_y_plus_1.temp);
        

    /*
    if (curr_surf.conductivite_thermique - surf_x_moins_1.surf.conductivite_thermique > 0.001) {
        if (curr_surf.height >  surf_x_moins_1.surf.height) {
            height_contact = surf_x_moins_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_x_moins_1 =  2 * dy * height_contact / dx * curr_surf.conductivite_thermique * surf_x_moins_1.surf.conductivite_thermique / (curr_surf.conductivite_thermique + surf_x_moins_1.surf.conductivite_thermique) * (prev_temp - surf_x_moins_1.temp);
    } else {
        if (curr_surf.height >  surf_x_moins_1.surf.height) {
            height_contact = surf_x_moins_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_x_moins_1 = curr_surf.conductivite_thermique * (surf_x_moins_1.temp - prev_temp) / dx * height_contact * dy; 
    }

    
    if (curr_surf.conductivite_thermique - surf_x_plus_1.surf.conductivite_thermique > 0.001) {
        if (curr_surf.height >  surf_x_plus_1.surf.height) {
            height_contact = surf_x_plus_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_x_plus_1 =   2 * dy * height_contact / dx * curr_surf.conductivite_thermique * surf_x_plus_1.surf.conductivite_thermique / (curr_surf.conductivite_thermique + surf_x_plus_1.surf.conductivite_thermique) * (prev_temp  - surf_x_plus_1.temp);
    } else {
        if (curr_surf.height >  surf_x_plus_1.surf.height) {
            height_contact = surf_x_plus_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_x_plus_1 =  curr_surf.conductivite_thermique * (surf_x_plus_1.temp - prev_temp) / dx * height_contact * dy; 
    }

    
    if (curr_surf.conductivite_thermique - surf_y_moins_1.surf.conductivite_thermique > 0.001) {
        if (curr_surf.height >  surf_y_moins_1.surf.height) {
            height_contact = surf_y_moins_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_y_moins_1 =   2 * dx * height_contact / dy * curr_surf.conductivite_thermique * surf_y_moins_1.surf.conductivite_thermique / (curr_surf.conductivite_thermique + surf_y_moins_1.surf.conductivite_thermique) * (prev_temp - surf_y_moins_1.temp);
    } else {
        if (curr_surf.height >  surf_y_moins_1.surf.height) {
            height_contact = surf_y_moins_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_y_moins_1 =  curr_surf.conductivite_thermique * (surf_y_moins_1.temp - prev_temp) / dy * height_contact * dx; 
    }

    
    if (curr_surf.conductivite_thermique - surf_y_plus_1.surf.conductivite_thermique > 0.001) {
        if (curr_surf.height >  surf_y_plus_1.surf.height) {
            height_contact = surf_y_plus_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_y_plus_1 =   2 * dx * height_contact / dy * curr_surf.conductivite_thermique * surf_y_plus_1.surf.conductivite_thermique / (curr_surf.conductivite_thermique + surf_y_plus_1.surf.conductivite_thermique) * (prev_temp - surf_y_plus_1.temp);
    } else {
        if (curr_surf.height >  surf_y_plus_1.surf.height) {
            height_contact = surf_y_plus_1.surf.height;
        } else {
            height_contact = curr_surf.height;
        }
        *flux_y_plus_1 = curr_surf.conductivite_thermique * (surf_y_plus_1.temp - prev_temp) / dy * height_contact * dx; 
    }
    */
}

double solar_radiation_conduction_floor(surface curr_surf, double tau, double phi, double prev_temp, double air_temp_above, double h, double dx, double dy, surface_temp surf_x_moins_1, surface_temp surf_x_plus_1, surface_temp surf_y_moins_1, surface_temp surf_y_plus_1) {
    double S = curr_surf.width * curr_surf.length;
    double m = curr_surf.masse_vol * S * curr_surf.height;
    double c = curr_surf.capacite_thermique;
    double al = curr_surf.albedo;
    double sigma = 5.67 / 100000000;
    double flux_x_plus_1, flux_x_moins_1, flux_y_plus_1, flux_y_moins_1; // flux pas surfaciques (en W quoi)

    calculer_flux_conduction_surface(curr_surf, surf_x_moins_1, surf_x_plus_1, surf_y_moins_1, surf_y_plus_1, &flux_x_moins_1, &flux_x_plus_1, &flux_y_moins_1, &flux_y_plus_1, prev_temp, dx, dy);

    //return prev_temp * (1 + h * S * tau / (m * c)) + S * tau * (phi - h * air_temp_above) / (m * c);
    //return 1 / (h * S - m * c / tau) * (h * S * air_temp_above - phi * S - m * c * prev_temp / tau);
    //return tau / (m * c) * ( m * c / tau * prev_temp + h * S * (prev_temp - air_temp_above) + phi * S );
    return prev_temp + tau / (m * c) * (S * ((1 - al) * phi + h * (prev_temp - air_temp_above) - sigma * pow(prev_temp, 4)) + flux_x_moins_1 + flux_x_plus_1 + flux_y_moins_1 + flux_y_plus_1 );                                            
}

/*
double air_temp_calc(int x, int y, int z, double lambda, double mu, double h_n, int nb_subd, double tau, f_matrix* prev_temp, double D, double fluid_speed) {

    double temp_x_plus_1, temp_x_moins_1, temp_y_plus_1, temp_y_moins_1, temp_z_plus_1, temp_z_moins_1; // Représentent à l'itération précédente les températures décalées de + ou - 1 selon x, y ou z

    assert( x > 0 && x < nb_subd - 1 && y > 0 && y < prev_temp[x].cols && z > 0 && z < prev_temp[x].rows);
    temp_x_moins_1 = prev_temp[x-1].data[idx( z , y , prev_temp[x].cols)];
    temp_x_plus_1  = prev_temp[x+1].data[idx( z , y , prev_temp[x].cols)];
    temp_y_moins_1 = prev_temp[ x ].data[idx( z ,y-1, prev_temp[x].cols)];
    temp_y_plus_1  = prev_temp[ x ].data[idx( z ,y+1, prev_temp[x].cols)];
    temp_z_moins_1 = prev_temp[ x ].data[idx(z-1, y , prev_temp[x].cols)];
    temp_z_plus_1  = prev_temp[ x ].data[idx(z+1, y , prev_temp[x].cols)];

    //return prev_temp[x].data[idx(z, y, prev_temp[x].cols)]
    //    + D * lambda / fluid_speed * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
    //                + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
    //                + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) );
    //return 
    //    tau * (D * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
    //                + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
    //                + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) )
    //    - fluid_speed * (temp_y_plus_1 - prev_temp[x].data[idx(z, y, prev_temp[x].cols)]) / lambda)
    //    + prev_temp[x].data[idx(z, y, prev_temp[x].cols)];    
    return prev_temp[x].data[idx(z, y, prev_temp[x].cols)]
        + D * tau * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
                    + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
                    + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) );
}
*/
#endif