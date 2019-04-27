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
	paquete.tamanio_nombre = *((u_int*)buffer); // Casteo el puntero a void a un puntero a u_int para despues buscar el valor al que apunta
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = malloc(paquete.tamanio_nombre);
	memcpy(paquete.nombreTabla, buffer, paquete.tamanio_nombre);
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
	paquete.key = malloc(paquete.tamanio_key);
	memcpy(paquete.key, buffer, paquete.tamanio_key);
	printf("La key es %s\n", paquete.key);
	free(buffer);

	puts("Listo, recibi el paquete completo!\n");

	return paquete;
	// Para tener en cuenta: El recv recibe un maximo de bytes como diga el tercer parametro,
	// pero puede recibir menos por alguna circunstancia no esperada (si llegaron menos bytes de los que esperaba).
	// La cantidad de bytes recibidos es el valor que devuelve recv, por lo que podemos almacenar y usar ese valor
	// para controlar posibles errores y abortar la operacion.
}

struct struct_insert recibir_insert(int cliente){
	struct struct_insert paquete;
	void* buffer = NULL;

	// Recibo tamanio del nombre de tabla
	buffer = malloc(sizeof(paquete.tamanio_nombre));
	recv(cliente, buffer, sizeof(paquete.tamanio_nombre), 0);
	paquete.tamanio_nombre = *((u_int*)buffer);
	printf("El nombre de tabla es de %d bytes\n", paquete.tamanio_nombre);
	free(buffer);

	// Ahora recibo el nombre de la tabla
	buffer = malloc(paquete.tamanio_nombre);
	recv(cliente, buffer, paquete.tamanio_nombre, 0);
	paquete.nombreTabla = malloc(paquete.tamanio_nombre);
	memcpy(paquete.nombreTabla, buffer, paquete.tamanio_nombre);
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
	paquete.key = malloc(paquete.tamanio_key);
	memcpy(paquete.key, buffer, paquete.tamanio_key);
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
	paquete.valor = malloc(paquete.tamanio_valor);
	memcpy(paquete.valor, buffer, paquete.tamanio_valor);
	printf("El valor es \"%s\"\n", paquete.valor);
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
		u_char cod_op;
		if(!recv(cliente, &cod_op, sizeof(u_char), 0)){
			puts("El cliente se descontecto");
			break;
		}

		switch (cod_op) {
			case SELECT:
			{
				puts("Recibi un SELECT");
				struct struct_select paquete = recibir_select(cliente);

				/*
				 * Depues haria lo que tenga que hacer con esta struct ya cargada
				 */
				printf("Comando recibido: SELECT %s %s\n\n", paquete.nombreTabla, paquete.key);

				// Por ultimo, y sabiendo que no voy a usar mas el paquete, libero la memoria dinamica (MUCHO MUY IMPORTANTE)
				free(paquete.nombreTabla);
				free(paquete.key);
			}
			break;
			case INSERT:
			{
				puts("Recibi un INSERT");
				struct struct_insert paquete = recibir_insert(cliente);

				/*
				 * Depues haria lo que tenga que hacer con esta struct ya cargada
				 */
				printf("Comando recibido: INSERT %s %s \"%s\"\n\n", paquete.nombreTabla, paquete.key, paquete.valor);

				// Por ultimo, y sabiendo que no voy a usar mas el paquete, libero la memoria dinamica (MUCHO MUY IMPORTANTE)
				free(paquete.nombreTabla);
				free(paquete.key);
				free(paquete.valor);
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
	}

	return EXIT_SUCCESS;
}
