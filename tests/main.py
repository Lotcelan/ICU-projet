import os, threading, subprocess

def create_test(T_e, Vitesse_air, Volume_air, L, l, n, c_p, D, config_l_wall_temp, config_floor_temp, config_r_wall_temp, config_l_wall_h, config_floor_h, config_r_wall_h, continuer_meme_si_fini, nb_it_supp, save_air_temp_filename, air_temp_last_first_file, masses_last_first_file, id):
    res = {}
    res["T_e"] = T_e
    res["Vitesse_air"] = Vitesse_air
    res["Volume_air"] = Volume_air
    res["L"] = L
    res["l"] = l
    res["n"] = n
    res["c_p"] = c_p
    res["D"] = D
    res["config_l_wall_temp"] = config_l_wall_temp
    res["config_floor_temp"] = config_floor_temp
    res["config_r_wall_temp"] = config_r_wall_temp
    res["config_l_wall_h"] = config_l_wall_h
    res["config_floor_h"] = config_floor_h
    res["config_r_wall_h"] = config_r_wall_h
    res["continuer_meme_si_fini"] = int(continuer_meme_si_fini)
    res["nb_it_supp"] = nb_it_supp
    res["save_air_temp_filename"] = save_air_temp_filename
    res["air_temp_last_first_file"] = air_temp_last_first_file
    res["masses_last_first_file"] = masses_last_first_file
    res["id"] = id

    return res

def run_test(t):
    cmd = f"../simulation/main {t['T_e']} {t['Vitesse_air']} {t['Volume_air']} {t['L']} {t['l']} {t['n']} {t['c_p']} {t['D']} {t['config_l_wall_temp']} {t['config_floor_temp']} {t['config_r_wall_temp']} {t['config_l_wall_h']} {t['config_floor_h']} {t['config_r_wall_h']} {t['continuer_meme_si_fini']} {t['nb_it_supp']} {t['save_air_temp_filename']} {t['air_temp_last_first_file']} {t['masses_last_first_file']} 1 0 {t['id']}" # le dernier 1 est pour le mode flask et le 0 pour print_to_file
    print(cmd)
    os.system(cmd) 

def create_custom_config(confs, names, id):
    for (c, n) in list(zip(confs,names)):
        subprocess.call(["python", "gen_config.py", c[0], (c[1] + n.format(id))])

def read_user_conf():
    f = open("../config/user_config.txt","r")
    data = f.read()
    f.close()
    data = data.split("\n")
    data =  [line for line in data if line != '']
    res = []
    count = 0
    while count < len(data):
        temp = []
        for line in data[count:count+6]:
            temp.append([line.split("|")[0], line.split("|")[1]])
            count += 1
        res.append(temp)

    return res
            



    return data

def main():
    """
        Fichier qui permet de faire des batteries de tests. Utilisation :
            - Veiller à ce que le fichier 'tests.txt' soit bien présent dans results
            - Régler ci-dessous les paramètres que l'on veut tester en créant des listes avec les valeur possibles, puis en loopant dessus pour créer une liste 'tests' de tests
            - Régler le nb de thread voulue (permet d'aller plus vite au prix d'une plus grande utilisation du CPU)
            - Puis cf README pour la méthode pour lancer les tests*

        Format de user_config.txt :
            - Sur chaque ligne (6 lignes/simulation (pour chaque température de murs/sol et chaque h de murs/sol)) :
                - [all/custom]|[arguments (cf gen_config.py) sans le nom de fichier à la fin]
    """

    tests = []
    names = ["../config/left_wall_temp_{}.conf", "../config/floor_temp_{}.conf", "../config/right_wall_temp_{}.conf", "../config/left_wall_h_{}.conf", "../config/floor_h_{}.conf", "../config/right_wall_h_{}.conf"]
    # A CHANGER
    nb_threads = 8

    nb_subdivision = 50

    vitesses = [5,15]
    volumes = [2,7.5]
    L_l = [0.5,5]
    D = [0.03,0.05,0.1]

    configs = read_user_conf()

    id = 0
    for conf in configs:
        create_custom_config(conf,names,id)
        for vit in vitesses:
            for vol in volumes:
                for l in L_l:
                    for d in D:
                        tests.append(create_test(288,vit,vol,l,l,nb_subdivision,1256,d,names[0].format(id),names[1].format(id), names[2].format(id), names[3].format(id), names[4].format(id), names[5].format(id), False,10,"../results/air_temp.tipe","../results/air_temp_last_first.tipe","../results/masses_temp_last_first.tipe", id))
        id += 1
    curr = 0
    nb_tests = len(tests)

    while nb_tests-curr > 0:
        threads = []
        
        while threading.active_count() <= nb_threads + 1 and curr < nb_tests:
            new = threading.Thread(target=run_test, args=(tests[curr],))
            new.start()
            threads.append(new)
            curr += 1

        for th in threads:
            th.join()

if __name__ == "__main__":
    main()