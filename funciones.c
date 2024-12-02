#include "funciones.h"  // Incluir la cabecera para usar sus funciones
#include <stdio.h>       // Incluir para uso de FILE, fprintf y demás funciones estándar

// Implementación de la función para editar el fichero de log
void editaFichero(FILE *fichero, RegistroComunicacion *registro)
{
    if (fichero == NULL) {    // Comprobar que el fichero esté abierto
        fprintf(stderr, "Error al editar el fichero\n");
        exit(1);
    }

    fprintf(fichero, "Tiempo del evento %s.\n", registro->fecha);
    // Comunicación realizada
    fprintf(fichero, "COMUNICACIÓN REALIZADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, nº de puerto efímero del cliente: %s.\n", 
            registro->nombreHost, registro->ip, registro->protocolo, registro->puertoEfimero);
    // Orden recibida
    fprintf(fichero, "ORDEN RECIBIDA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, puerto del cliente: %s, orden: %s.\n", 
            registro->nombreHost, registro->ip, registro->protocolo, registro->puertoCliente, registro->orden);
    // Respuesta enviada
    fprintf(fichero, "RESPUESTA ENVIADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, puerto del cliente: %s, respuesta mandada: %s.\n", 
            registro->nombreHost, registro->ip, registro->protocolo, registro->puertoCliente, registro->respuesta);
    // Comunicación finalizada
    fprintf(fichero, "COMUNICACIÓN FINALIZADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, nº de puerto efímero del cliente: %s.\n", 
            registro->nombreHost, registro->ip, registro->protocolo, registro->puertoEfimero);
}

void formatoFinger(char* cadena, size_t tam, DatosFinger datos) {
    // Asegurarnos de que la cadena comience vacía
    cadena[0] = '\0';

    // Formatear la cadena siguiendo el patrón exacto de la imagen
    snprintf(cadena, tam, 
        "Login: %-30sName: %-30s\nDirectory: %-30sShell: %-30s\nOn since %-30s on pts/%-2s from %-15s\n%-30s\nNo mail.\nNo Plan.\n", 
        datos.login, 
        datos.nombre, 
        datos.directory, 
        datos.shell, 
        datos.fecha, 
        datos.pts, 
        datos.ip, 
        datos.tiempo_idle);
}