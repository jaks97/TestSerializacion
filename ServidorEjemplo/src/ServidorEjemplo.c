/*
 ============================================================================
 Name        : ServidorEjemplo.c
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

struct struct_select recibir_select(int cliente){
	struct struct_select paquete;
	void* buffer;

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(u_int));
	recv(cliente, buffer, sizeof(u_int), 0);
	paquete.tamanio_nombre = *((u_int*)buffer);
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = (char*)buffer;
	printf("El nombre de tabla es %s\n", paquete.nombreTabla);
	free(buffer);

	// Lo mismo, pero con la key
	buffer = malloc(sizeof(u_int));
	recv(cliente, buffer, sizeof(u_int), 0);
	paquete.tamanio_key = *((u_int*)buffer);
	printf("La key es de %d bytes\n", paquete.tamanio_key);
	free(buffer);

	buffer = malloc(paquete.tamanio_key);
	recv(cliente, buffer, paquete.tamanio_key, 0);
	paquete.key = (char*)buffer;
	printf("La key es %s\n", paquete.key);
	free(buffer);

	puts("Listo, recibi el paquete completo!");

	return paquete;
}

struct struct_insert recibir_insert(int cliente){
	struct struct_insert paquete;
	void* buffer;

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(u_int));
	recv(cliente, buffer, sizeof(u_int), 0);
	paquete.tamanio_nombre = *((u_int*)buffer);
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = (char*)buffer;
	printf("El nombre de tabla es %s\n", paquete.nombreTabla);
	free(buffer);

	// Lo mismo, pero con la key
	buffer = malloc(sizeof(u_int));
	recv(cliente, buffer, sizeof(u_int), 0);
	paquete.tamanio_key = *((u_int*)buffer);
	printf("La key es de %d bytes\n", paquete.tamanio_key);
	free(buffer);

	buffer = malloc(paquete.tamanio_key);
	recv(cliente, buffer, paquete.tamanio_key, 0);
	paquete.key = (char*)buffer;
	printf("La key es %s\n", paquete.key);
	free(buffer);


	// Por ultimo el valor
	buffer = malloc(sizeof(u_int));
	recv(cliente, buffer, sizeof(u_int), 0);
	paquete.tamanio_valor = *((u_int*)buffer);
	printf("El valor es de %d bytes\n", paquete.tamanio_valor);
	free(buffer);

	buffer = malloc(paquete.tamanio_valor);
	recv(cliente, buffer, paquete.tamanio_valor, 0);
	paquete.valor = (char*)buffer;
	printf("El valor es \"%s\"\n", paquete.valor);
	free(buffer);

	puts("Listo, recibi el paquete completo!");

	return paquete;
}

int main(void) {
	puts("Iniciando servidor");

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080); // Puerto

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	// Estas 2 lineas son para poder reusar un socket en un puerto que ya uso un proceso recientemente terminado
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, &direccionServidor, sizeof(direccionServidor))) {
		puts("Fallo el servidor");
		exit(EXIT_FAILURE); // No seria la manera mas prolija de atender esto
	}

	listen(servidor, SOMAXCONN);
	puts("Escuchando");

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion = sizeof(direccionCliente);
	int cliente = accept(servidor, &direccionCliente, &tamanoDireccion);
	printf("Recibi una conexion en %d\n", cliente);

	while(1){// Recibo el codigo de op
		u_char* cod_op = malloc(sizeof(u_char));
		if(!recv(cliente, cod_op, sizeof(u_char), 0)){
			puts("El cliente se descontecto");
			break;
		}

		switch (*cod_op) {
			case SELECT:
			{
				puts("Recibi un SELECT");
				struct struct_select paquete = recibir_select(cliente);
				// Depues haria lo que tenga que hacer con esta struct ya cargada
			}
			break;
			case INSERT:
			{
				puts("Recibi un INSERT");
				struct struct_insert paquete = recibir_insert(cliente);
				// Depues haria lo que tenga que hacer con esta struct ya cargada
			}
			break;
			case CREATE:
				break;
			case DESCRIBE:
				break;
			case DROP:
				break;
			case JOURNAL:
				break;
			default:
				puts("Recibi una operacion invalida...");
		}
		free(cod_op);
	}

	return EXIT_SUCCESS;
}
