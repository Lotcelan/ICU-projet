import matplotlib
import matplotlib.pyplot as plt
matplotlib.use("GTK4Agg")

def str_list_to_float(lst):
    return [float(x) for x in lst]

def read_first_last_matrix(filename, read_meta_data=False):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data = data[0:len(data)-1]
    first_line = data[0].split("*")
    if read_meta_data:
        (nb_sub, rows, cols, T_e, Vitesse_air, Volume_air, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall) = (int(first_line[0]), int(first_line[1]), int(first_line[2]), float(first_line[3]), float(first_line[4]), float(first_line[5]), float(first_line[6]), float(first_line[7]), float(first_line[8]), float(first_line[9]), float(first_line[10]), float(first_line[11]), float(first_line[12]))
    else:
        (nb_sub, rows, cols) = (int(first_line[0]), int(first_line[1]), int(first_line[2]))
    data = data[1:len(data)]
    nb = int((len(data))/(rows*nb_sub)) # normalement 2
    res = []
    for i in range(nb):
        temp = [] #liste des matrices de la ie itération
        for j in range(nb_sub):
            temp2 = []
            for k in range(rows):
                temp2.append(str_list_to_float(data[i*rows*nb_sub+j*nb_sub+k].split(";")))
            temp.append(temp2)
        res.append(temp)
    if read_meta_data:
        return (res[0], res[1], rows, cols, nb_sub, T_e, Vitesse_air, Volume_air, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall)
    else:
        return (res[0], res[1], rows, cols, nb_sub)

def enthalpy_calc(air_temp_last_first_file="./results/air_temp_last_first.tipe", masses_last_first_file="./results/masses_last_first.tipe"):
    """
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - air_temp_last_first_file : fichier qui contient l'ensemble des matrices contenant les températures du fluide à la première et dernière itération
        - masses_last_first_file : de même avec les masses de chaque subdivision de fluide
    Valeur de retour:
        - l'enthalpie totale fournie/reçue (selon le signe) au/du gaz
    """

    (mat_0, mat_end, rows, cols, nb_sub,  T_e, Vitesse_air, Volume_air, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall) = read_first_last_matrix(air_temp_last_first_file, True)
    (masses_0, masses_end, _, _, _) = read_first_last_matrix(masses_last_first_file)
    h_tot = 0
    for i in range(nb_sub):
        for j in range(rows):
            for k in range(cols):
                h_tot += masses_0[i][j][k] * 1256.0 * (mat_end[i][j][k] - mat_0[i][j][k])
    return (h_tot, nb_sub, T_e, Vitesse_air, Volume_air, L, l, c_p, D, offset_floor, offset_l_wall, offset_r_wall)

def acquire_data(list_files):
    enthalpies = []
    values = []
    for f in list_files:
        temp = enthalpy_calc(f[0],f[1])
        enthalpies.append(temp[0])
        values.append(temp[1:])
    return enthalpies, values

def value_to_str(value):
    parametres =  {"nb_sub":0, "T_e":1, "Vit_air":2, "Vol_air":3, "L":4, "l":5, "c_pww":6, "D":7, "offset_floor":8, "offset_l_wall":9, "offset_r_walll":10}
    res = ""
    wanted = ["offset_floor"]
    for i in range(len(wanted)):
        if i == len(wanted)-1:
            res += f"{wanted[i]} = {value[parametres[wanted[i]]]}"
        else:
            res += f"{wanted[i]} = {value[parametres[wanted[i]]]};"
    return res

def main():
    # Pour plus tard https://matplotlib.org/stable/gallery/subplots_axes_and_figures/subplots_demo.html

    enthalpies, values = acquire_data([(f"./results/air_temp_last_first_{i}.tipe",f"./results/masses_last_first_{i}.tipe") for i in range(0,1)])
    fig, ax = plt.subplots()
    
    bars = ax.bar([value_to_str(v) for v in values], enthalpies)

    ax.bar_label(bars)

    plt.xticks(rotation=-35)
    plt.xlabel("Différentes configurations")
    plt.ylabel("Variation d'enthalpie en $(J)$")

    plt.grid(True)

    plt.show()

if __name__ == "__main__":
    main()