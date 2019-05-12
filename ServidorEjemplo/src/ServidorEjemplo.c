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
	uint16_t key;
}struct_select;

typedef struct{
	uint16_t tamanio_nombre;
	char* nombreTabla;
	uint16_t key;
	uint16_t tamanio_valor;
	char* valor;
}struct_insert;

// Capaz es mejor no tener los tamanios en las structs y calcularos solo al enviar?
// No se, aca pruebo esta opcion, entonces la logica de las funciones de describe es un tanto distinta a las otras.
typedef struct{
	char* nombreTabla;
}struct_describe;


struct_select recibir_select(int cliente){
	struct_select paquete;
	void* buffer;

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(uint16_t));
	recv(cliente, buffer, sizeof(uint16_t), 0);
	paquete.tamanio_nombre = *((uint16_t*)buffer); // Casteo el puntero a void a un puntero a uint para despues buscar el valor al que apunta
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = malloc(paquete.tamanio_nombre);
	memcpy(paquete.nombreTabla, buffer, paquete.tamanio_nombre);
	printf("El nombre de tabla es %s\n", paquete.nombreTabla);
	free(buffer);

	// Ahora recibo la key
	buffer = malloc(sizeof(paquete.key));
	recv(cliente, buffer, sizeof(paquete.key), 0);
	paquete.key = *((uint16_t*)buffer); // Casteo el puntero a void a un puntero a uint para despues buscar el valor al que apunta
	printf("La key es %d\n", paquete.key);
	free(buffer);

	puts("Listo, recibi el paquete completo!\n");

	return paquete;
	// Para tener en cuenta: El recv recibe un maximo de bytes como diga el tercer parametro,
	// pero puede recibir menos por alguna circunstancia no esperada (si llegaron menos bytes de los que esperaba).
	// La cantidad de bytes recibidos es el valor que devuelve recv, por lo que podemos almacenar y usar ese valor
	// para controlar posibles errores y abortar la operacion.
}

struct_insert recibir_insert(int cliente){
	struct_insert paquete;
	void* buffer = NULL;

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(paquete.tamanio_nombre));
	recv(cliente, buffer, sizeof(paquete.tamanio_nombre), 0);
	paquete.tamanio_nombre = *((uint16_t*)buffer);
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = malloc(paquete.tamanio_nombre);
	memcpy(paquete.nombreTabla, buffer, paquete.tamanio_nombre);
	printf("El nombre de tabla es %s\n", paquete.nombreTabla);
	free(buffer);

	// Ahora recibo la key
	buffer = malloc(sizeof(paquete.key));
	recv(cliente, buffer, sizeof(uint16_t), 0);
	paquete.key = *((uint16_t*)buffer); // Casteo el puntero a void a un puntero a uint para despues buscar el valor al que apunta
	printf("La key es %d\n", paquete.key);
	free(buffer);

	// Por ultimo el valor
	buffer = malloc(sizeof(uint16_t));
	recv(cliente, buffer, sizeof(uint16_t), 0);
	paquete.tamanio_valor = *((uint16_t*)buffer);
	printf("El valor es de %d bytes\n", paquete.tamanio_valor);
	free(buffer);

	buffer = malloc(paquete.tamanio_valor);
	recv(cliente, buffer, paquete.tamanio_valor, 0);
	paquete.valor = malloc(paquete.tamanio_valor);
	memcpy(paquete.valor, buffer, paquete.tamanio_valor);
	printf("El valor es \"%s\"\n", paquete.valor);
	free(buffer);

	puts("Listo, recibi el paquete completo!\n");

	return paquete;
}

struct_describe recibir_describe(int cliente){
	struct_describe paquete;
	void* buffer;
	uint16_t tamanio_string; // Uso esta variable para almacenar los tamanios de los string que vaya a ir recibiendo

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(uint16_t));
	recv(cliente, buffer, sizeof(uint16_t), 0);
	tamanio_string = *((uint16_t*)buffer); // Casteo el puntero a void a un puntero a uint para despues buscar el valor al que apunta
	printf("El nombre de tabla es de %d bytes\n", tamanio_string);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(tamanio_string);
	recv(cliente, buffer,tamanio_string, 0);
	paquete.nombreTabla = malloc(tamanio_string);
	memcpy(paquete.nombreTabla, buffer, tamanio_string);
	printf("El nombre de tabla es %s\n", paquete.nombreTabla);
	free(buffer);

	puts("Listo, recibi el paquete completo!\n");

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

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))) {
		puts("Fallo el servidor");
		exit(EXIT_FAILURE); // No seria la manera mas prolija de atender esto
	}

	listen(servidor, SOMAXCONN);
	puts("Escuchando");

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion = sizeof(direccionCliente);
	int cliente = accept(servidor, (void*) &direccionCliente, &tamanoDireccion);
	printf("Recibi una conexion en %d\n", cliente);

	while(1){
		// Recibo el codigo de op
		uint8_t cod_op;
		if(!recv(cliente, &cod_op, sizeof(uint8_t), 0)){ // Problema, recv es no bloqueante, asi que estoy en espera activa hasta
													  	 // que se desconecte el cliente o reciba algo. Deberiamos usar select()?
			puts("El cliente se desconecto");
			break;
		}

		switch (cod_op) {
			case SELECT:
			{
				puts("Recibi un SELECT");
				struct_select paquete = recibir_select(cliente);

				/*
				 * Depues haria lo que tenga que hacer con esta struct ya cargada
				 */
				printf("Comando recibido: SELECT %s %d\n\n", paquete.nombreTabla, paquete.key);

				// Por ultimo, y sabiendo que no voy a usar mas el paquete, libero la memoria dinamica (MUCHO MUY IMPORTANTE)
				free(paquete.nombreTabla);
			}
			break;
			case INSERT:
			{
				puts("Recibi un INSERT");
				struct_insert paquete = recibir_insert(cliente);

				/*
				 * Depues haria lo que tenga que hacer con esta struct ya cargada
				 */
				printf("Comando recibido: INSERT %s %d \"%s\"\n\n", paquete.nombreTabla, paquete.key, paquete.valor);

				// Por ultimo, y sabiendo que no voy a usar mas el paquete, libero la memoria dinamica (MUCHO MUY IMPORTANTE)
				free(paquete.nombreTabla);
				free(paquete.valor);
			}
			break;
			case CREATE:
				break;
			case DESCRIBE:
			{
				puts("Recibi un DESCRIBE");
				struct_describe paquete = recibir_describe(cliente);

				/*
				 * Depues haria lo que tenga que hacer con esta struct ya cargada
				 */
				printf("Comando recibido: DESCRIBE %s\n\n", paquete.nombreTabla);

				// Por ultimo, y sabiendo que no voy a usar mas el paquete, libero la memoria dinamica (MUCHO MUY IMPORTANTE)
				free(paquete.nombreTabla);
			}
			break;
			case DROP:
				break;
			case JOURNAL:
				break;
			default:
				puts("Recibi una operacion invalida...");
		}
	}
	close(cliente); // No me olvido de cerrar el socket que ya no voy a usar mas
	close(servidor); // No me olvido de cerrar el socket que ya no voy a usar mas

	return EXIT_SUCCESS;
}
