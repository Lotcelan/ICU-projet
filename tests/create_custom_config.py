import subprocess

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

def create_custom_config(confs, names, id):
    for (c, n) in list(zip(confs,names)):
        subprocess.call(["python", "gen_config.py", c[0], (c[1] + n.format(id))])

configs = read_user_conf()
names = ["../config/left_wall_temp_{}.conf", "../config/floor_temp_{}.conf", "../config/right_wall_temp_{}.conf", "../config/left_wall_h_{}.conf", "../config/floor_h_{}.conf", "../config/right_wall_h_{}.conf"]

id = 0
for conf in configs:
    create_custom_config(conf,names,id)
    id += 1