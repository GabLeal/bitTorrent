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
#include <pthread.h>

//Definição das variáveis globais
#define SizeBuffer 1024
#define TEMPORIZADOR 1

//Variáveis tipo inteiras utilizadas
int nArg, sock, limitTime, timer, recivied, trackerServerPort, portServer, socketServer, option = 0;
//Varíavel do tipo arquivo
FILE *inputFile;
//Variável de vetor do tipo char
char fileName[200], responseBuffer[2], serverIp[15];
// Variáveis que utilizam a estrutura de sockaddr_in da biblioteca netinet/in.h
struct sockaddr_in addrServer, addrClient, addrServerRemote;
//Struct do tipo de Thread utilizadas
pthread_t timerThread, recognitionThread;

//Aqui nessa função nós geramos o checksum
char checksum(char data[], int size)
{

  int i;
  char checksum = 0;

  //Aqui nós realizamos um loop até o tamanho máximo
  for (i = 0; i < size; i++)
  {

    //Dentro do loop, o checksum recebe todos os dados de data
    checksum += data[i];
    //Aqui é realizada a operação de bitwise
    checksum &= 127;
  }
  //Retornamos o Checksum
  return checksum;
}

//Aqui nessa função nós realizamos a validação do checksum
int validateChecksum(char buffer[], int size)
{

  int i;
  char check = 0;

  //Aqui nós realizamos um loop até o tamanho máximo - 2
  for (i = 0; i < size - 2; i++)
  {

    //Dentro do loop, a variável comparadora do checksum recebe todos os dados de data
    check += buffer[i];
    //Aqui é realizada a operação de bitwise
    check &= 127;
  }
  //Aqui é realizado o print do Checksum
  printf("\n\n Verificação do Checksum: \n O checksum esperado foi: 0x%x\nO checksum real foi: 0x%x\n", buffer[size - 1], check);

  //Se o valor no buffer for igual ao check, é porque ocorreu tudo certo, o checksum foi igual e o pacote foi entregue corretamente
  if (buffer[size - 1] == (check))
  {
    printf("Status do pacote: Correto!\n\n");
    return 1;
  }
  else
  {
    printf("Status do pacote: Ocorreu um erro\n\n");
    return 0;
  }
}

//Essa função é responsável por fazer a temporização, os tempos de espera
void *timerFunction()
{
  sleep(TEMPORIZADOR);
  limitTime = 1;
  recivied = 0;
  timer = 0;
}

//Essa função realiza o cancelamento da thread
int pthread_cancel(pthread_t thread);

//Essa função espera por reconhecimento
void *recogntionFunction()
{
  ssize_t response;
  //Aqui é definido o tamanho do Servidor Remoto
  int addrLength = sizeof(addrServerRemote);
  //Aqui é utilizada a função recvfrom, na qual o intuito é receber uma mensagem do soquete e os dados do soquete
  response = recvfrom(socketServer, &responseBuffer, 2, 0, (struct sockaddr *)&addrServerRemote, &addrLength);
  recivied = 1;
  timer = 0;
}

//Essa função é responsável por ler o arquivo
void readFile(char fileName[])
{
  
  //Aqui é feita a abertura do arquivo no modo de leitura
  inputFile = fopen(fileName, "rb");
  //Feita a verificação que se dentro da variável inputFile for vazia, é porque o arquivo não existe
  if (!inputFile)
  {
    printf("Ocorreu um erro para ler! O arquivo não existe\n");
  }
}

//Essa função é responsável por escrever no arquivo
void writeFile(char fileName[])
{
  char src[10] = "_1";
  //Aqui é feita a concatenção das duas strings
  strcat(fileName, src);

  //É feito a abertura do arquivo em modo de escrita
  inputFile = fopen(fileName, "wb");
  //Feita a verificação que se dentro da variável inputFile for vazia, é porque o arquivo não existe
  if (!inputFile)
  {
    printf("Ocorreu um erro para escrever! O arquivo não existe\n");
  }
}

//Aqui nós temos a função de cliente
void *clientFunction()
{

  scanf("%d", &option);
  if (option == 1)
  {
    //Variáveis tipo inteiras utilizadas
    int socketClient, receivedBytes, binder;
    // Variáveis para representar o retorno de funções
    ssize_t readBytes, writeBytes, response;
    // Variável que utilizam a estrutura de sockaddr_in da biblioteca netinet/in.h
    struct sockaddr_in addrServer;
    //Variável que utilizam a estrutura de hostent da biblioteca netdb.h
    struct hostent *h;
    //Variável de vetor do tipo char
    char inputBuffer[SizeBuffer + 3], responseBuffer[2], trackerBuffer[4], packageNumber = 0;

    //Aqui é feita a conexao com o servidor rastreador
    //Verificação se o nArg é maior que 2, se não for é porque está na forma errada
    if (nArg < 2)
    {
      printf("Forma incorreta! O Uso correto e: Endereco IP - Porta\n");
      exit(1);
    }

    //A função getHostByName retorna a estrutura de hostent do host através de um endereço Ip
    h = gethostbyname(serverIp);

    //Faz a verificação se não retornou nulo ou vazio da função de getHostByName
    if (h == NULL)
    {
      printf("Erro no nome do host!\n");
      exit(1);
    }

    //A estrutura de socket recebe o valor de H
    addrServerRemote.sin_family = h->h_addrtype;
    //Copia o conteudo h_addr_list para s_addr
    memcpy((char *)&addrServerRemote.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    //Definição da porta de conexão
    addrServerRemote.sin_port = htons(3030);

    //Aqui é a criação do socket
    socketClient = socket(AF_INET, SOCK_DGRAM, 0);
    //Aqui é feito a verificação se o socket foi corretamente criado
    //Caso tenha sido bem sucedido, a função irá retornar 1, se não irá retornar 0 ou -1
    if (socketClient < 0)
    {
      printf("Ocorreu um erro na abertura do socket! \n");
      exit(1);
    }

    //Aqui é feita a conexão com qualquer porta
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(0);

    //Aqui é realizada a função bind, na qual é atribuido um endereço de soquete a um socket
    //Caso seja bem sucedido a função irá retornar 0, se não irá retornar -1
    binder = bind(socketClient, (struct sockaddr *)&addrServer, sizeof(addrServer));
    //Aqui é feito a verificação se o bind foi feito corretamente
    if (binder < 0)
    {
      printf("Ocorreu um erro no bind, não foi possível conectar a porta! \n");
      exit(1);
    }

    //Aqui é feito a requisição do arquivo ao servidor rastreador
    printf("Insira o nome do arquivo a ser requisitado:\n");
    scanf("%s", fileName);

    //Aqui é feito o envio do arquivo
    response = sendto(socketClient, fileName, sizeof(fileName), 0, (struct sockaddr *)&addrServerRemote, sizeof(addrServerRemote));

    //Aqui é pego o tamanho do servidor remoto
    int addrLength = sizeof(addrServerRemote);

    //Aqui é utilizada a função recvfrom, na qual o intuito é receber uma mensagem do soquete e os dados do soquete
    receivedBytes = recvfrom(socketClient, &trackerBuffer, 4, 0, (struct sockaddr *)&addrServerRemote, &addrLength);
    close(socketClient);

    //Aqui é feito a verificação da resposta do servidor rastreador
    if (!strcmp(trackerBuffer, "0000"))
    {
      printf("Arquivo não foi encontrado\n");
      return 0;
    }


    //Aqui é feito a conexão com outro cliente torrent ***
    //A função getHostByName retorna a estrutura de hostent do host através de um endereço Ip
    h = gethostbyname(serverIp);

    //Faz a verificação se não retornou nulo ou vazio da função de getHostByName
    if (h == NULL)
    {
      printf("Erro no nome do host!\n");
      exit(1);
    }

    //A estrutura de socket recebe o valor de H
    addrServerRemote.sin_family = h->h_addrtype;
    //Copia o conteudo h_addr_list para s_addr
    memcpy((char *)&addrServerRemote.sin_addr.s_addr,h->h_addr_list[0], h->h_length);
    //Definição da porta de conexão
    addrServerRemote.sin_port = htons(atoi(trackerBuffer));

    //Aqui é feito a criação do socket
    socketClient = socket(AF_INET, SOCK_DGRAM, 0);
    //Aqui é feito a verificação se o socket foi corretamente criado
	  //Caso tenha sido bem sucedido, a função irá retornar 1, se não irá retornar 0 ou -1
    if (socketClient < 0)
    {
      printf("Ocorreu um erro na abertura do socket\n");
      exit(1);
    }
  
	  //Aqui é feito a conexão com a porta através da struct de socket
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(0);

    //Aqui é realizada a função bind, na qual é atribuido um endereço de soquete a um socket
		//Caso seja bem sucedido a função irá retornar 0, se não irá retornar -1
    binder = bind(socketClient, (struct sockaddr *)&addrServer, sizeof(addrServer));
		//Aqui é feito a verificação se o bind foi feito corretamente
    if (binder < 0)
    {
      printf("Ocorreu um erro no bind, não foi possível conectar a porta! \n");
      exit(1);
    }

    //Aqui é pego o tamanho do servidor remoto
    addrLength = sizeof(addrServerRemote);
    
    //Aqui é feito o envio do arquivo
    writeBytes = sendto(socketClient, fileName, sizeof(fileName), 0, (struct sockaddr *)&addrServerRemote, sizeof(addrServerRemote));

    //Se o writeBytes for igual a zero é porque ocorreu algum erro
    if (writeBytes == 0)
    {
      printf("Erro no envio do nome do arquivo\n");
      //Se ocorreu um erro, é feito o fechamento do socket do Cliente
      close(socketClient);
    }

    //Faz a gravação no arquivo
    writeFile(fileName);

    //Aqui é feito um while para ser feito a transferência dos pacotes
    while (1)
    {

      //Aqui é recebido o pacote no inputBuffer
      receivedBytes = recvfrom(socketClient, &inputBuffer, SizeBuffer + 3, 0, (struct sockaddr *)&addrServerRemote, &addrLength);
      //Feito a escrita no arquivo
      fwrite(&inputBuffer, sizeof(char), receivedBytes - 3, inputFile);

      //Aqui verifica o numero de sequencia
      if (packageNumber > inputBuffer[receivedBytes - 1])
      {
        printf("Foi detectado um pacote duplicado\n");
        printf("Envio de reconhecimento ACK\n");
        responseBuffer[0] = 1;
        responseBuffer[1] = packageNumber;
        //Feito o envio do pacote
        response = sendto(socketClient, responseBuffer, 2, 0, (struct sockaddr *)&addrServerRemote, sizeof(addrServerRemote));
        continue;
      }

      //Aqui é feita a validação do checksum
      if (!validateChecksum(inputBuffer, receivedBytes - 1))
      {
        printf("O numero do NACK e = %d\n", packageNumber);
        responseBuffer[0] = 2;
        responseBuffer[1] = packageNumber;
        //Feito o envio do pacote
        response = sendto(socketClient, responseBuffer, 2, 0, (struct sockaddr *)&addrServerRemote, sizeof(addrServerRemote));
        continue;
      }
      else
      {
        printf("O numero do ACK e= %d\n", packageNumber);
        responseBuffer[0] = 1;
        responseBuffer[1] = packageNumber;
        //Feito o envio do pacote
        response = sendto(socketClient, responseBuffer, 2, 0, (struct sockaddr *)&addrServerRemote, sizeof(addrServerRemote));
      }

      packageNumber = packageNumber + 1;
      packageNumber %= 128;

      //Se o for o ultimo pacote, é fechado o arquivo
      if (inputBuffer[receivedBytes - 3] == '1')
      {
        fclose(inputFile);
        break;
      }
    }
	  //Feito o fechamento do Socket de Cliente
    close(socketClient);
  }
  return 0;
}


//Aqui fica a Função de Servidor
void *server_function()
{
  //Variáveis tipo inteiras utilizadas
  int binder, bytesSend, bytesLeft, rc, transferCompleted = 0, amountOfBytesSent = 0, amountOfPackageSent = 0;
	//Variável de vetor do tipo char
  char sendBuffer[SizeBuffer + 3], packageCounter = 0;
  // Variáveis para representar o retorno de funções
  ssize_t readBytes, writeBytes, response;
	//Variável para armazenar o tamanho do socket do cliente
  socklen_t clilen;

  //Conexao com o cliente
  //Verificação se o nArg é maior que 2, se não for é porque está na forma errada
  if (nArg < 2)
  {
    printf("Forma incorreta! O Uso correto e: Endereco IP - Porta\n");
    exit(1);
  }

  //Aqui é feito a criação do socket
  socketServer = socket(AF_INET, SOCK_DGRAM, 0);
  //Aqui é feito a verificação se o socket foi corretamente criado
	//Caso tenha sido bem sucedido, a função irá retornar 1, se não irá retornar 0 ou -1
  if (socketServer <= 0)
  {
    printf("Ocorreu um erro na abertura do socket: %s\n", strerror(errno));
    exit(1);
  }
  //Caso tenha sido bem sucedido é continuado a execução
  else if (socketServer)
  {
    printf("=== Cliente torrent em espera ===\n");
    printf("Digite '1' para buscar um arquivo.\n");
  }

  //Aqui é feito a conexão com a porta através da struct de socket
  addrServer.sin_family = AF_INET;
  addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
  addrServer.sin_port = htons(portServer);

  //Aqui é realizada a função bind, na qual é atribuido um endereço de soquete a um socket
	//Caso seja bem sucedido a função irá retornar 0, se não irá retornar -1
  binder = bind(socketServer, (struct sockaddr *)&addrServer, sizeof(addrServer));
  //Aqui é feito a verificação se o bind foi feito corretamente
  if (binder < 0)
  {
    printf("Ocorreu um erro no bind, não foi possível conectar a porta! : %s\n", strerror(errno));
    exit(1);
  }

  //Aqui é pego o tamanho do servidor de cliente
  clilen = sizeof(addrClient);

  //Aqui é utilizada a função recvfrom, na qual o intuito é receber uma mensagem do soquete e os dados do soquete
  readBytes = recvfrom(socketServer, &fileName, sizeof(fileName), 0, (struct sockaddr *)&addrClient, &clilen);

  //Aqui é lido o arquivo
  readFile(fileName);

  //Aqui é verificado o tamnho do arquivo
  fseek(inputFile, 0, SEEK_END);
  long long sizeFile = ftell(inputFile);
  printf("%lld\n", sizeFile);
  fseek(inputFile, 0, SEEK_SET);

  //]Aqui é feito um while para transferencia dos pacotes
  while (1)
  {
    //Se a transferência for completa, ele para
    if (transferCompleted)
      break;

    //Aqui é feito uma condição para pacotes de tamanho padrão
    if (amountOfBytesSent + SizeBuffer < sizeFile)
    {
      //Aqui é criado e enviado um pacote com dados,possuindo a flag de pacote final, o checksum e numero de sequencia
      fread(&sendBuffer, SizeBuffer, 1, inputFile);
      sendBuffer[SizeBuffer] = '0';
      sendBuffer[SizeBuffer + 1] = checksum(sendBuffer, SizeBuffer);
      sendBuffer[SizeBuffer + 2] = packageCounter;

      //Feito o envio
      bytesSend = sendto(socketServer, sendBuffer, SizeBuffer + 3, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));

      //Se os bytes enviados forem menor que zero é porque não foi bem sucedido
      if (bytesSend < 0)
      {
        printf("Ocorreu um erro, não foi possivel enviar os dados\n");
        //Como ocorreu um erro, foi finalizado o socket
        close(socketServer);
        exit(1);
      }

      //Aqui é feito o inicio do temporizador
      timer = 1;
      recivied = 0;
      limitTime = 0;
      pthread_create(&timerThread, NULL, timerFunction, NULL);
      pthread_create(&recognitionThread, NULL, recogntionFunction, NULL);

      while (timer)
      {
        //Aqui ele espera pelo estouro do temporizador ou pacote de reconhecimento
      }

      pthread_cancel(recognitionThread);
      pthread_cancel(timerThread);

      //Verifica o reconhecimento do pacote
      if (responseBuffer[0] == 1 && recivied == 1)
      {
        printf("Numero de sequencia do ACK: %d\n", responseBuffer[1]);
        packageCounter = packageCounter + 1;
        packageCounter %= 128;
      }
      else
      {
        if (limitTime)
        {
          printf("\nTempo de espera excedido\n");
        }
        else
        {
          printf("Numero de sequencia do NACK: %d\n", responseBuffer[1]);
        }
        bytesSend = sendto(socketServer, sendBuffer, SizeBuffer + 3, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));
      }

      amountOfPackageSent = amountOfBytesSent + 1;
      amountOfBytesSent += bytesSend;

      memset(sendBuffer, 0x0, SizeBuffer);
    }
    //Else para paotes com tamnho menor que o padrão
    else
    {

      bytesLeft = sizeFile - amountOfBytesSent;
      memset(sendBuffer, 0x0, SizeBuffer);

      //Cria e envia um pacote com dados, flag de pacote final, checksum e numero de sequencia
      fread(&sendBuffer, SizeBuffer, 1, inputFile);
      sendBuffer[bytesLeft] = '1';
      sendBuffer[bytesLeft + 1] = checksum(sendBuffer, bytesLeft);
      sendBuffer[bytesLeft + 2] = packageCounter;

      bytesSend = sendto(socketServer, sendBuffer, bytesLeft + 3, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));

      if (bytesSend < 0)
      {
        printf("ERROR: Não foi possivel enviar os dados\n");
        close(socketServer);
        exit(1);
      }

      //Inicio do temporizador
      timer = 1;
      recivied = 0;
      limitTime = 0;

      pthread_create(&timerThread, NULL, timerFunction, NULL);
      pthread_create(&recognitionThread, NULL, recogntionFunction, NULL);

      while (timer)
      {
        //Espera pelo estouro do temporizador ou pacote de reconhecimento
      }

      pthread_cancel(recognitionThread);
      pthread_cancel(timerThread);

      //Aqui ele verifica o reconhecimento do pacote
      if (responseBuffer[0] == 1 && recivied == 1)
      {
        printf("Numero de sequencia do ACK:: %d\n", responseBuffer[1]);
        packageCounter = packageCounter + 1;
        packageCounter %= 128;
      }
      else
      {
        if (limitTime)
        {
          printf("\n O Tempo de espera excedido\n");
        }
        else
        {
          printf("Numero de sequencia do NACK: %d\n", responseBuffer[1]);
        }
        bytesSend = sendto(socketServer, sendBuffer, bytesLeft + 3, 0, (struct sockaddr *)&addrClient, sizeof(addrClient));
      }

      amountOfPackageSent = amountOfPackageSent + 1;
      amountOfBytesSent += bytesSend;

      memset(sendBuffer, 0x0, SizeBuffer);
      transferCompleted = 1;
    }
  }

  //Finalização dos sockets
  close(sock);
  close(socketServer);
  return 0;
}




//Aqui é realizado a função principal
int main(int argc, char const *argv[])
{
  //Definição das variáveis do tipo thread de servidor e cliente
  pthread_t server, client;

  //Definição das portas para conexao
  nArg = argc;
  strcpy(serverIp, argv[1]);
  trackerServerPort = 3030;
  portServer = atoi(argv[2]);

  //Inicia as thread de servidor e cliente torrent
  pthread_create(&server, NULL, server_function, NULL);
  pthread_create(&client, NULL, clientFunction, NULL);

  pthread_join(client, NULL);
  pthread_join(server, NULL);

  return 0;
}
