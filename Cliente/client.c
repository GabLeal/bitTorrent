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

//Aqui nós fazemos os clientes
int main(int argc, char const *argv[])
{

	//Variáveis tipo inteiras utilizadas
	int port, connector, socketClient;
	//Variável de vetor do tipo char
	char str[4096];
	// Variáveis que utilizam a estrutura de sockaddr_in da biblioteca netinet/in.h
	struct sockaddr_in addrServer;
	// Variáveis para representar o retorno de funções
	ssize_t readBytes, writeBytes;

	//Aqui é feita a verificação se o argc é menor que 3, assim não estando da forma correta
	if (argc < 3)
	{
		printf("Maneira correta: Endereco IP - Porta\n");
		exit(1);
	}

	//Aqui é feito a criação do Socket do cliente
	socketClient = socket(AF_INET, SOCK_STREAM, 0);

	//Aqui é feito a verificação se o socket foi corretamente criado
	//Caso tenha sido bem sucedido, a função irá retornar 1, se não irá retornar 0 ou -1
	if (socketClient <= 0)
	{
		printf("Ocorreu um erro na abertura do socket: %s\n", strerror(errno));
		exit(1);
	}

	//Aqui é utilziado a função bzero, que adiciona n bytes(addrServer) em &addrServer
	bzero(&addrServer, sizeof(addrServer));
	//Aqui é feito a conversão do dado tipo string para o tipo inteiro através da função atoi
	port = atoi(argv[2]);
	//Aqui é feito a conexão com a porta através da struct de socket
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);

	//Aqui utilizamos a função connect para realizar uma conexão com o socket
	//Se a conexão for bem sucedida, irá retornar 0, se não irá retornar -1
	connector = connect(socketClient, (const struct sockaddr *)&addrServer, sizeof(addrServer));
	//Verificação se a conexão foi bem sucedida
	if (connector < 0)
	{
		fprintf(stderr, ". Ocorreu um erro! Não foi possível fazer a conexao! \n");
		exit(1);
	}
	else
	{
		printf("Conexao bem sucedida! Conectado com: %s\n", argv[1]);
	}

	//Aqui é impresso a mensagem
	while (1)
	{
		printf("Mensagem: ");
		//A função fgets lê a string do arquivo
		fgets(str, sizeof(str), stdin);
		//A função write grava no arquivo indicado as informações obtidas
		writeBytes = write(socketClient, str, sizeof(str));
		//Se writeBytes for igual a 0, ou ocorreu algum erro ou não há nada escrito
		if (writeBytes == 0)
		{
			printf("Ocorreu um erro no write: %s\n", strerror(errno));
			printf("Nao ha nada escrito.\n");
		}
		//A função read realiza a leitura dos dados do arquivo para a memória
		readBytes = read(socketClient, str, sizeof(str));
		//Se readBytes for menor ou igual a zero ocorreu algum erro na função read
		if (readBytes <= 0)
		{
			printf("Ocorreu um erro no read: %s\n", strerror(errno));
		}
	}
	//Feito o fechamento do Socket de Cliente
	close(socketClient);

	return 0;
}
