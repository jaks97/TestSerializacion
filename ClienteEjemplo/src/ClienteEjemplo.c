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
#include <string.h>
#include <arpa/inet.h>

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

/*struct struct_insert{
	u_int tamanio_nombre;
	char* nombreTabla;
	u_int tamanio_key;
	char* key;
	u_int tamanio_valor;
	char* valor;
};*/
struct struct_insert{
	u_int tamanio_nombre;
	char* nombreTabla;
	u_int tamanio_key;
	char* key;
	u_int tamanio_valor;
	char* valor;
};

void enviar_select(int cliente, struct struct_select paquete){
	// Primero envio codigo de operacion
	u_char cod_op = SELECT;
	send(cliente, &cod_op, sizeof(u_char), 0);

	// Luego los datos
	size_t tamanio_paquete = sizeof(u_int)*2 + paquete.tamanio_key + paquete.tamanio_nombre; // Calculo el tamanio del paquete
	void* buffer = malloc(tamanio_paquete); // Pido memoria para el tamanio del paquete completo que voy a enviar

	int posbuffer = 0; // Voy a usar esta variable para ir moviendome por el buffer

	// Primero el nombre de la tabla
	memcpy(buffer, &paquete.tamanio_nombre, sizeof(paquete.tamanio_nombre)); // En el comienzo del buffer copio el tamanio del nombre de la tabla
	posbuffer += sizeof(paquete.tamanio_nombre); // Me corro 4 bytes del int
	memcpy(buffer + posbuffer, paquete.nombreTabla, paquete.tamanio_nombre); // En la nueva posicion copio el nombre de la tabla
	posbuffer += paquete.tamanio_nombre; // Me corro la longitud del string

	// Lo mismo para la clave
	memcpy(buffer + posbuffer, &paquete.tamanio_key, sizeof(paquete.tamanio_key));
	posbuffer += sizeof(paquete.tamanio_key);
	memcpy(buffer + posbuffer, paquete.key, paquete.tamanio_key);
	// Al pedo calcular el corrimiento ahora, no voy a enviar mas nada y ademas ya me ocupe todo el buffer

	// Por ultimo envio el paquete y libero el buffer.
	send(cliente, buffer, tamanio_paquete, 0);
	free(buffer);
}

void enviar_insert(int cliente, struct struct_insert paquete){
	// Primero envio codigo de operacion
	u_char cod_op = INSERT;
	send(cliente, &cod_op, sizeof(u_char), 0);

	// Luego los datos
	size_t tamanio_paquete = sizeof(u_int)*3 + paquete.tamanio_key + paquete.tamanio_nombre + paquete.tamanio_valor; // Calculo el tamanio del paquete
	void* buffer = malloc(tamanio_paquete); // Pido memoria para el tamanio del paquete completo que voy a enviar

	int posbuffer = 0; // Voy a usar esta variable para ir moviendome por el buffer

	// Primero el nombre de la tabla
	memcpy(buffer, &paquete.tamanio_nombre, sizeof(paquete.tamanio_nombre)); // En el comienzo del buffer copio el tamanio del nombre de la tabla
	posbuffer += sizeof(paquete.tamanio_nombre); // Me corro 4 bytes del int
	memcpy(buffer + posbuffer, paquete.nombreTabla, paquete.tamanio_nombre); // En la nueva posicion copio el nombre de la tabla
	posbuffer += paquete.tamanio_nombre; // Me corro la longitud del string

	// Lo mismo para la clave
	memcpy(buffer + posbuffer, &paquete.tamanio_key, sizeof(paquete.tamanio_key));
	posbuffer += sizeof(paquete.tamanio_key);
	memcpy(buffer + posbuffer, paquete.key, paquete.tamanio_key);
	posbuffer += paquete.tamanio_key;

	// Lo mismo para el valor
	memcpy(buffer + posbuffer, &paquete.tamanio_valor, sizeof(paquete.tamanio_valor));
	posbuffer += sizeof(paquete.tamanio_valor);
	memcpy(buffer + posbuffer, paquete.valor, paquete.tamanio_valor);
	// Al pedo calcular el corrimiento ahora, no voy a enviar mas nada y ademas ya me ocupe todo el buffer

	// Por ultimo envio el paquete y libero el buffer.
	send(cliente, buffer, tamanio_paquete, 0);
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
	paquete.nombreTabla = "TABLA1"; // 7 bytes
	paquete.tamanio_nombre = strlen(paquete.nombreTabla) + 1; // 4 bytes
	paquete.key = "KEY1"; // 5 bytes
	paquete.tamanio_key = strlen(paquete.key) + 1; // 4 bytes
	paquete.valor = "ALGUN VALOR"; // 12 bytes
	paquete.tamanio_valor = strlen(paquete.valor) + 1; // 4 bytes

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
