#!/usr/bin/env bash

KCTL="minikube kubectl --"
DOCKERCFG="$HOME/snap/docker/current/.docker/config.json"

# Dont do this if the Cluster is already running!
#minikube start

# Create a secret to login to docker hub
#${KCTL} create secret generic regcred --from-file=${DOCKERCFG} --type=kubernetes.io/dockerconfigjson


