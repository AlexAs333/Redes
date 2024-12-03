#include "funciones.h"  // Incluir la cabecera para usar sus funciones
#include <stdio.h>       // Incluir para uso de FILE, fprintf y demás funciones estándar

// Implementación de la función para editar el fichero de log
void editaFichero(FILE *fichero, RegistroComunicacion *registro){
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

char* finger(const char *usuario) {
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
        if (entry->ut_type == USER_PROCESS && strcmp(entry->ut_user, usuario) == 0) {
            // Fecha de inicio de sesión
            time_t tiempo = entry->ut_tv.tv_sec;
            struct tm *tm_info = localtime(&tiempo);
            char fecha[64];
            strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm_info);
            free(datos.fecha);
            datos.fecha = strdup(fecha);

            // Terminal y dirección IP
            free(datos.pts);
            datos.pts = strdup(entry->ut_line);

            free(datos.ip);
            datos.ip = strdup(entry->ut_host);
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