#!/usr/bin/env bash
set -e

echo "🧹 Deleting old Kubernetes resources..."
kubectl delete -f manifest.yaml --ignore-not-found

echo "🐳 Removing old Docker images..."
docker rmi distributed-huffman:latest huffman-api:latest 2>/dev/null || true

echo "🔨 Rebuilding Docker images..."
docker build -t distributed-huffman:latest worker
docker build -t huffman-api:latest api

echo "📦 Loading images into kind..."
kind load docker-image distributed-huffman:latest
kind load docker-image huffman-api:latest

echo "🚀 Redeploying to Kubernetes..."
kubectl apply -f manifest.yaml

echo "⏳ Waiting for pods..."
kubectl wait --for=condition=Ready pod -l app=zookeeper --timeout=120s
kubectl wait --for=condition=Ready pod -l app=huffman-api --timeout=120s

echo "✅ Deployment complete"
kubectl get pods

