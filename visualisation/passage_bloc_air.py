import pyray as pr
"""
Différences entre raylib pyhton/C :
the function names are in snake_case.
Some string and pointer conversions are handled automatically.
There are some helper functions to create structures.
"""

def str_list_to_float(lst, modifier=0):
    return [float(x) + modifier for x in lst]

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

def create_mat(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    if data == []:
        return []
    nb_rows = len(data)
    res = []
    for i in range(nb_rows):
        if data[i] != '':
            res.append(str_list_to_float(data[i].split(" "), -273))

    return res

def matrix_gen(data, nb, nb_sub, rows):
    for i in range(nb):
        temp = [] #liste des matrices de la ie itération
        for j in range(nb_sub):
            temp2 = []
            for k in range(rows):
                temp2.append(str_list_to_float(data[i*rows*nb_sub+j*nb_sub+k].split(";")))
            temp.append(temp2)
        yield temp

def read_config_values(floor_temp_file, left_wall_temp_file, right_wall_temp_file):
    return create_mat(floor_temp_file), create_mat(left_wall_temp_file), create_mat(right_wall_temp_file)


def get_surf_col_by_temp(temp, min_temp, max_temp):
    if temp > max_temp:
        temp = max_temp
    if temp < min_temp:
        temp = min_temp
    ecart = max_temp - min_temp
    if ecart == 0:
        return (0,0,0)
    
    perc = abs(temp-min_temp)/ecart
    return ((perc)*255 % 256,0,(1-perc)*255 % 256) 

def get_col_by_temp(temp, min_temp, max_temp):

    ecart = max_temp - min_temp
    if ecart == 0:
        return (0,0,0)
    
    perc = abs(temp-min_temp)/ecart
    return ((perc)*255 % 256,0,(1-perc)*255 % 256) 

def main():

    window_length = 1500
    window_height = 1000

    air_temp_file = "./results/air_temp_0.tipe"
    (data, rows, cols, nb, nb_sub, min_temp, max_temp) = get_info(air_temp_file)
    #rows = 50
    #cols = 50
    #nb = 2048
    #nb_sub = 50
    #min_temp = 15.0
    #max_temp = 75
    mat_gen = matrix_gen(data, nb, nb_sub, rows)
    floor_temp, left_wall_temp, right_wall_temp = read_config_values("./config/floor_temp_0.conf", "./config/left_wall_temp_0.conf", "./config/right_wall_temp_0.conf")
    pr.init_window(window_length, window_height, "3D Air")
    pr.set_target_fps(60)

    camera = pr.Camera3D([nb_sub + 50, nb_sub + 80, nb_sub + 50], [0.0, 25.0, 0.0], [0.0, 1.0, 0.0], 45.0, 0)
    pr.set_camera_mode(camera, pr.CAMERA_FREE) #CAMERA_ORBITAL

    # https://github.com/raysan5/raylib/blob/master/examples/core/core_split_screen.c
    #vp1 = [0                                     , 0                                     , window_length // 2, window_height // 2] # En haut à gauche
    #vp2 = [window_length // 2                    , 0                                     , window_length     , window_height // 2] # En haut à droite
    #vp3 = [0                                     , window_height // 2                    , window_length // 2, window_height     ] # En bas à gauche
    #vp4 = [window_length // 2, window_height // 2, window_length // 2, window_height // 2, window_length     , window_height     ] # En bas à droite


    nb_frames_per_display = 1

    fluid_start_X = -cols - cols // 2
    fluid_start_Z = -rows // 2

    while not pr.window_should_close():
        
        for _ in range(nb_frames_per_display):
            mat_bloc = next(mat_gen)
        pr.update_camera(camera)

        pr.begin_drawing()
        pr.clear_background(pr.RAYWHITE)

        pr.begin_mode_3d(camera)
        pr.draw_grid(nb_sub, 1)
        
        # Progression de l'air selon l'axe x
        left_wall_start_X = -cols // 2
        left_wall_start_Z = -rows // 2 - 1

        # DRAW LEFT WALL
        for i in range(rows):
            for j in range(cols):
                new_col = get_surf_col_by_temp(left_wall_temp[rows-1-i][j], min_temp, max_temp)
                pr.draw_cube((left_wall_start_X + j, i, left_wall_start_Z), 1, 1, 1, pr.Color(int(new_col[0]), int(new_col[1]), int(new_col[2]), 15))

        # DRAW RIGHT WALL
        #right_wall_start_X = -cols // 2
        #right_wall_start_Z = rows // 2 + 1
        #for i in range(rows):
        #    for j in range(cols):
        #        new_col = get_surf_col_by_temp(right_wall_temp[rows-1-i][j], min_temp, max_temp)
        #        pr.draw_cube((right_wall_start_X + j, i, right_wall_start_Z), 1, 1, 1, pr.Color(int(new_col[0]), int(new_col[1]), int(new_col[2]), 15))

        floor_start_X = - cols // 2
        floor_start_Z = - rows // 2
        # DRAW FLOOR
        for i in range(rows):
            for j in range(cols):
                new_col = get_surf_col_by_temp(floor_temp[i][j], min_temp, max_temp)
                pr.draw_cube((floor_start_X + j, -1, floor_start_Z + i), 1, 1, 1, pr.Color(int(new_col[0]), int(new_col[1]), int(new_col[2]), 15))

        # DRAW FLUID
        offset = 0
        for mat in mat_bloc:
            for i in range(rows):
                for j in range(cols):
                    new_col = get_col_by_temp(mat[rows-1-i][j], min_temp, max_temp)
                    pr.draw_cube((fluid_start_X + j, i, fluid_start_Z + offset), 1, 1, 1, pr.Color(int(new_col[0]), int(new_col[1]), int(new_col[2]), 255))
            offset += 1
        if fluid_start_X < cols // 2:
            fluid_start_X += 1

        pr.end_mode_3d()

        pr.end_drawing()
    #    pr.begin_mode_3d(camera)
    #    pr.draw_grid(20, 1.0)
    #    pr.end_mode_3d()
    #    pr.end_drawing()
    #pr.close_window()

if __name__ == "__main__":
    main()
    