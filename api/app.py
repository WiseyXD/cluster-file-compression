from flask import Flask, request, send_file
import os

app = Flask(__name__)
DATA_DIR = "/data"
os.makedirs(DATA_DIR, exist_ok=True)

@app.route("/submit", methods=["POST"])
def submit():
    worker_id = request.headers.get("X-Worker-ID", "unknown")
    path = os.path.join(DATA_DIR, f"{worker_id}.bin")
    with open(path, "wb") as f:
        f.write(request.data)
    return "OK", 200

@app.route("/result", methods=["GET"])
def result():
    out = os.path.join(DATA_DIR, "final.bin")
    with open(out, "wb") as dst:
        for f in sorted(os.listdir(DATA_DIR)):
            if f.endswith(".bin"):
                with open(os.path.join(DATA_DIR, f), "rb") as src:
                    dst.write(src.read())
    return send_file(out, as_attachment=True)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
