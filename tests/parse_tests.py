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