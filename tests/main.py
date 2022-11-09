import os

def create_test(T_e, Vitesse_air, Volume_air, L, l, n, c_p, D, offset_floor, offset_l_wall, offset_r_wall, continuer_meme_si_fini, nb_it_supp, save_air_temp_filename, air_temp_last_first_file, masses_last_first_file):
    res = {}
    res["T_e"] = T_e
    res["Vitesse_air"] = Vitesse_air
    res["Volume_air"] = Volume_air
    res["L"] = L
    res["l"] = l
    res["n"] = n
    res["c_p"] = c_p
    res["D"] = D
    res["offset_floor"] = offset_floor
    res["offset_l_wall"] = offset_l_wall
    res["offset_r_wall"] = offset_r_wall
    res["continuer_meme_si_fini"] = int(continuer_meme_si_fini)
    res["nb_it_supp"] = nb_it_supp
    res["save_air_temp_filename"] = save_air_temp_filename
    res["air_temp_last_first_file"] = air_temp_last_first_file
    res["masses_last_first_file"] = masses_last_first_file

    return res

def run_test(t):
    
    #os.system("./run.sh &")
    os.system(f"../simulation/main {t['T_e']} {t['Vitesse_air']} {t['Volume_air']} {t['L']} {t['l']} {t['n']} {t['c_p']} {t['D']} {t['offset_floor']} {t['offset_l_wall']} {t['offset_r_wall']} {t['continuer_meme_si_fini']} {t['nb_it_supp']} {t['save_air_temp_filename']} {t['air_temp_last_first_file']} {t['masses_last_first_file']} 1 0") # le dernier 1 est pour le mode flask


def main():
    tests = []

    vitesses = [1,5,10,20,30,50]
    volumes = [0.5,1,3,5,7.5,15,20]
    L_l = [0.5,1,3,5,7.5,15,20]
    D = [0.01,0.03,0.176,0.5,1]

    for vit in vitesses:
        for vol in volumes:
            for l in L_l:
                for d in D:
                    tests.append(create_test(288,vit,vol,l,l,50,1256,d,10,10,10,False,10,"air_temp.tipe","air_temp_last_first.tipe","masses_temp_last_first.tipe")
)

    for t in tests:
        run_test(t)

if __name__ == "__main__":
    main()