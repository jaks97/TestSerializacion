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

typedef struct{
	uint16_t tamanio_nombre;
	char* nombreTabla;
	uint16_t tamanio_key;
	char* key;
}struct_select;

typedef struct{
	uint16_t tamanio_nombre;
	char* nombreTabla;
	uint16_t tamanio_key;
	char* key;
	uint16_t tamanio_valor;
	char* valor;
}struct_insert;

// Capaz es mejor no tener los tamanios en las structs y calcularos solo al enviar?
// No se, aca pruebo esta opcion, entonces la logica de las funciones de describe es un tanto distinta a las otras.
typedef struct{
	char* nombreTabla;
}struct_describe;

void enviar_select(int cliente, struct_select paquete){
	const uint8_t cod_op = SELECT;

	// Armo paquete con los datos
	size_t tamanio_paquete = sizeof(cod_op) + sizeof(uint16_t)*2 + paquete.tamanio_key + paquete.tamanio_nombre; // Calculo el tamanio del paquete
	void* buffer = malloc(tamanio_paquete); // Pido memoria para el tamanio del paquete completo que voy a enviar

	int desplazamiento = 0; // Voy a usar esta variable para ir moviendome por el buffer

	// Primero codigo de operacion
	memcpy(buffer, &cod_op, sizeof(cod_op)); // En el comienzo del buffer copio el codigo de operacion
	desplazamiento += sizeof(cod_op); // Me corro 1 byte del uint8_t

	// Ahora el nombre de la tabla
	memcpy(buffer + desplazamiento, &paquete.tamanio_nombre, sizeof(paquete.tamanio_nombre)); // En el comienzo del buffer copio el tamanio del nombre de la tabla
	desplazamiento += sizeof(paquete.tamanio_nombre); // Me corro 2 bytes del uint16
	memcpy(buffer + desplazamiento, paquete.nombreTabla, paquete.tamanio_nombre); // En la nueva posicion copio el nombre de la tabla
	desplazamiento += paquete.tamanio_nombre; // Me corro la longitud del string

	// Lo mismo para la clave
	memcpy(buffer + desplazamiento, &paquete.tamanio_key, sizeof(paquete.tamanio_key));
	desplazamiento += sizeof(paquete.tamanio_key);
	memcpy(buffer + desplazamiento, paquete.key, paquete.tamanio_key);
	// Al pedo calcular el desplazamiento ahora, no voy a enviar mas nada y ademas ya me ocupe todo el buffer

	// Por ultimo envio el paquete y libero el buffer.
	send(cliente, buffer, tamanio_paquete, 0); // Hago un solo send para todo, asi nos aseguramos que el paquete llega en orden
	free(buffer);
}

void enviar_insert(int cliente, struct_insert paquete){
	const uint8_t cod_op = INSERT;

	// Armo paquete con los datos
	size_t tamanio_paquete = sizeof(cod_op) + sizeof(uint16_t)*3 + paquete.tamanio_key + paquete.tamanio_nombre + paquete.tamanio_valor; // Calculo el tamanio del paquete
	void* buffer = malloc(tamanio_paquete); // Pido memoria para el tamanio del paquete completo que voy a enviar

	int desplazamiento = 0; // Voy a usar esta variable para ir moviendome por el buffer

	// Primero codigo de operacion
	memcpy(buffer, &cod_op, sizeof(cod_op)); // En el comienzo del buffer copio el codigo de operacion
	desplazamiento += sizeof(cod_op); // Me corro 1 byte del uint8_t

	// Ahora el nombre de la tabla
	memcpy(buffer + desplazamiento, &paquete.tamanio_nombre, sizeof(paquete.tamanio_nombre)); // En el comienzo del buffer copio el tamanio del nombre de la tabla
	desplazamiento += sizeof(paquete.tamanio_nombre); // Me corro 2 bytes del uint16
	memcpy(buffer + desplazamiento, paquete.nombreTabla, paquete.tamanio_nombre); // En la nueva posicion copio el nombre de la tabla
	desplazamiento += paquete.tamanio_nombre; // Me corro la longitud del string

	// Lo mismo para la clave
	memcpy(buffer + desplazamiento, &paquete.tamanio_key, sizeof(paquete.tamanio_key));
	desplazamiento += sizeof(paquete.tamanio_key);
	memcpy(buffer + desplazamiento, paquete.key, paquete.tamanio_key);
	desplazamiento += paquete.tamanio_key;

	// Lo mismo para el valor
	memcpy(buffer + desplazamiento, &paquete.tamanio_valor, sizeof(paquete.tamanio_valor));
	desplazamiento += sizeof(paquete.tamanio_valor);
	memcpy(buffer + desplazamiento, paquete.valor, paquete.tamanio_valor);
	// Al pedo calcular el desplazamiento ahora, no voy a enviar mas nada y ademas ya me ocupe todo el buffer

	// Por ultimo envio el paquete y libero el buffer.
	send(cliente, buffer, tamanio_paquete, 0); // Hago un solo send para todo, asi nos aseguramos que el paquete llega en orden
	free(buffer);
}

void enviar_describe(int cliente, struct_describe paquete){
	const uint8_t cod_op = DESCRIBE;

	uint16_t tamanio_nombre = strlen(paquete.nombreTabla)+1; // Calculo el tamanio del nombre

	// Armo paquete con los datos
	size_t tamanio_paquete = sizeof(cod_op) + sizeof(uint16_t) + tamanio_nombre; // Calculo el tamanio del paquete
	void* buffer = malloc(tamanio_paquete); // Pido memoria para el tamanio del paquete completo que voy a enviar

	int desplazamiento = 0; // Voy a usar esta variable para ir moviendome por el buffer

	// Primero codigo de operacion
	memcpy(buffer, &cod_op, sizeof(cod_op)); // En el comienzo del buffer copio el codigo de operacion
	desplazamiento += sizeof(cod_op); // Me corro 1 byte del uint8_t

	// Ahora el nombre de la tabla
	memcpy(buffer + desplazamiento, &tamanio_nombre, sizeof(tamanio_nombre)); // En el comienzo del buffer copio el tamanio del nombre de la tabla
	desplazamiento += sizeof(tamanio_nombre); // Me corro 2 bytes del uint16
	memcpy(buffer + desplazamiento, paquete.nombreTabla, tamanio_nombre); // En la nueva posicion copio el nombre de la tabla
	// Al pedo calcular el desplazamiento ahora, no voy a enviar mas nada y ademas ya me ocupe todo el buffer

	// Por ultimo envio el paquete y libero el buffer.
	send(cliente, buffer, tamanio_paquete, 0); // Hago un solo send para todo, asi nos aseguramos que el paquete llega en orden
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
	if (connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor))) {
		puts("No se pudo conectar con el servidor (FS)");
		exit(EXIT_FAILURE); // No seria la manera mas prolija de atender esto
	}

	// Armo un INSERT TABLA1 KEY1 "ALGUN VALOR"
	struct_insert paquete;
	paquete.nombreTabla = "TABLA1"; // 7 bytes
	paquete.tamanio_nombre = strlen(paquete.nombreTabla) + 1; // 2 bytes
	paquete.key = "KEY1"; // 5 bytes
	paquete.tamanio_key = strlen(paquete.key) + 1; // 2 bytes
	paquete.valor = "ALGUN VALOR"; // 12 bytes
	paquete.tamanio_valor = strlen(paquete.valor) + 1; // 2 bytes

	// Lo envio
	puts("Enviando INSERT");
	enviar_insert(cliente, paquete);
	// En este caso uso todos string literals, asi que no uso memoria dinamica. Pero si lo hiciese, hay que liberarla

	// Armo un SELECT TABLA1 KEY1
	struct_select paquete2;
	paquete2.nombreTabla = "TABLA1";
	paquete2.tamanio_nombre = strlen(paquete2.nombreTabla) + 1;
	paquete2.key = "KEY1";
	paquete2.tamanio_key = strlen(paquete2.key) + 1;

	// Lo envio
	puts("Enviando SELECT");
	enviar_select(cliente, paquete2);
	// En este caso uso todos string literals, asi que no uso memoria dinamica. Pero si lo hiciese, hay que liberarla

	// Armo un DESCRIBE TABLA1 (con logica ligeramente distinta)
	struct_describe paquete3;
	paquete3.nombreTabla = "TABLA1";

	// Lo envio
	puts("Enviando DESCRIBE");
	enviar_describe(cliente, paquete3);
	// En este caso uso todos string literals, asi que no uso memoria dinamica. Pero si lo hiciese, hay que liberarla

	puts("Termine");
	close(cliente); // No me olvido de cerrar el socket que ya no voy a usar
	return EXIT_SUCCESS;
}
