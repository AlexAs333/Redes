#!/bin/bash

# Si se pasó un argumento, intentar matar al proceso llamado 'servidor'
if [ -n "$1" ]; then
    PID=$(ps -e | grep 'servidor' | awk '{print $1}')
    
    if [ -n "$PID" ]; then
        kill -9 $PID
        echo "Proceso 'servidor' detenido (PID: $PID)."
    else
        echo "No se encontró ningún proceso llamado 'servidor'."
    fi
fi

# Compilar los archivos
gcc servidor.c funciones.c -o servidor
gcc client.c -o client
