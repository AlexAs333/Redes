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

    //Parámetros de entrada
    char* metodo = argv[1];
    char* remoteHost = "nogal";
    char* usuario = "all";
    if(argc > 2){
        if (strchr(argv[2], '@')) {//si aparece @ entonces
            char* atSign = strchr(argv[2], '@');
            *atSign = '\0'; // Separar usuario y host
            usuario = argv[2]; //usuario == antes@ argv[2]            
            remoteHost = atSign + 1; //remoteHost = despues@ argv[2];
        }
        else{   //sino hay @ usuario = argv[2] y remotehost es localhost
            usuario = argv[2];
        }
    }//sino hay segundo parametro los valores x defecto

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