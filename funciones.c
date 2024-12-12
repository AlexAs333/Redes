/*

** Fichero: funciones.c
** Autores:
** Daniel Dominguez Parra DNI 45138288Y
** Alex Asensio Boj DNI 77221233K
*/
#include "funciones.h"
#include <stdio.h> 

void editaFichero(FILE *fichero, const char *fecha, const char *nombreHost, const char *ip,
                  const char *protocolo, const char *puertoEfimero, const char *orden,
                  const char *respuesta, int puertoCliente) {
    if (fichero == NULL) {
        fprintf(stderr, "Error al editar el fichero\n");
        exit(1);
    }

    // Escribir la información al archivo
    fprintf(fichero, "--------------------NUEVO LOG--------------------\n", fecha);
    fprintf(fichero, "Tiempo del evento: %s.\n", fecha);
    // Comunicación realizada
    fprintf(fichero, "COMUNICACIÓN REALIZADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, nº de puerto efímero del cliente: %s.\n", 
            nombreHost, ip, protocolo, puertoEfimero);
    // Orden recibida
    fprintf(fichero, "ORDEN RECIBIDA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, puerto del cliente: %d, orden: %s.\n", 
            nombreHost, ip, protocolo, puertoCliente, orden);
    // Respuesta enviada
    fprintf(fichero, "RESPUESTA ENVIADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, puerto del cliente: %d, respuesta mandada:\n %s.\n", 
            nombreHost, ip, protocolo, puertoCliente, respuesta);
    // Comunicación finalizada
    fprintf(fichero, "COMUNICACIÓN FINALIZADA: nombre del host: %s, dirección IP: %s, protocolo de transporte: %s, nº de puerto efímero del cliente: %s.\n", 
            nombreHost, ip, protocolo, puertoEfimero);
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

char* finger(const char *usuario) {
	//printf("[finger] usuario = %s", usuario);
    static char resultado[TAM_BUFFER]; // Usar una variable estática para la cadena final
    struct passwd *pwd = getpwnam(usuario);
    if (!pwd) {
        return strdup("Usuario no encontrado.\n");
    }

    // Crear una estructura DatosFinger
    DatosFinger datos = {0};

    // Asignar memoria y copiar los valores necesarios
    datos.login = strdup(usuario);
    datos.nombre = strdup(pwd->pw_gecos);
    datos.directory = strdup(pwd->pw_dir);
    datos.shell = strdup(pwd->pw_shell);

    // Buscar sesión activa en el sistema
    struct utmpx *entry;
    setutxent();
    datos.fecha = strdup("Desconocido");
    datos.pts = strdup("?");
    datos.ip = strdup("?");
    datos.tiempo_idle = strdup("?");

    while ((entry = getutxent())) {
        if (entry->ut_type == USER_PROCESS && strncmp(entry->ut_user, usuario, sizeof(entry->ut_user)) == 0) {
            // Fecha de inicio de sesión
            time_t tiempo = entry->ut_tv.tv_sec;
            struct tm *tm_info = localtime(&tiempo);
            char fecha[64];
            strftime(fecha, sizeof(fecha), "%a %b %d %H:%M (%Z)", tm_info);
            free(datos.fecha);
            datos.fecha = strdup(fecha);

            // Terminal y dirección IP
            free(datos.pts);
            datos.pts = strndup(entry->ut_line, sizeof(entry->ut_line) - 1);

            free(datos.ip);
            datos.ip = strndup(entry->ut_host, sizeof(entry->ut_host) - 1);
            break;
        }
    }
    endutxent();

    // Generar la salida formateada
    formatoFinger(resultado, TAM_BUFFER, datos);

    // Liberar memoria asignada en DatosFinger
    free(datos.login);
    free(datos.nombre);
    free(datos.directory);
    free(datos.shell);
    free(datos.fecha);
    free(datos.tiempo_idle);
    free(datos.ip);
    free(datos.pts);

    // Retornar la cadena generada
    return resultado;
}
