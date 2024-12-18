/*

** Fichero: servidor.c
** Autores:
** Daniel Dominguez Parra DNI 45138288Y
** Alex Asensio Boj DNI 77221233K
*/
/*
 *          		S E R V I D O R
 *
 *	This is an example program that demonstrates the use of
 *	sockets TCP and UDP as an IPC mechanism.  
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "funciones.h"


#define PUERTO 1733
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	1024	/* maximum size of packets to be received */
#define MAXHOST 128

extern int errno;

/*
 *			M A I N
 *
 *	This routine starts the server.  It forks, leaving the child
 *	to do all the work, so it does not have to be run in the
 *	background.  It sets up the sockets.  It
 *	will loop forever, until killed by a signal.
 *
 */
 
void serverTCP(int s, struct sockaddr_in peeraddr_in, FILE *fichero);
void serverUDP(int s, char * buffer, struct sockaddr_in clientaddr_in, FILE *fichero);
void errout(char *);		/* declare error out routine */

int FIN = 0;             /* Para el cierre ordenado */
void finalizar(){ FIN = 1; }

int main(argc, argv)
int argc;
char *argv[];
{

    int s_TCP, s_UDP;		/* connected socket descriptor */
    int ls_TCP;				/* listen socket descriptor */
    
    int cc;				    /* contains the number of bytes read */
     
    struct sigaction sa = {.sa_handler = SIG_IGN}; /* used to ignore SIGCHLD */
    
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in clientaddr_in;	/* for peer socket address */
	int addrlen;
	
    fd_set readmask;
    int numfds,s_mayor;
    
    char buffer[BUFFERSIZE];	/* buffer for packets to be read into */
    
    struct sigaction vec;

    //*Crear el fichero
    FILE *fichero = fopen("peticiones.log", "a");

    if(fichero == NULL){
        fprintf(stderr, "Error al abrir el fichero\n");
        exit(1);
    }

		/* Create the listen socket. */
	ls_TCP = socket (AF_INET, SOCK_STREAM, 0);
	if (ls_TCP == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to create socket TCP\n", argv[0]);
		exit(1);
	}
	/* clear out address structures */
	memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
   	memset ((char *)&clientaddr_in, 0, sizeof(struct sockaddr_in));

    addrlen = sizeof(struct sockaddr_in);

		/* Set up address structure for the listen socket. */
	myaddr_in.sin_family = AF_INET;
		/* The server should listen on the wildcard address,
		 * rather than its own internet address.  This is
		 * generally good practice for servers, because on
		 * systems which are connected to more than one
		 * network at once will be able to have one server
		 * listening on all networks at once.  Even when the
		 * host is connected to only one network, this is good
		 * practice, because it makes the server program more
		 * portable.
		 */
	myaddr_in.sin_addr.s_addr = INADDR_ANY;
	myaddr_in.sin_port = htons(PUERTO);

	/* Bind the listen address to the socket. */
	if (bind(ls_TCP, (const struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to bind address TCP\n", argv[0]);
		exit(1);
	}
		/* Initiate the listen on the socket so remote users
		 * can connect.  The listen backlog is set to 5, which
		 * is the largest currently supported.
		 */
	if (listen(ls_TCP, 5) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to listen on socket\n", argv[0]);
		exit(1);
	}
	
	
	/* Create the socket UDP. */
	s_UDP = socket (AF_INET, SOCK_DGRAM, 0);
	if (s_UDP == -1) {
		perror(argv[0]);
		printf("%s: unable to create socket UDP\n", argv[0]);
		exit(1);
	   }
	/* Bind the server's address to the socket. */
	if (bind(s_UDP, (struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		printf("%s: unable to bind address UDP\n", argv[0]);
		exit(1);
	    }

		/* Now, all the initialization of the server is
		 * complete, and any user errors will have already
		 * been detected.  Now we can fork the daemon and
		 * return to the user.  We need to do a setpgrp
		 * so that the daemon will no longer be associated
		 * with the user's control terminal.  This is done
		 * before the fork, so that the child will not be
		 * a process group leader.  Otherwise, if the child
		 * were to open a terminal, it would become associated
		 * with that terminal as its control terminal.  It is
		 * always best for the parent to do the setpgrp.
		 */
	setpgrp();

	switch (fork()) {
	case -1:		/* Unable to fork, for some reason. */
		perror(argv[0]);
		fprintf(stderr, "%s: unable to fork daemon\n", argv[0]);
		exit(1);

	case 0:     /* The child process (daemon) comes here. */

			/* Close stdin and stderr so that they will not
			 * be kept open.  Stdout is assumed to have been
			 * redirected to some logging file, or /dev/null.
			 * From now on, the daemon will not report any
			 * error messages.  This daemon will loop forever,
			 * waiting for connections and forking a child
			 * server to handle each one.
			 */
		fclose(stdin);
		fclose(stderr);

			/* Set SIGCLD to SIG_IGN, in order to prevent
			 * the accumulation of zombies as each child
			 * terminates.  This means the daemon does not
			 * have to make wait calls to clean them up.
			 */
		if ( sigaction(SIGCHLD, &sa, NULL) == -1) {
            perror(" sigaction(SIGCHLD)");
            fprintf(stderr,"%s: unable to register the SIGCHLD signal\n", argv[0]);
            exit(1);
            }
            
		    /* Registrar SIGTERM para la finalizacion ordenada del programa servidor */
        vec.sa_handler = (void *) finalizar;
        vec.sa_flags = 0;
        if ( sigaction(SIGTERM, &vec, (struct sigaction *) 0) == -1) {
            perror(" sigaction(SIGTERM)");
            fprintf(stderr,"%s: unable to register the SIGTERM signal\n", argv[0]);
            exit(1);
            }
        
		while (!FIN) {
            /* Meter en el conjunto de sockets los sockets UDP y TCP */
            FD_ZERO(&readmask);
            FD_SET(ls_TCP, &readmask);
            FD_SET(s_UDP, &readmask);
            /* 
            Seleccionar el descriptor del socket que ha cambiado. Deja una marca en 
            el conjunto de sockets (readmask)
            */ 
    	    if (ls_TCP > s_UDP) s_mayor=ls_TCP;
    		else s_mayor=s_UDP;

            if ( (numfds = select(s_mayor+1, &readmask, (fd_set *)0, (fd_set *)0, NULL)) < 0) {
                if (errno == EINTR) {
                    FIN=1;
		            close (ls_TCP);
		            close (s_UDP);
                    perror("\nFinalizando el servidor. Se�al recibida en elect\n "); 
                }
            }
           else { 

                /* Comprobamos si el socket seleccionado es el socket TCP */
                if (FD_ISSET(ls_TCP, &readmask)) {
                    /* Note that addrlen is passed as a pointer
                     * so that the accept call can return the
                     * size of the returned address.
                     */
    				/* This call will block until a new
    				 * connection arrives.  Then, it will
    				 * return the address of the connecting
    				 * peer, and a new socket descriptor, s,
    				 * for that connection.
    				 */
    			s_TCP = accept(ls_TCP, (struct sockaddr *) &clientaddr_in, &addrlen);
    			if (s_TCP == -1) exit(1);
    			switch (fork()) {
        			case -1:	/* Can't fork, just exit. */
        				exit(1);
        			case 0:		/* Child process comes here. */
                    			close(ls_TCP); /* Close the listen socket inherited from the daemon. */
        				serverTCP(s_TCP, clientaddr_in, fichero);
        				exit(0);
        			default:	/* Daemon process comes here. */
        					/* The daemon needs to remember
        					 * to close the new accept socket
        					 * after forking the child.  This
        					 * prevents the daemon from running
        					 * out of file descriptor space.  It
        					 * also means that when the server
        					 * closes the socket, that it will
        					 * allow the socket to be destroyed
        					 * since it will be the last close.
        					 */
        				close(s_TCP);
        			}
             } /* De TCP*/
          /* Comprobamos si el socket seleccionado es el socket UDP */
          if (FD_ISSET(s_UDP, &readmask)) {
                /* This call will block until a new
                * request arrives.  Then, it will
                * return the address of the client,
                * and a buffer containing its request.
                * BUFFERSIZE - 1 bytes are read so that
                * room is left at the end of the buffer
                * for a null character.
                */
                cc = recvfrom(s_UDP, buffer, BUFFERSIZE - 1, 0,
                   (struct sockaddr *)&clientaddr_in, &addrlen);
                if ( cc == -1) {
                    perror(argv[0]);
                    printf("%s: recvfrom error\n", argv[0]);
                    exit (1);
                    }
                /* Make sure the message received is
                * null terminated.
                */
                buffer[cc]='\0';
                serverUDP (s_UDP, buffer, clientaddr_in, fichero);
                }
          }
		}   /* Fin del bucle infinito de atenci�n a clientes */
        /* Cerramos los sockets UDP y TCP */
        close(ls_TCP);
        close(s_UDP);
        fclose(fichero);
    
        printf("\nFin de programa servidor!\n");
        
	default:		/* Parent process comes here. */
		exit(0);
	}

}

/*
 *				S E R V E R T C P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */

// La función serverTCP modificada
void serverTCP(int s, struct sockaddr_in clientaddr_in, FILE *fichero)
{
    int reqcnt = 0; /* keeps count of number of requests */
    char buf[TAM_BUFFER]; /* This example uses TAM_BUFFER byte messages. */
    char hostname[MAXHOST]; /* remote host's name string */

    int len, len1, status;
    struct hostent *hp; /* pointer to host info for remote host */
    long timevar; /* contains time returned by time() */

    struct linger linger; /* allow a lingering, graceful close; */
    
    /* Look up the host information for the remote host */
    status = getnameinfo((struct sockaddr *)&clientaddr_in, sizeof(clientaddr_in),
                           hostname, MAXHOST, NULL, 0, 0);
    if (status) {
        if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL)
            perror("inet_ntop");
    }

    /* Log a startup message. */
    time(&timevar);
    printf("Startup from %s port %u at %s", hostname, ntohs(clientaddr_in.sin_port), (char *)ctime(&timevar));

    /* Set the socket for a lingering, graceful close. */
    linger.l_onoff = 1;
    linger.l_linger = 1;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) == -1) {
        errout(hostname);
    }
    char respuesta[TAM_BUFFER];
    char acumulado[TAM_BUFFER*100];
    /* Go into a loop, receiving requests from the remote client. */
    while (len = recv(s, buf, TAM_BUFFER, 0)) {
        if (len == -1) errout(hostname); /* error from recv */

        while (len < TAM_BUFFER) {
            len1 = recv(s, &buf[len], TAM_BUFFER - len, 0);
            if (len1 == -1) errout(hostname);
            len += len1;
        }

        reqcnt++;
        
        if(strcmp(buf, "all") == 0){
		FILE *fp;
        	char line[TAM_BUFFER];
    		char login[TAM_BUFFER];
    		struct passwd *pwd;

    		// Ejecutar el comando "who"
    		fp = popen("who", "r");
   		if (fp == NULL) {
        		errout(hostname);
    		}

    		// Leer cada línea de la salida de "who"
    		while (fgets(line, sizeof(line), fp) != NULL) {
        		// Extraer el login (asumimos que es la primera palabra de cada línea)
        		if (sscanf(line, "%s", login) == 1) {
            			// Buscar el nombre completo del usuario usando /etc/passwd
           			 pwd = getpwnam(login);
            			if (pwd == NULL) {
                			snprintf(respuesta, TAM_BUFFER, "Usuario %s no encontrado en /etc/passwd\n", login);
            			} else {
                			snprintf(respuesta, TAM_BUFFER, "%s", finger(login));
            			}

            			// Enviar la respuesta del finger al cliente
	    			strcat(acumulado, respuesta);
				if (send(s, respuesta, strlen(respuesta), 0) != strlen(respuesta)) {
                			errout(hostname);
            			}
        		}
    		}	

    		pclose(fp);
	}
	else {
        	snprintf(respuesta, TAM_BUFFER, "%s", finger(buf));
		strcat(acumulado,respuesta);
            	if (send(s, respuesta, strlen(respuesta), 0) != strlen(respuesta)) {
                	errout(hostname);
            	}
        }
        
    }
    //log del TCP
    // Obtener la fecha y hora actual para el registro
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char fecha[26];
    strftime(fecha, 26, "%a %b %d %H:%M (%Z)", tm_info);

    // Obtener el nombre del host
    char hostnameBuffer[256];
    char nombreHost[256];
    if (gethostname(hostnameBuffer, sizeof(hostnameBuffer)) == 0) {
        
        strncpy(nombreHost, hostnameBuffer, sizeof(nombreHost) - 1);
        nombreHost[sizeof(nombreHost) - 1] = '\0';
    } 
    else {
        perror("Error obteniendo el nombre del host");
        strcpy(nombreHost, "Desconocido");
    }

    // Obtener la dirección IP del cliente
    char ip[INET_ADDRSTRLEN];
    strncpy(ip, inet_ntoa(clientaddr_in.sin_addr), sizeof(ip) - 1);
    ip[sizeof(ip) - 1] = '\0';

    // Definir otros valores
    char protocolo[] = "TCP";
    char puertoEfimero[10];
    snprintf(puertoEfimero, sizeof(puertoEfimero), "%d",ntohs(clientaddr_in.sin_port));
    int puertoCliente = ntohs(clientaddr_in.sin_port);

    // Llamar a la función para escribir en el archivo
    editaFichero(fichero, fecha, nombreHost, ip, protocolo, puertoEfimero, buf, acumulado, puertoCliente);


    close(s);

    /* Log a finishing message. */
    time(&timevar);
    printf("Completed %s port %u, %d requests, at %s\n",
           hostname, ntohs(clientaddr_in.sin_port), reqcnt, (char *)ctime(&timevar));
}


/*
 *	This routine aborts the child process attending the client.
 */
void errout(char *hostname)
{
	printf("Connection with %s aborted on error\n", hostname);
	exit(1);     
}


/*
 *				S E R V E R U D P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */
void serverUDP(int s, char *buffer, struct sockaddr_in clientaddr_in, FILE *fichero)
{
    struct in_addr reqaddr; /* for requested host's address */
    struct hostent *hp;     /* pointer to host info for requested host */
	struct passwd *pwd;
    int nc, errcode;
    struct addrinfo hints, *res;
    int addrlen = sizeof(struct sockaddr_in);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    /* Treat the message as a string containing a hostname. */
    /* Esta función es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta. */
    //!!COMENTADO PORQUE BLOQUEA ?
    /*errcode = getaddrinfo(buffer, NULL, &hints, &res); 
    if (errcode != 0){
        /* Name was not found.  Return a
         * special value signifying the error. */
        /*reqaddr.s_addr = ADDRNOTFOUND;
      }
    else {*/
        /* Copy address of host into the return buffer. */
        /*reqaddr = ((struct sockaddr_in *) res.ai_addr).sin_addr;
    }
    freeaddrinfo(res);*/

    char respuesta[TAM_BUFFER];
    char acumulado[TAM_BUFFER*100];

	if (strcmp(buffer, "all") == 0) {
        	FILE *fp;
        	char line[256];
        	char login[64];

        	// Abrir un pipe para ejecutar el comando 'who'
        	fp = popen("who", "r");
        	if (fp == NULL) {
            		perror("popen");
            	return;
        	}

        	// Leer cada línea de la salida de 'who'
        	while (fgets(line, sizeof(line), fp) != NULL) {
			if (sscanf(line, "%s", login) == 1) {
            			// Buscar el nombre completo del usuario usando /etc/passwd
            			pwd = getpwnam(login);
            			if (pwd == NULL) {
                			snprintf(respuesta, TAM_BUFFER, "Usuario %s no encontrado en /etc/passwd\n", login);
            			} else {
                			printf("Consultando usuario: %s (%s)\n", login, pwd->pw_gecos);
                			snprintf(respuesta, TAM_BUFFER, "%s", finger(login));
            			}
                		// Enviar la respuesta al cliente
				strcat(acumulado, respuesta);
                		nc = sendto(s, respuesta, strlen(respuesta) +1, 0, (struct sockaddr *)&clientaddr_in, addrlen);
                		if (nc == -1) {
                    			perror("serverUDP");
                    			printf("%s: sendto error\n", "serverUDP");
                    			pclose(fp); // Cerrar el pipe antes de salir
                    			return;
                		}
          		}
        	}

        	// Cerrar el pipe
        	pclose(fp);
    	}

	else {
        /* Consultar un usuario específico */
        snprintf(respuesta, TAM_BUFFER, "%s", finger(buffer));
	strcat(acumulado,respuesta);
        nc = sendto(s, respuesta, strlen(respuesta) +1, 0, (struct sockaddr *)&clientaddr_in, addrlen);
        if (nc == -1) {
            perror("serverUDP");
            printf("%s: sendto error\n", "serverUDP");
            return;
        }
    }
    
    nc = sendto(s, 0,0, 0, (struct sockaddr *)&clientaddr_in, addrlen);
    //log del UDP
    // Obtener la fecha y hora actual para el registro
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char fecha[26];
    strftime(fecha, 26, "%a %b %d %H:%M (%Z)", tm_info);

    // Obtener el nombre del host
    char hostnameBuffer[256];
    char nombreHost[256];
    if (gethostname(hostnameBuffer, sizeof(hostnameBuffer)) == 0) {
        
        strncpy(nombreHost, hostnameBuffer, sizeof(nombreHost) - 1);
        nombreHost[sizeof(nombreHost) - 1] = '\0';
    } 
    else {
        perror("Error obteniendo el nombre del host");
        strcpy(nombreHost, "Desconocido");
    }

    // Obtener la dirección IP del cliente
    char ip[INET_ADDRSTRLEN];
    strncpy(ip, inet_ntoa(clientaddr_in.sin_addr), sizeof(ip) - 1);
    ip[sizeof(ip) - 1] = '\0';

    // Definir otros valores
    char protocolo[] = "UDP";
    char puertoEfimero[10];
    snprintf(puertoEfimero, sizeof(puertoEfimero), "%d",ntohs(clientaddr_in.sin_port));
    int puertoCliente = ntohs(clientaddr_in.sin_port);

    // Llamar a la función para escribir en el archivo
    editaFichero(fichero, fecha, nombreHost, ip, protocolo, puertoEfimero, buffer, acumulado, puertoCliente);
}

