import sys, json

def val_by_loc(default_value, list, i, j):
    for elem in list:
        if j >= elem["start_idx"][1] and j < elem["start_idx"][1] + elem["width"] and i >= elem["start_idx"][0] and i < elem["start_idx"][0] + elem["height"]:
            return elem["value"]
    return default_value

def custom(config, rows, cols, filename):
    default_value = config["default_value"]
    list = config['list']
    f = open(filename, "w+")
    for i in range(rows):
        for j in range(cols - 1):
            f.write(str(val_by_loc(default_value, list, i, j)) + " ")
        f.write(str(val_by_loc(default_value, list, i, cols - 1)) + "\n")
    f.close()

def all_the_same(value, rows, cols, filename):
    f = open(filename, "w+")
    for i in range(rows):
        for j in range(cols-1):
            f.write(str(value) + " ")
        f.write(value + "\n")
    f.close()


def main(args):
    """
        Objectif : permettre de créer des fichiers de configuration pour la simulation (par exemple la température, ou les valeurs de h)
        Param :
            args : Liste d'arguments en ligne de commande
            - 0 : nom du fichier
            - 1 : type de config (obligatoire)
                - "all"
                - "custom"
            - 2 : config (dépend du type de config, mais obligatoire)
                - pour "all" :
                    - "[value];[rows]*[cols];[path_to_config_file]" sans les crochets
                - pour "custom" :
                    - "[config];[rows]*[cols];[path_to_config_file]" avec config un string JSON de la forme : (en voyant la matrice comme commençant en haut à gauche par (0,0) (en terme d'index) et en incrémentant le premier pour les lignes et le second pour les colonnes)
                        '{
                            "default_value" : [default_value],
                            "list" : [
                                { "value" : [value], "start_idx" :  [[start_row], [start_col]], "width" : [width], "height" : [height]},
                                ...
                            ] 
                        
                        }'
            Exemple : 
                - python gen_config.py custom '{ "default_value" : 288, "list" : [ { "value" : 200, "start_idx" :  [0,0], "width" : 2, "height" : 3 }, { "value" : 200, "start_idx" : [5,5], "width" : 3, "height" : 1 } ] };10*10;../config/test'
    """

    if args[1] == "all":

        value = args[2].split(";")[0]
        rows, cols = [int(i) for i in args[2].split(";")[1].split("*")]
        filename = args[2].split(";")[2]

        all_the_same(value, rows, cols, filename)

    elif args[1] == "custom":
        config = json.loads(args[2].split(";")[0])
        rows, cols = [int(i) for i in args[2].split(";")[1].split("*")]
        filename = args[2].split(";")[2]

        custom(config, rows, cols, filename)


if __name__ == "__main__":
    args = sys.argv
    print(f"{sys.argv=}")
    main(args)