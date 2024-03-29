import pygame, sys
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
    (rows, cols) = (int(data[0].split("*")[0]), int(data[0].split("*")[1]))
    data = data[1:len(data)]
    nb = int((len(data))/(rows)) # nb d'itérations
    res = [] # res[i] : i e itération, res[i][j] i e itération, res[i][j] ... j e ligne, res[i][j][k] ... k e colonne
    for i in range(nb):
        
        temp = []
        for k in range(rows):
            temp.append(str_list_to_float(data[i*rows+k].split(";")))
        res.append(temp)
        
    return (res, rows, cols, nb)

def get_info(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data = data[0:len(data)-1]
    (rows, cols) = (int(data[0].split("*")[0]), int(data[0].split("*")[1]))
    data = data[1:len(data)]
    nb = int((len(data))/(rows)) # nb d'itérations
    return (data,rows, cols, nb)

def matrix_gen(data, nb, rows):

    for i in range(nb):
        temp = [] #liste des matrices de la ie itération
    
        for k in range(rows):
            temp.append(str_list_to_float(data[i*rows+k].split(";")))
        yield temp

def get_col_by_temp(temp, min_temp, max_temp):
    ecart = max_temp - min_temp
    if ecart == 0:
        return (0,0,0)
    
    perc = abs(temp-min_temp)/ecart
    return ((perc)*255 % 256,0,(1-perc)*255 % 256) 


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

def get_local_min_max(mat, rows, cols):
    local_min = mat[0][0]
    local_max = mat[0][0]
    for i in range(rows):
        for j in range(cols):
            if mat[i][j] < local_min:
                local_min = mat[i][j] 
            if mat[i][j]  > local_max:
                local_max = mat[i][j]
    return local_min, local_max

def main(args):
    """
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - air_temp_file : fichier qui contient l'ensemble des matrices contenant les températures du fluide à la première à chaque itération
        - masses_last_fist_file : de même avec les masses de chaque subdivision de fluide
    """
    nb_frames_per_display = 1
    adaptive_colors = False

    if (a := "-sf" in args) or (b := "--skip-frames" in args):
        i = args.index("-sf" if a else "--skip-frames")
        try:
            value = int(args[i+1])  
            nb_frames_per_display = value
        except:
            print("Erreur, entrez une valeur cohérente")
    if (a := "-ac" in args) or (b := "--adaptive-colors" in args):
        i = args.index("-ac" if a else "--adaptive-colors")
        try:
            value = bool(int(args[i+1]))  
            print(f"Adaptive colors : {value}, arg : {args[i+1]=}")
            adaptive_colors = value 
        except:
            print("Erreur, entrez une valeur cohérente")
    
    adaptive_colors = True # Pour l'instant

    air_temp_file = "./results/floor_temp_0.tipe"
    (data, rows, cols, nb, ) = get_info(air_temp_file)
    mat_gen = matrix_gen(data, nb, rows)

    screen_size_x = 1000
    screen_size_y = 1000

    FPS = 25

    L = screen_size_x // (cols)
    H = screen_size_y // (rows)

    screen = pygame.display.set_mode([screen_size_x, screen_size_y])

    running = True
    frame = 0

    should_pause = False

    while (running and frame < nb):
        pygame.display.set_caption(f"{frame=}/{nb}")

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE:
                    should_pause = not should_pause

        i = 0
        if not (should_pause):
            for _ in range(nb_frames_per_display):
                mat = next(mat_gen)
                if adaptive_colors:
                    local_min, local_max = get_local_min_max(mat, rows, cols)
                    for i in range(rows):
                        for j in range(cols):
                            draw_surf_element2(screen,j*L, i*H,get_col_by_temp(mat[i][j], local_min, local_max),H,L)
                else:
                    print("CA NE DEVRAIT PAS ARRIVER ICI ALED")
                    draw_i_th_matrix(screen, H, L, mat, (i%nb_cols)*L*cols, (i//nb_cols)*H*rows,min_temp, max_temp, rows, cols)
                i+=1
            frame += nb_frames_per_display
        
            print(frame)

        pygame.display.flip()
        fpsClock.tick(FPS)
        
    pygame.quit()

if __name__ == "__main__":
    args = sys.argv
    main(args)