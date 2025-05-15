#!/bin/bash

HOST=localhost
PORT=6666

TERMINAL_PIDS=()

# Fonction de nettoyage pour le script principal
cleanup_main() {
    echo "Fermeture de toutes les connexions..."

    # Fermer tous les terminaux ouverts
    for pid in "${TERMINAL_PIDS[@]}"; do
        if ps -p $pid > /dev/null; then
            kill -TERM $pid 2>/dev/null
        fi
    done

    exit 0
}

# Configuration du trap pour le script principal
trap cleanup_main SIGINT SIGTERM EXIT

# Terminal 1 : alice
gnome-terminal --title="IRC Client - alice" -- bash -c "
    # Fonction de nettoyage pour le client
    cleanup_client() {
        echo 'Fermeture de la connexion...'
        [[ -n \"\$BG_PID\" ]] && kill \$BG_PID 2>/dev/null
        [[ -e /proc/self/fd/3 ]] && exec 3>&-
        exit 0
    }

    # Configuration du trap pour le client
    trap cleanup_client SIGINT SIGTERM EXIT

    # Établir la connexion
    exec 3<>/dev/tcp/$HOST/$PORT

    # Messages d'initialisation
    sleep 1
    echo -e 'PASS 1\r' >&3
    sleep 0.3
    echo -e 'NICK alice\r' >&3
    sleep 0.3
    echo -e 'USER alice 0 * :Alice Anderson\r' >&3
    sleep 0.3
    echo -e 'JOIN #room\r' >&3

    # Configuration pour permettre la lecture et l'écriture
    echo 'Client IRC connecté en tant que alice. Tapez vos commandes (Ctrl+C pour quitter):'

    # Lecture des réponses du serveur en arrière-plan
    (while IFS= read -r line <&3; do
        echo \"\$line\"
    done) &
    BG_PID=\$!

    # Lecture de l'entrée utilisateur et envoi au serveur
    while read input; do
        # Vérifier si on veut quitter
        if [[ \"\$input\" == \"quit\" || \"\$input\" == \"exit\" ]]; then
            echo -e \"QUIT :Au revoir\r\" >&3
            cleanup_client
        else
            echo -e \"\$input\r\" >&3
        fi
    done
" &
TERMINAL_PIDS+=($!)

# Terminal 2 : bob
gnome-terminal --title="IRC Client - bob" -- bash -c "
    # Fonction de nettoyage pour le client
    cleanup_client() {
        echo 'Fermeture de la connexion...'
        [[ -n \"\$BG_PID\" ]] && kill \$BG_PID 2>/dev/null
        [[ -e /proc/self/fd/3 ]] && exec 3>&-
        exit 0
    }

    # Configuration du trap pour le client
    trap cleanup_client SIGINT SIGTERM EXIT

    # Établir la connexion
    exec 3<>/dev/tcp/$HOST/$PORT

    # Messages d'initialisation
    sleep 1
    echo -e 'PASS 1\r' >&3
    sleep 0.3
    echo -e 'NICK bob\r' >&3
    sleep 0.3
    echo -e 'USER bob 0 * :Bob Brown\r' >&3
    sleep 0.3
    echo -e 'JOIN #room\r' >&3

    # Configuration pour permettre la lecture et l'écriture
    echo 'Client IRC connecté en tant que bob. Tapez vos commandes (Ctrl+C pour quitter):'

    # Lecture des réponses du serveur en arrière-plan
    (while IFS= read -r line <&3; do
        echo \"\$line\"
    done) &
    BG_PID=\$!

    # Lecture de l'entrée utilisateur et envoi au serveur
    while read input; do
        # Vérifier si on veut quitter
        if [[ \"\$input\" == \"quit\" || \"\$input\" == \"exit\" ]]; then
            echo -e \"QUIT :Au revoir\r\" >&3
            cleanup_client
        else
            echo -e \"\$input\r\" >&3
        fi
    done
" &
TERMINAL_PIDS+=($!)

# Terminal 3 : carol
gnome-terminal --title="IRC Client - carol" -- bash -c "
    # Fonction de nettoyage pour le client
    cleanup_client() {
        echo 'Fermeture de la connexion...'
        [[ -n \"\$BG_PID\" ]] && kill \$BG_PID 2>/dev/null
        [[ -e /proc/self/fd/3 ]] && exec 3>&-
        exit 0
    }

    # Configuration du trap pour le client
    trap cleanup_client SIGINT SIGTERM EXIT

    # Établir la connexion
    exec 3<>/dev/tcp/$HOST/$PORT

    # Messages d'initialisation
    sleep 1
    echo -e 'PASS 1\r' >&3
    sleep 0.3
    echo -e 'NICK carol\r' >&3
    sleep 0.3
    echo -e 'USER carol 0 * :Carol Clark\r' >&3
    sleep 0.3
    #echo -e 'JOIN #room\r' >&3

    # Configuration pour permettre la lecture et l'écriture
    echo 'Client IRC connecté en tant que carol. Tapez vos commandes (Ctrl+C pour quitter):'

    # Lecture des réponses du serveur en arrière-plan
    (while IFS= read -r line <&3; do
        echo \"\$line\"
    done) &
    BG_PID=\$!

    # Lecture de l'entrée utilisateur et envoi au serveur
    while read input; do
        # Vérifier si on veut quitter
        if [[ \"\$input\" == \"quit\" || \"\$input\" == \"exit\" ]]; then
            echo -e \"QUIT :Au revoir\r\" >&3
            cleanup_client
        else
            echo -e \"\$input\r\" >&3
        fi
    done
" &
TERMINAL_PIDS+=($!)

# Terminal 4 : dave
gnome-terminal --title="IRC Client - dave" -- bash -c "
    # Fonction de nettoyage pour le client
    cleanup_client() {
        echo 'Fermeture de la connexion...'
        [[ -n \"\$BG_PID\" ]] && kill \$BG_PID 2>/dev/null
        [[ -e /proc/self/fd/3 ]] && exec 3>&-
        exit 0
    }

    # Configuration du trap pour le client
    trap cleanup_client SIGINT SIGTERM EXIT

    # Établir la connexion
    exec 3<>/dev/tcp/$HOST/$PORT

    # Messages d'initialisation
    sleep 1
    echo -e 'PASS 1\r' >&3
    sleep 0.3
    echo -e 'NICK dave\r' >&3
    sleep 0.3
    echo -e 'USER dave 0 * :Dave Davis\r' >&3
    sleep 0.3
    #echo -e 'JOIN #room\r' >&3

    # Configuration pour permettre la lecture et l'écriture
    echo 'Client IRC connecté en tant que dave. Tapez vos commandes (Ctrl+C pour quitter):'

    # Lecture des réponses du serveur en arrière-plan
    (while IFS= read -r line <&3; do
        echo \"\$line\"
    done) &
    BG_PID=\$!

    # Lecture de l'entrée utilisateur et envoi au serveur
    while read input; do
        # Vérifier si on veut quitter
        if [[ \"\$input\" == \"quit\" || \"\$input\" == \"exit\" ]]; then
            echo -e \"QUIT :Au revoir\r\" >&3
            cleanup_client
        else
            echo -e \"\$input\r\" >&3
        fi
    done
" &
TERMINAL_PIDS+=($!)

echo "Clients IRC démarrés. Appuyez sur Ctrl+C pour fermer tous les terminaux."

# Garder le script principal en vie jusqu'à ce que l'utilisateur appuie sur Ctrl+C
wait