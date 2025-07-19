from flask import Flask, render_template
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

app = Flask(__name__, static_url_path='/static')

@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def catch_all(path):
    return render_template('index.html')

if __name__ == '__main__':
    app.run(debug=True)
