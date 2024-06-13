import os
import requests
from requests.auth import HTTPBasicAuth
import getpass

# Définir les variables
repo_url = "https://nexus.lmb.liebherr.i/repository/raw-pd1/HEXplorer_repo/" 
directory_path = "C:/Users/lmbhoc2/Documents/Code/HEXplorer/HEXplorer_qt6_github/installer/repository" 

# Prompt for username and password
username = input("Enter your Nexus username: ")
password = getpass.getpass("Enter your Nexus password: ") 

cert_file = 'cert/LiebherrRootCA2.crt'

# Parcourir tous les fichiers dans le répertoire
for root, _, files in os.walk(directory_path):
    for file_name in files:
        file_path = os.path.join(root, file_name)
        # Read each file
        with open(file_path, 'rb') as file:
            # Define the complete URL for the upload
            upload_url = f"{repo_url}/{os.path.relpath(file_path, directory_path)}"
            
            # Perform the PUT request to upload the file
            response = requests.put(upload_url, data=file, auth=HTTPBasicAuth(username, password), verify=False)
            
            # Check the response
            if response.status_code == 201:
                print(f"File '{file_name}' uploaded successfully")
            else:
                print(f"Upload error for '{file_name}': {response.status_code} - {response.text}")