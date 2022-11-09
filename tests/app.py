from flask import Flask, request

f = open("../results/tests.txt", "a")

try:
    app = Flask(__name__)
    @app.route('/', methods=['POST'])
    def result():
        res = ""
        for param in request.form.to_dict():
            #print(f"{param} -> {request.form.to_dict()[param]}")
            res += f"{param}={request.form.to_dict()[param]};"
        print("Received !")
        f.write(res + "\n")
        print("")
        return 'Received !' # response to your request.
except:
    f.close()
