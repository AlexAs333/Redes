/*

** Fichero: funciones.h
** Autores:
** Daniel Dominguez Parra DNI 45138288Y
** Alex Asensio Boj DNI
*/
#ifndef FUNCIONES_H  // Protección de inclusión
#define FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>
#include <pwd.h>
#define TAM_BUFFER 1024

// Definición de la estructura para almacenar los datos de comunicación
typedef struct {
    char *fecha;
    char *nombreHost;
    char *ip;
    char *protocolo;
    char *puertoEfimero;
    char *orden;
    char *respuesta;
    char *puertoCliente;
} RegistroComunicacion;

typedef struct{
    char* login; 
    char* nombre; 
    char* directory; 
    char* shell; 
    char* fecha; 
    char* tiempo_idle; 
    char* ip; 
    char* pts; 
} DatosFinger;


// Declaración de la función para editar el fichero de log
void editaFichero(FILE *fichero, RegistroComunicacion *registro);

void formatoFinger(char* cadena, size_t tam, DatosFinger datos);    //tam será el tamaño del buffer

char* finger(const char *usuario);

#endif // FUNCIONES_H
