from flask import Flask, request, send_file, jsonify
import os

app = Flask(__name__)
DATA_DIR = "/data"
os.makedirs(DATA_DIR, exist_ok=True)


@app.route("/health", methods=["GET"])
def health():
    """Health check endpoint"""
    return "OK", 200


@app.route("/upload", methods=["POST"])
def upload():
    """Upload input file for workers to compress"""
    path = os.path.join(DATA_DIR, "input.txt")
    with open(path, "wb") as f:
        f.write(request.data)
    size = len(request.data)
    return jsonify({"status": "uploaded", "size": size, "path": path}), 200


@app.route("/submit", methods=["POST"])
def submit():
    """Workers submit their compressed binary chunks"""
    worker_id = request.headers.get("X-Worker-ID", "unknown")
    path = os.path.join(DATA_DIR, f"{worker_id}.bin")
    with open(path, "wb") as f:
        f.write(request.data)
    size = len(request.data)
    print(f"Received {size} bytes from worker {worker_id}")
    return jsonify({"status": "received", "worker": worker_id, "size": size}), 200


@app.route("/status", methods=["GET"])
def status():
    """Check status of worker submissions"""
    files = [f for f in os.listdir(DATA_DIR) if f.endswith(".bin")]
    total_size = sum(
        os.path.getsize(os.path.join(DATA_DIR, f)) for f in files
    )
    return jsonify({
        "workers_submitted": len(files),
        "files": files,
        "total_compressed_bytes": total_size
    }), 200


@app.route("/result", methods=["GET"])
def result():
    """Combine all worker outputs into final result"""
    out = os.path.join(DATA_DIR, "final.bin")
    files = sorted([f for f in os.listdir(DATA_DIR) if f.endswith(".bin") and f != "final.bin"])
    
    if not files:
        return jsonify({"error": "No compressed data available"}), 404
    
    with open(out, "wb") as dst:
        for f in files:
            with open(os.path.join(DATA_DIR, f), "rb") as src:
                dst.write(src.read())
    
    return send_file(out, as_attachment=True, download_name="compressed.bin")


if __name__ == "__main__":
    print("Starting Huffman API server on port 8080...")
    app.run(host="0.0.0.0", port=8080)

