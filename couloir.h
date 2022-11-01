#ifndef COULOIR
#define COULOIR

typedef struct f_matrix
{
    int cols; // Ce sera normalement toujours n
    int rows; // Ce sera normalement toujours n
    float* data; //1 dim float array
} f_matrix;

typedef struct surface
{
    float width;
    float length;

} surface;

typedef struct surface_temp
{
    surface surf;
    float temp;
} surface_temp;

typedef struct s_t_matrix
{
    int rows; // Ce sera normalement toujours n
    int cols; // Ce sera normalement toujours n
    surface_temp* data; //1 dim surface_temp array
} s_t_matrix;

typedef struct idx_couple
{
    int fst;
    int snd;
} idx_couple;

int idx(int i, int j, int size) { return i*size + j; }

float calculer_T_floor(int i, int j, float lambda, float mu, float tau, surface_temp s_t, f_matrix* air_temp, float K, float m_i, float Vitesse_air) {
    if (j > 0 && (j%air_temp->cols != 0)) {
        return (Vitesse_air/3.6*air_temp[i].data[idx(air_temp[i].rows-1, j-1, air_temp[i].cols)]/lambda + air_temp[i].data[idx(air_temp[i].rows-1, j, air_temp[i].cols)]/tau - K/m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp)/(Vitesse_air/3.6/lambda + 1/tau - K*(s_t.surf.length * s_t.surf.width)/m_i);
    } else {
        return (Vitesse_air/3.6*air_temp[i].data[idx(air_temp[i].rows-1, j, air_temp[i].cols)]/lambda + air_temp[i].data[idx(air_temp[i].rows-1, j, air_temp[i].cols)]/tau - K/m_i * (s_t.surf.length * s_t.surf.width) * s_t.temp)/(Vitesse_air/3.6/lambda + 1/tau - K*(s_t.surf.length * s_t.surf.width)/m_i);
    }

}

float calculer_T_air(int x, int y, int z, float lambda, float mu, float h_n, int nb_subd, float tau, f_matrix* prev_temp, float D) {

    float temp_x_plus_1, temp_x_moins_1, temp_y_plus_1, temp_y_moins_1, temp_z_plus_1, temp_z_moins_1;
    /*
    if (x > 0) { temp_x_moins_1 = prev_temp[x-1].data[idx(y,z,prev_temp[x].cols)];} else { temp_x_moins_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    if (x < nb_subd-1) { temp_x_plus_1 = prev_temp[x+1].data[idx(y,z,prev_temp[x].cols)];} else { temp_x_plus_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    if (y > 0) { temp_y_moins_1 = prev_temp[x].data[idx(y-1,z,prev_temp[x].cols)];} else { temp_y_moins_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    if (y < prev_temp[x].cols) { temp_y_plus_1 = prev_temp[x].data[idx(y+1,z,prev_temp[x].cols)];} else { temp_y_plus_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    if (z > 0) { temp_z_moins_1 = prev_temp[x].data[idx(y,z-1,prev_temp[x].cols)];} else { temp_z_moins_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    if (z < prev_temp[x].rows) { temp_z_plus_1 = prev_temp[x].data[idx(y,z+1,prev_temp[x].cols)];} else { temp_z_plus_1 = prev_temp[x].data[idx(y,z,prev_temp[x].cols)];}
    */
   if (x > 0) { temp_x_moins_1 = prev_temp[x-1].data[idx(z,y,prev_temp[x].cols)];} else { temp_x_moins_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (x < nb_subd-1) { temp_x_plus_1 = prev_temp[x+1].data[idx(z,y,prev_temp[x].cols)];} else { temp_x_plus_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (y > 0) { temp_y_moins_1 = prev_temp[x].data[idx(z,y-1,prev_temp[x].cols)];} else { temp_y_moins_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (y < prev_temp[x].cols) { temp_y_plus_1 = prev_temp[x].data[idx(z,y+1,prev_temp[x].cols)];} else { temp_y_plus_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (z > 0) { temp_z_moins_1 = prev_temp[x].data[idx(z-1,y,prev_temp[x].cols)];} else { temp_z_moins_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    if (z < prev_temp[x].rows) { temp_z_plus_1 = prev_temp[x].data[idx(z+1,y,prev_temp[x].cols)];} else { temp_z_plus_1 = prev_temp[x].data[idx(z,y,prev_temp[x].cols)];}
    
    return prev_temp[x].data[idx(z,y,prev_temp[x].cols)] + D * tau * ( (temp_x_plus_1 - 2 * prev_temp[x].data[idx(z,y,prev_temp[x].cols)] + temp_x_moins_1 ) / (mu * mu) + (temp_y_plus_1 - 2 * prev_temp[x].data[idx(z,y,prev_temp[x].cols)] + temp_y_moins_1 ) / (lambda * lambda) + (temp_z_plus_1 - 2 * prev_temp[x].data[idx(z,y,prev_temp[x].cols)] + temp_z_moins_1 ) / (h_n* h_n) );


    /*
    float A_m_j = 0.0;
    if (m > 0) { A_m_j += prev_temp[i].data[idx(m-1,j,prev_temp[i].cols)]; } else { A_m_j += prev_temp[i].data[idx(m,j,prev_temp[i].cols)]; }
    if (m < prev_temp[i].rows) { A_m_j += prev_temp[i].data[idx(m+1,j,prev_temp[i].cols)]; } else { A_m_j += prev_temp[i].data[idx(m,j,prev_temp[i].cols)]; }
    if (j > 0) { A_m_j += prev_temp[i].data[idx(i, j-1 ,prev_temp[i].cols)]; } else { A_m_j += prev_temp[i].data[idx(i,j,prev_temp[i].cols)]; }
    if (j < prev_temp[i].cols) { A_m_j += prev_temp[i].data[idx(m,j+1,prev_temp[i].cols)]; } else { A_m_j += prev_temp[i].data[idx(m,j,prev_temp[i].cols)]; }

    return tau * ( D * ( A_m_j - 4 * prev_temp[i].data[idx(m,j,prev_temp[i].cols)] ) / lambda ) + prev_temp[i].data[idx(m,j,prev_temp[i].cols)];
    */

}

#endif