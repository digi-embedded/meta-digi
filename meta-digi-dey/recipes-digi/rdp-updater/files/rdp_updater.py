import requests
import subprocess
import time
import socket
import platform

# Define the Flask server details
FLASK_SERVER_IP = "10.10.62.92"
FLASK_SERVER_PORT = "5000"

# Set the device type for this client (e.g., RS2)
device_type = "Mercury"

def check_rdp_session_active():
    """Check if there is an active RDP session on the client."""
    try:
        if platform.system() == "Windows":
            result = subprocess.run(["query", "user"], capture_output=True, text=True)
            return "rdp-tcp" in result.stdout.lower()
        else:
            return "Not Available"
    except FileNotFoundError:
        return "Not Available"
    except Exception as e:
        print(f"Error checking RDP session: {e}")
        return "Not Available"

def check_ssh_session_active():
    """Check if there is an active SSH session on the client."""
    try:
        if platform.system() == "Windows":
            result = subprocess.run(["netstat", "-ano"], capture_output=True, text=True)
            return "sshd" in result.stdout.lower()
        else:
            result = subprocess.run(["who"], capture_output=True, text=True)
            return "pts/" in result.stdout.lower()
    except FileNotFoundError:
        return "Not Available"
    except Exception as e:
        print(f"Error checking SSH session: {e}")
        return "Not Available"

def update_session_status():
    hostname = socket.gethostname()
    rdp_status = check_rdp_session_active()
    ssh_status = check_ssh_session_active()
    
    try:
        response = requests.post(
            f"http://{FLASK_SERVER_IP}:{FLASK_SERVER_PORT}/session_status",  
            json={'hostname': hostname, 'rdp_session_active': rdp_status, 'ssh_session_active': ssh_status, 'device_type': device_type}
        )
        if response.status_code == 200:
            print(f"Successfully updated session status for {device_type} ({hostname}) to {FLASK_SERVER_IP}.")
        else:
            print(f"Failed to update session status to {FLASK_SERVER_IP}.")
    except Exception as e:
        print(f"Error updating session status to {FLASK_SERVER_IP}: {e}")

if __name__ == "__main__":
    while True:
        update_session_status()
        time.sleep(10)  # Update every 10 seconds
