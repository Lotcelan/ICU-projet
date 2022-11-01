import pygame, time
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
                
            #print(*data[i*rows+j+1].split(";"))
                temp2.append(str_list_to_float(data[i*rows*nb_sub+j*nb_sub+k].split(";")))
            
            temp.append(temp2)
        res.append(temp)
    return (res, rows, cols, nb, nb_sub, min_temp, max_temp)


test =  {}
import random
def get_col_by_temp(temp, min_temp, max_temp):
    #e = abs(max_temp - min_temp) / 100
    #perc = abs(abs(temp) - abs(min_temp)) / e
    #ecart = abs(max_temp) + abs(min_temp)
    #perc = abs(temp-min_temp)/ecart #*100
    ecart = abs(abs(max_temp) - abs(min_temp))
    perc = abs(temp-min_temp)/ecart
    return ((perc)*255 % 256,0,(1-perc)*255 % 256) 
    """
    try:
        return test[temp]
    except:
        test[temp] = (random.randint(0,255), random.randint(0,255), random.randint(0,255))
        return test[temp]
    """

def draw_surf_element(screen, i, j, color, h, l, temperature):
    pygame.draw.rect(screen, color, pygame.Rect(i*l, j*h, l, h))

def draw_surf_element2(screen, i, j, color,h,l):
    #print(f"Drawing rect : {i=} , {j=} , {color=} , {h=} , {l=}")
    try:
        pygame.draw.rect(screen, color, pygame.Rect(i, j, l, h))
    except:
        pygame.draw.rect(screen, (0,0,0), pygame.Rect(i, j, l, h))

def draw_i_th_matrix(screen, H, L, matrix, start_pos_x, start_pos_y, min_temp, max_temp, rows, cols):
    for i in range(rows):
        for j in range(cols):
            draw_surf_element2(screen,start_pos_x + j*L, start_pos_y + i*H,get_col_by_temp(matrix[i][j], min_temp, max_temp),H,L)
 
"""
def main():
    (matrices, rows, cols, nb, nb_sub, min_temp, max_temp) = read_values("../v2/wind_data.tipe")
    
    #(surfaces, rows, cols, nb) = read_values("../v1/surf_data.tipe")
    FPS = int(nb*2)

    L = 1000//(cols)
    H = 1000//(rows)

    screen = pygame.display.set_mode([1000, 1000])

    #screen.fill((0, 0, 0))

    running = True
    for subd in range(nb_sub):
        frame = 0
        while (running and frame < nb):
            
            pygame.display.set_caption(f"Matrice : {subd} / {nb_sub} - Frame : {frame} / {nb}")

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

            #for i in range(nb):
            for i in range(rows):
                for j in range(cols):
                    draw_surf_element(screen,j,i,get_col_by_temp(matrices[frame][subd][i][j], min_temp, max_temp),H,L,matrices[frame][subd][i][j])

            frame += 1
            pygame.display.flip()
            fpsClock.tick(FPS)
            




            pygame.display.flip()
            fpsClock.tick(FPS)


    pygame.quit()
"""

def idx(i,j,size):
    return i*size + j

def main():
    """
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - air_temp_file : fichier qui contient l'ensemble des matrices contenant les températures du fluide à la première à chaque itération
        - masses_last_fist_file : de même avec les masses de chaque subdivision de fluide
    """
    air_temp_file = "./air_temp.tipe"
    (matrices, rows, cols, nb, nb_sub, min_temp, max_temp) = read_values(air_temp_file)
    
    #(surfaces, rows, cols, nb) = read_values("../v1/surf_data.tipe")
    FPS = 25
    nb_cols = int(nb_sub/10)
    nb_rows = int(nb_sub / nb_cols)

    L = 1000//(cols*nb_cols)
    H = 1000//(rows*nb_rows)

    screen = pygame.display.set_mode([1000, 1000])

    #screen.fill((0, 0, 0))

    running = True
    frame = 0
    while (running and frame < nb):
        pygame.display.set_caption(f"{frame=}")
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        #for i in range(nb):
        for i in range(nb_rows):
            for j in range(nb_cols):
                draw_i_th_matrix(screen, H, L, matrices[frame][idx(i,j,nb_cols)], j*L*cols, i*H*rows,min_temp, max_temp, rows, cols)
                #print(f"{frame=} - {i=} - {j=} - {idx(i,j,nb_cols)=} - {i*nb_rows*H=} - {j*nb_cols*L=}")
        frame += 1
        print(frame)
        pygame.display.flip()
        fpsClock.tick(FPS)
        

    pygame.quit()

if __name__ == "__main__":
    main()