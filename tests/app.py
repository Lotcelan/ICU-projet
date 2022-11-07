from flask import Flask, request
app = Flask(__name__)
@app.route('/', methods=['POST'])
def result():
    for param in request.form.to_dict():
        print(f"{param} -> {request.form.to_dict()[param]}")
    print("")
    return 'Received !' # response to your request.