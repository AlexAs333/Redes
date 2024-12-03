#!/bin/bash
PID=$(ps -e | grep 'servidor' | awk '{print $1}')

if [ -n "$PID" ]; then
    kill -15 $PID
    echo "Proceso 'servidor' detenido (PID: $PID)."
else
    echo "No se encontró ningún proceso llamado 'servidor'."
fi
