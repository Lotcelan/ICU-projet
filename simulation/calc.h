#ifndef CALC
#define CALC

#include "utils.h"

float floor_temp_calc(int i, int j, float lambda, float mu, float tau, surface_temp s_t, f_matrix* air_temp, float c_p, float m_i, float fluid_speed) {
    if (j > 0 && (j % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j - 1, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau - (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau - (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / lambda + air_temp[i].data[idx(air_temp[i].rows - 1, j, air_temp[i].cols)] / tau - (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau - (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }
}

float wall_temp_calc(int i, int j, int left_right, float lambda, float mu, float tau, surface_temp s_t, f_matrix* air_temp, float c_p, float m_i, float fluid_speed) {
    if (j > 0 && (j % air_temp->cols != 0)) {
        return (fluid_speed / 3.6 * air_temp[left_right].data[idx(i, j - 1, air_temp[left_right].cols)] / lambda + air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)] / tau - (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau - (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    } else {
        return (fluid_speed / 3.6 * air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)] / lambda + air_temp[left_right].data[idx(i, j, air_temp[left_right].cols)] / tau - (s_t.h / c_p) / m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp) / (fluid_speed / 3.6 / lambda + 1 / tau - (s_t.h / c_p) * (s_t.surf.length * s_t.surf.width) / m_i);
    }
}

float air_temp_calc(int x, int y, int z, float lambda, float mu, float h_n, int nb_subd, float tau, f_matrix* prev_temp, float D) {

    float temp_x_plus_1, temp_x_moins_1, temp_y_plus_1, temp_y_moins_1, temp_z_plus_1, temp_z_moins_1; // Représentent à l'itération précédente les températures décalées de + ou - 1 selon x, y ou z

    if (x > 0) { temp_x_moins_1 = prev_temp[x - 1].data[idx(z, y, prev_temp[x].cols)]; } else { temp_x_moins_1 = prev_temp[x].data[idx(z, y, prev_temp[x].cols)]; }
    if (x < nb_subd - 1) { temp_x_plus_1 = prev_temp[x + 1].data[idx(z, y, prev_temp[x].cols)]; } else { temp_x_plus_1 = prev_temp[x].data[idx(z, y, prev_temp[x].cols)]; }
    if (y > 0) { temp_y_moins_1 = prev_temp[x].data[idx(z,y-1,prev_temp[x].cols)]; } else { temp_y_moins_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (y < prev_temp[x].cols) { temp_y_plus_1 = prev_temp[x].data[idx(z, y + 1, prev_temp[x].cols)]; } else { temp_y_plus_1 = prev_temp[x].data[idx(z, y, prev_temp[x].cols)]; }
    if (z > 0) { temp_z_moins_1 = prev_temp[x].data[idx(z - 1, y, prev_temp[x].cols)];} else { temp_z_moins_1 = prev_temp[x].data[idx(z, y, prev_temp[x].cols)]; }
    if (z < prev_temp[x].rows) { temp_z_plus_1 = prev_temp[x].data[idx(z + 1, y, prev_temp[x].cols)];}  else { temp_z_plus_1 = prev_temp[x].data[idx(z, y, prev_temp[x].cols)]; }
    
    return prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + D * tau * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu) + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda) + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z, y, prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n* h_n) );
}

#endif