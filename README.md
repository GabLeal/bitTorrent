COM240.1 - REDES DE COMPUTADORES
TRABALHO 02

Grupo composto por:
CAMILA DAS GRACAS GRANDE DE FREITAS     - 2019005186
GABRIEL LEAL DOS SANTOS                 - 2019001418
GUSTAVO REZENDE DE ALMEIDA              - 2019005257
LAVYNIA DE OLIVEIRA                     - 2019014971


Para realização deste trabalho foram utilizados os códigos, PDFs, Aulas e Materiais restantes disponibilizados no SIGAA.

Como requisitado, foi implementado um modelo UDP que possui soma de verificação(checksum), número de sequência, temporizador e pacote de reconhecimento. Como adicional, também é possível inserir o nome do arquivo que se dejesa fazer a requisição no parâmetro de servidor rastreador.

Para se realizar o funcionamento da aplicação deve ser executado dois clientes torrent e um servidor rastreador, na qual para que qualquer cliente pudesse fazer uma requisição ou atendê-la, foi utilizado uma thread.


Para executar a aplicação:

Comandos para o servidor:
    Compilar: gcc server.c -o server -lpthread
    Executar: ./server 127.0.0.1 <nome_do_arquivo>

Comandos para o Cliente 1:
    Compilar: gcc torrent_client.c -o client -lpthread
    Executar: ./client 127.0.0.1 2020

Comandos para o Cliente 2:
    Compilar: gcc torrent_client.c -o client -lpthread
    Executar: ./client 127.0.0.1 2021
