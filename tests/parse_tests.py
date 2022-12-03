import matplotlib
import matplotlib.pyplot as plt
import sys
matplotlib.use("GTK4Agg")
import pandas as pd

def parse_file(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()
    res =  []
    data = data.split("\n")
    for line in data:
        if line != "":
            temp = []
            for elem in line.split(";"):
                if elem != "":
                    temp.append(elem.split("="))
            res.append(temp)

    return res

def main(args):
    """
        Une fois les tests effectués, on veut pouvoir les exploiter. La variable parsed_content contient la liste des résultats de chaque test.
        Arguments :
            -> -d : afficher le graphe
            -> -Ma [float] : donne le numéro des simulations (ordre dans le fichier test.txt) dont la température max est plus grande qu'un seuil donné
            -> -b [float] [float] : donne le numéro des simulations (odre dans le fichier tests.txt) dont la température min est plus grande que le premier flottant et la temp max plus petite que la seconde valeur
            -> -be [float] [float] [file_path] : donne le numéro des simulations (odre dans le fichier tests.txt) dont la température min est plus grande que le premier flottant et la temp max plus petite que la seconde valeur ET exporte les données dans [file_name] de chaque simulation
    """

    parsed_content = parse_file("../results/tests.txt")

    if (a := "-Ma" in args) or (b := "--max-above-value" in args):
        i = args.index("-Ma" if a else "--max-above-value")
        try:
            value = float(args[i+1])
            res = []
            i = 0
            for line in parsed_content:
                if (float(line[-2][1])) != "inf" and (float(line[-2][1])) > value:
                    res.append(i)
                i+=1
            print(f"Les simulations qui ont donné une max_temp plus grande (strictement) que {value} sont : {res}")
        except:
            print("Erreur, entrez une valeur cohérente")

    if (a := "-b" in args) or (b := "--between" in args):
        i = args.index("-b" if a else "--between")
        try:
            min_floor = float(args[i+1])
            max_ceil = float(args[i+2])
            res = []
            i = 0
            for line in parsed_content:
                if (float(line[-2][1])) != "inf" and (float(line[-2][1])) <= max_ceil and float(line[-3][1]) != "-inf" and float(line[-3][1]) >= min_floor:
                    res.append(i)
                i+=1
            print(f"Les simulations qui ont donné une max_temp plus petite que {max_ceil} et une min_temp plus grande que {min_floor} sont : {res}")
        except Exception as e:
            print(f"Erreur, entrez une valeur cohérente !\n {e}")

    if (a := "-be" in args) or (b := "--between-export" in args):
        i = args.index("-be" if a else "--between-export")
        try:
            min_floor = float(args[i+1])
            max_ceil = float(args[i+2])
            export_file = args[i+3]
            res_id = []
            res_content = []
            i = 0
            for line in parsed_content:
                if (float(line[-2][1])) != "inf" and (float(line[-2][1])) <= max_ceil and float(line[-3][1]) != "-inf" and float(line[-3][1]) >= min_floor:
                    print(f"Checking {float(line[-3][1])} and {float(line[-2][1])}")
                    res_id.append(i)
                    res_content.append(line)
                i+=1
            print(f"Les simulations qui ont donné une max_temp plus petite que {max_ceil} et une min_temp plus grande que {min_floor} sont : {res_id}")
            with open(export_file, "w+") as f:
                for l in res_content:
                    for (oui, val) in l:
                        f.write(f"{oui} = {val} | ")
                    f.write("\n")
            

        except Exception as e:
            print(f"Erreur, entrez une valeur cohérente !\n {e}")


    if "-d" in args or "--display-plot" in args:

        min_temps = [float(line[-3][1]) if float(line[-3][1]) != float("-inf") else 0 for line in parsed_content]
        max_temps = [float(line[-2][1]) if float(line[-2][1]) != float("inf") else 0 for line in parsed_content]


        indexes = [i for i in range(len(parsed_content))]
        
        df = pd.DataFrame([[str(i),min_temps[i], max_temps[i]] for i in indexes], columns=["Idx","Min_temp","Max_temp"])

        df.plot(x="Idx",kind="bar",title="Oui")

        plt.ylim([-50,50])
        plt.grid(True)
        
        plt.axhline(y = 15, color = 'r', linestyle = '-') # Si l'on a fait tous les tests a une T_e identique
        
        plt.show()

if __name__ == "__main__":
    args = sys.argv
    main(args)