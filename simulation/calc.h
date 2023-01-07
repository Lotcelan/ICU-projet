#ifndef CALC
#define CALC

#include "def.h"

double floor_temp_calc(int i, int j, double lambda, double mu, double tau, surface_temp s_t, f_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
    if (j > 0 && (j % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j - 1, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }   
}

double air_temp_calc_ray(int x, int y, int z, double tau, double c_p, double m_i, double prev_temp, double radiation_absorbee) {
    return prev_temp + radiation_absorbee * tau / c_p / m_i;
}

//double floor_temp_calc_ray(int i, int j, double lambda, double mu, double tau, surface_temp s_t, f_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
//    double ray_solaire_surf = 341; //1230 * exp( -1 / (3.8 * sin(3.14 / 180 * (10 + 1.6))));
//    if (j > 0 && (j % air_temp->cols != 0)) {
//        return (0.2 * ray_solaire_surf * s_t.surf.length * s_t.surf.width / m_i / c_p + fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j - 1, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
//    } else {
//        return (0.2 * ray_solaire_surf * s_t.surf.length * s_t.surf.width / m_i / c_p + fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
//    }   
//}

double wall_temp_calc(int i, int j, int wall_idx, double lambda, double mu, double tau, surface_temp s_t, f_matrix* air_temp, double c_p, double m_i, double fluid_speed) {
    if (j > 0 && (j % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * air_temp[wall_idx].data[idx(i, j - 1, air_temp[wall_idx].cols)] / lambda + air_temp[wall_idx].data[idx(i, j, air_temp[wall_idx].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * air_temp[wall_idx].data[idx(i, j, air_temp[wall_idx].cols)] / lambda + air_temp[wall_idx].data[idx(i, j, air_temp[wall_idx].cols)] / tau + (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau + (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }
}

double air_temp_calc_args(int x, int y, int z, double lambda, double mu, double h_n, int nb_subd, double tau, f_matrix* prev_temp, double D, double fluid_speed, double temp_x_plus_1, double temp_x_moins_1, double temp_y_plus_1, double temp_y_moins_1, double temp_z_plus_1, double temp_z_moins_1) {

    return prev_temp[x].data[idx(z, y, prev_temp[x].cols)]
        + D * tau * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
                    + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
                    + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) );
    //return 
    //    tau * (D * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu)
    //                + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda)
    //                + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n * h_n) )
    //    - fluid_speed / 3.6  * (temp_y_plus_1 - prev_temp[x].data[idx(z, y, prev_temp[x].cols)]) / lambda)
    //    + prev_temp[x].data[idx(z, y, prev_temp[x].cols)];   
}

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

#endif