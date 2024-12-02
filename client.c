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
    char* remoteHost;
    char* usuario;
    if(argc > 3){
        //si aparece @ entonces
            //usuario == despues@ argv[2]
            //remoteHost = despues@ argv[2];
        //sino
            //usuario == argv[2]
            //remoteHost = //!localhost
    }
    else{
        //usuario == ???
        //remoteHost = //!localhost
    }

    //Lanzar TCP o UDP
    pid_t pid = fork(); //*Para permitir que varios clienetes se ejecuten a la vez
    if (pid == 0){  //Hijo
        if (strcmp(metodo, "TCP") == 0) {
            execlp("./clientcp", "clientcp", usuario, remoteHost, NULL);
        } else if (strcmp(metodo, "UDP") == 0) {
            printf("UDP\n");
            execlp("./clientudp", "clientudp", usuario, remoteHost, NULL);
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