/*
 ============================================================================
 Name        : ClienteEjemplo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

enum operaciones{
	SELECT = 1,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP,
	JOURNAL
};

struct struct_select{
	u_int tamanio_nombre;
	char* nombreTabla;
	u_int tamanio_key;
	char* key;
};

struct struct_insert{
	u_int tamanio_nombre;
	char* nombreTabla;
	u_int tamanio_key;
	char* key;
	u_int tamanio_valor;
	char* valor;
};

void enviar_select(int cliente, struct struct_select paquete){
	// Primero codigo de operacion
	u_char* cod_op = malloc(sizeof(u_char));
	*cod_op = SELECT;
	send(cliente, cod_op, sizeof(u_char), 0);
	free(cod_op);


	// Luego los datos
	void* buffer;

	// Primero tamanio del nombre de tabla
	buffer = malloc(sizeof(paquete.tamanio_nombre));
	*((u_int*)buffer) = paquete.tamanio_nombre;
	send(cliente, buffer, sizeof(paquete.tamanio_nombre), 0);
	free(buffer);

	// Despues el nombre de la tabla
	buffer = strdup(paquete.nombreTabla);
	send(cliente, buffer, paquete.tamanio_nombre, 0);
	free(buffer);

	// Despues la key
	buffer = malloc(sizeof(paquete.tamanio_key));
	*((u_int*)buffer) = paquete.tamanio_key;
	send(cliente, buffer, sizeof(paquete.tamanio_key), 0);
	free(buffer);

	buffer = strdup(paquete.key);
	send(cliente, buffer, paquete.tamanio_key, 0);
	free(buffer);
}

void enviar_insert(int cliente, struct struct_insert paquete){
	// Primero codigo de operacion
	u_char* cod_op = malloc(sizeof(u_char));
	*cod_op = INSERT;
	send(cliente, cod_op, sizeof(u_char), 0);
	free(cod_op);


	// Luego los datos
	void* buffer;

	// Primero nombre de tabla (y su tamanio)
	buffer = malloc(sizeof(paquete.tamanio_nombre));
	*((u_int*)buffer) = paquete.tamanio_nombre;
	send(cliente, buffer, sizeof(paquete.tamanio_nombre), 0);
	free(buffer);

	buffer = strdup(paquete.nombreTabla);
	send(cliente, buffer, paquete.tamanio_nombre, 0);
	free(buffer);

	// Despues la key
	buffer = malloc(sizeof(paquete.tamanio_key));
	*((u_int*)buffer) = paquete.tamanio_key;
	send(cliente, buffer, sizeof(paquete.tamanio_key), 0);
	free(buffer);

	buffer = strdup(paquete.key);
	send(cliente, buffer, paquete.tamanio_key, 0);
	free(buffer);

	// Por ultimo el valor
	buffer = malloc(sizeof(paquete.tamanio_valor));
	*((u_int*)buffer) = paquete.tamanio_valor;
	send(cliente, buffer, sizeof(paquete.tamanio_valor), 0);
	free(buffer);

	buffer = strdup(paquete.valor);
	send(cliente, buffer, paquete.tamanio_valor, 0);
	free(buffer);
}

int main(void) {
	puts("Iniciando cliente");

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family= AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1"); // Direccion IP
	direccionServidor.sin_port = htons(8080); // PUERTO

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	puts("Conectando con servidor (FS)");
	if (connect(cliente, &direccionServidor, sizeof(direccionServidor))) {
		puts("No se pudo conectar con el servidor (FS)");
		exit(EXIT_FAILURE); // No seria la manera mas prolija de atender esto
	}

	// Armo un INSERT TABLA1 KEY1 "ALGUN VALOR"
	struct struct_insert paquete;
	paquete.nombreTabla = "TABLA1";
	paquete.tamanio_nombre = strlen(paquete.nombreTabla) + 1;
	paquete.key = "KEY1";
	paquete.tamanio_key = strlen(paquete.key) + 1;
	paquete.valor = "ALGUN VALOR";
	paquete.tamanio_valor = strlen(paquete.valor) + 1;

	// Lo envio
	enviar_insert(cliente, paquete);


	// Armo un SELECT TABLA1 KEY1
	struct struct_select paquete2;
	paquete2.nombreTabla = "TABLA1";
	paquete2.tamanio_nombre = strlen(paquete2.nombreTabla) + 1;
	paquete2.key = "KEY1";
	paquete2.tamanio_key = strlen(paquete2.key) + 1;

	// Lo envio
	enviar_select(cliente, paquete2);



	return EXIT_SUCCESS;
}
