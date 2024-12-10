/*

** Fichero: client.c
** Autores:
** Daniel Dominguez Parra DNI 45138288Y
** Alex Asensio Boj DNI
*/
/*
* Programa que implementa el código del cliente y deriva a UDP o TCP segun convenga
* Uso: ./cliente <TCP/UDP > [usuario] [usuario@host]
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    //Robustez
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <TCP/UDP>\n", argv[0]);
        exit(1);
    }

    char* metodo = argv[1];
    char* remoteHost = "nogal";
    char* usuario = "all";

    if (argc > 2) {
        if (strchr(argv[2], '@')) { // Si aparece @ entonces
            char* atSign = strchr(argv[2], '@');
            if (atSign == argv[2]) { 
                // Si el @ está al principio, no hay usuario
                usuario = "all";
            } else {
                *atSign = '\0'; // Separar usuario y host
                usuario = argv[2]; // Usuario == antes del @
            }
            remoteHost = atSign + 1; // RemoteHost = después del @
        } else { 
            // Si no hay @, usuario = argv[2] y remoteHost es "localhost"
            usuario = argv[2];
        }
    }


    //Lanzar TCP o UDP
    pid_t pid = fork(); //*Para permitir que varios clienetes se ejecuten a la vez
    if (pid == 0){  //Hijo
        if (strcmp(metodo, "TCP") == 0) {
            execlp("./clientcp", "clientcp", remoteHost, usuario, NULL);
        } else if (strcmp(metodo, "UDP") == 0) {
            execlp("./clientudp", "clientudp", remoteHost, usuario, NULL);
        } else {
            fprintf(stderr, "Usage: %s <TCP || UDP>\n", argv[0]);
            exit(1);
        }
    } 
    else{  //Padre
        wait(NULL); //esperar muerte
    }
    return 0;
}