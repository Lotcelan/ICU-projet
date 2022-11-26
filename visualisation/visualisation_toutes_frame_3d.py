import pygame
from numpy import float64 as f64
import numpy as np
from math import floor, ceil

pygame.init()
fpsClock = pygame.time.Clock()

def str_list_to_float(lst):
    return [float(x) for x in lst]

def read_values(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data = data[0:len(data)-1]
    (nb_sub, rows, cols) = (int(data[0].split("*")[0]), int(data[0].split("*")[1]), int(data[0].split("*")[2]))
    (min_temp, max_temp) = (float(data[len(data)-1].split(";")[0]), float(data[len(data)-1].split(";")[1]))
    data = data[1:len(data)-1]
    nb = int((len(data))/(rows*nb_sub)) # nb d'itérations
    res = [] # res[i] : i e itération, res[i][j] i e itération, j e matrice, res[i][j][k] ... k e ligne, res[i][j][k][l] ... l e colonne
    for i in range(nb):
        temp = [] #liste des matrices de la ie itération
        for j in range(nb_sub):
            temp2 = []
            for k in range(rows):
                temp2.append(str_list_to_float(data[i*rows*nb_sub+j*nb_sub+k].split(";")))
            temp.append(temp2)
        res.append(temp)
    return (res, rows, cols, nb, nb_sub, min_temp, max_temp)

def get_info(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data = data[0:len(data)-1]
    (nb_sub, rows, cols) = (int(data[0].split("*")[0]), int(data[0].split("*")[1]), int(data[0].split("*")[2]))
    (min_temp, max_temp) = (float(data[len(data)-1].split(";")[0]), float(data[len(data)-1].split(";")[1]))
    data = data[1:len(data)-1]
    nb = int((len(data))/(rows*nb_sub)) # nb d'itérations
    return (data,rows, cols, nb, nb_sub, min_temp, max_temp)

def matrix_gen(data, nb, nb_sub, rows):

    for i in range(nb):
        temp = [] #liste des matrices de la ie itération
        for j in range(nb_sub):
            temp2 = []
            for k in range(rows):
                temp2.append(str_list_to_float(data[i*rows*nb_sub+j*nb_sub+k].split(";")))
            temp.append(temp2)
        yield temp

def get_col_by_temp(temp, min_temp, max_temp):
    ecart = max_temp - min_temp
    if ecart == 0:
        return (0,0,0)
    
    perc = abs(temp-min_temp)/ecart
    return ((perc)*255 % 256,0,(1-perc)*255 % 256) 
    """
    if temp != "inf" and temp != "-inf":
        return (255 * 1 / (1 + np.exp(-temp - 15)) , 0,0)
    else:
        return (0,0,0)
    """
    """
    if temp != "inf" and temp != "-inf" and temp >= -5000 and temp <= 5000:
        ecart = abs(abs(5000) + abs(-5000))
        perc = abs(temp-5000)/ecart
        return ((perc)*255 % 256,0,(1-perc)*255 % 256) 
    else:
        return (0,0,0)
    """
    

def draw_surf_element(screen, i, j, color, h, l, temperature):
    pygame.draw.rect(screen, color, pygame.Rect(i*l, j*h, l, h))

def draw_surf_element2(screen, i, j, color,h,l):
    try:
        pygame.draw.rect(screen, color, pygame.Rect(i, j, l, h))
    except:
        pygame.draw.rect(screen, (0,0,0), pygame.Rect(i, j, l, h))

def draw_i_th_matrix(screen, H, L, matrix, start_pos_x, start_pos_y, min_temp, max_temp, rows, cols):
    for i in range(rows):
        for j in range(cols):
            draw_surf_element2(screen,start_pos_x + j*L, start_pos_y + i*H,get_col_by_temp(matrix[i][j], min_temp, max_temp),H,L)

def idx(i,j,size):
    return i*size + j

def main():
    """
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - air_temp_file : fichier qui contient l'ensemble des matrices contenant les températures du fluide à la première à chaque itération
        - masses_last_fist_file : de même avec les masses de chaque subdivision de fluide
    """
    air_temp_file = "./results/air_temp_0.tipe"
    (data, rows, cols, nb, nb_sub, min_temp, max_temp) = get_info(air_temp_file)
    mat_gen = matrix_gen(data, nb, nb_sub, rows)

    screen_size_x = 1000
    screen_size_y = 1000

    FPS = 25
    nb_cols = ceil(nb_sub / 10)
    nb_rows = int(nb_sub / nb_cols)

    L = screen_size_x // (cols * nb_cols)
    H = screen_size_y // (rows * nb_rows)

    screen = pygame.display.set_mode([screen_size_x, screen_size_y])

    running = True
    frame = 0

    

    while (running and frame < nb):
        pygame.display.set_caption(f"{frame=}/{nb}")
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        i = 0
        mat_bloc = next(mat_gen)
        for mat in mat_bloc:
            draw_i_th_matrix(screen, H, L, mat, (i%nb_cols)*L*cols, (i//nb_cols)*H*rows,min_temp, max_temp, rows, cols)
            i+=1
        frame += 1
        
        print(frame)

        pygame.display.flip()
        fpsClock.tick(FPS)
        
    pygame.quit()

if __name__ == "__main__":
    main()