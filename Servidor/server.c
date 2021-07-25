#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

//Aqui nós fazemos o servidor rasteador
int main(int argc, char const *argv[])
{

	//Variáveis tipo inteiras utilizadas
	int socketServer, binder;
	//Variável de vetor do tipo char
	char bufferTracker[100];
	// Variáveis que utilizam a estrutura de sockaddr_in da biblioteca netinet/in.h
	struct sockaddr_in addrServer, addrClient;
	// Variáveis para representar o retorno de funções
	ssize_t readBytes, writeBytes, response;
	//Variável para armazenar o tamanho do socket do cliente
	socklen_t clientLength;

	while (1)
	{
		//Aqui é feito a criação do Socket do servidor
		socketServer = socket(AF_INET, SOCK_DGRAM, 0);

		//Aqui é feito a verificação se o socket foi corretamente criado
		//Caso tenha sido bem sucedido, a função irá retornar 1, se não irá retornar 0 ou -1
		if (socketServer <= 0)
		{
			printf("Ocorreu um erro na abertura do socket: %s\n", strerror(errno));
			exit(1);
		}
		else if (socketServer)
		{
			//Aqui será a mostrada a mensagem que o cliente está aguardando enquanto o cliente não aceita
			do
			{
				printf("Aguardando cliente\n");
			} while (!accept);
		}

		//Aqui é feito a conexão com as porta através da struct de socket
		addrServer.sin_family = AF_INET;
		addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
		addrServer.sin_port = htons(3030);

		//Aqui é realizada a função bind, na qual é atribuido um endereço de soquete a um socket
		//Caso seja bem sucedido a função irá retornar 0, se não irá retornar -1
		binder = bind(socketServer, (struct sockaddr *)&addrServer, sizeof(addrServer));
		//Aqui é feito a verificação se o bind foi feito corretamente
		if (binder < 0)
		{
			printf("Ocorreu um erro no Bind: %s\n", strerror(errno));
			exit(1);
		}

		//Aqui é atribuido o tamanho do addrClient
		clientLength = sizeof(addrClient);

		//Aqui é utilizada a função recvfrom, na qual o intuito é receber uma mensagem do soquete e os dados do soquete
		readBytes = recvfrom(socketServer, &bufferTracker, sizeof(bufferTracker), 0, (struct sockaddr *)&addrClient, &clientLength);

		//Aqui é feito a comparação se o nome do arquivo requisitado no rastreador do buffer é o mesmo
		//Se for, vai enviar o arquivo, se não, não vai
		if (!strcmp(bufferTracker, argv[2]))
		{
			printf("Arquivo localizado! Enviando arquivo!\n");
			response = sendto(socketServer, "2021", 4, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));
		}
		else
		{
			printf("Arquivo não existente!\n");
			response = sendto(socketServer, "0000", 4, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));
		}
		//Feito o fechamento do Socket Servidor
		close(socketServer);
	}
	return 0;
}
