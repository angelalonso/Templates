import subprocess
import shutil
import sys
import platform
from typing import Tuple

def check():
    """
    Check and correct minikube installation.
    """
    if is_installed():
        print("Minikube is installed on this system.")
    else:
        print("Minikube is NOT installed on this system. Install it first")
        print(" - you can do the following:")
        print(" curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64")
        print(" sudo install minikube-linux-amd64 /usr/local/bin/minikube")
        sys.exit(1)


def is_installed():
    """
    Check if minikube is installed on the system.
    Returns True if minikube is found and working, False otherwise.
    """
    # First check if the command exists in PATH
    if shutil.which("minikube") is None:
        return False
    
    try:
        # Try to get the version to verify it's properly installed
        result = subprocess.run(
            ["minikube", "version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return result.returncode == 0
    except (subprocess.SubprocessError, FileNotFoundError):
        return False
