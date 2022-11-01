def str_list_to_float(lst):
    return [float(x) for x in lst]

def read_first_last_matrix(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data = data[0:len(data)-1]
    (nb_sub, rows, cols) = (int(data[0].split("*")[0]), int(data[0].split("*")[1]), int(data[0].split("*")[2]))
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
    return (res[0], res[1], rows, cols, nb_sub)

def main():
    """
    Objectif : calculer la variation d'enthalpie massique engendrée par la variation de température du fluide
    Paramètres :
        - air_temp_last_fist_file : fichier qui contient l'ensemble des matrices contenant les températures du fluide à la première et dernière itération
        - masses_last_fist_file : de même avec les masses de chaque subdivision de fluide
    """

    air_temp_last_fist_file = "./air_temp_last_first.tipe"
    masses_last_fist_file = "./masses_last_first.tipe"

    (mat_0, mat_end, rows, cols, nb_sub) = read_first_last_matrix(air_temp_last_fist_file)
    (masses_0, masses_end, _, _, _) = read_first_last_matrix(masses_last_fist_file)
    h_tot = 0
    for i in range(nb_sub):
        for j in range(rows):
            for k in range(cols):
                h_tot += masses_0[i][j][k] * 1256.0 * (mat_end[i][j][k] - mat_0[i][j][k])
    print(h_tot)

if __name__ == "__main__":
    main()