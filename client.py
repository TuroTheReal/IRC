import socket
import time

HOST = "localhost"
PORT = 7772

# Ouvre une connexion socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    # Fonction pour envoyer une commande IRC bien formée
    def send(cmd):
        print(f"=> {cmd}")
        s.sendall((cmd + "\r\n").encode())
        time.sleep(0.5)

    # Envoie quelques commandes classiques
    send("CAP LS 302")
    send("NICK testuser")
    send("USER testuser 0 * :Test User")
    send("JOIN #42")
    send("PRIVMSG #42 :Hello depuis le client Python !")

    # Attend la réponse (non bloquant ici, lecture simple)
    try:
        while True:
            data = s.recv(1024)
            if not data:
                break
            print(f"<= {data.decode(errors='ignore')}")
    except KeyboardInterrupt:
        print("Déconnexion")
