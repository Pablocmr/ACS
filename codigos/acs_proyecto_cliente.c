/*
 **Codigo del Cliente
 **Autores: Martínez Ramírez Pablo César
 **         Zacatelco Zenteno Rodrigo Alberto
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
 **MYPORT: Puerto al que conectan los usuarios
 **LINE_MAX: 
 **MAXDATASIZE:
*/

#define PORT 3490
#define LINE_MAX 200
#define MAXDATASIZE 300

int main(int argc, char *argv[])
{
  
  /*
   **their_addr: Estructura socket de internet del servidor (informacion sobre su direccion)
   **sockfd: File descriptor para estructura socket servidor (escuchar sobre sock_fd)
   **numbytes: Numero real de bytes leidos
   **buf: BUffer
   **he: host entry
  */

  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct hostent *he;
  struct sockaddr_in their_addr;

  /*
   **Cliente ocupa 2 argumentos: nombre del ejecutable y la IP del servidor. 
   **Si no son 2 argumentos, se sale de la ejecución
  */

  if(argc != 2)
  {
    fprintf(stderr, "Client-Usage: %s hostname_del_servidor\n", argv[0]);
    exit(1);
  }

  /*
   **1. Llamada a funcion gethostbyname para obtener la informacion del host
   **2. Lo que devuelve la funcion gethostbyname se asigna a la variable he
   **3. Compara he contra nulo para validar que no haya errores
   **3.1 Si es igual, llama a funcion perror y se sale de la ejecucion
   **3.2 Si es diferente, la llamada a la funcion gethostbyname fue exitosa
  */

  if((he=gethostbyname(argv[1])) == NULL)
  {
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("Client-The remote host is: %s\n", argv[1]);

  /*
   **1. Llamada a funcion socket
   **2. Lo que devuelve la funcion socket se asigna a la variable sockfd
   **3. Compara sockfd contra -1 para validar que no haya errores
   **3.1 Si es igual, llama a funcion perror y se sale de la ejecucion
   **3.2 Si es diferente, la llamada a la funcion socket fue exitosa
  */

 /*
  Se crea un socket bidieccional, con modo de conexión de flujos de byte,
  puede proporcionar un mecanismo de transmisión para datos fuera de banda.
  */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket()");
    exit(1);
  }
  else
    printf("Client-The socket() sockfd is OK...\n");

  /*
   **Inicializar la estructura their_addr para pasarle la IP y el puerto
   **their_addr.sin_family: Socket TCP/IP (Ordenación de bytes de la máquina) 
   **their_addr.sin_port: Numero de Puerto (short, Ordenación de bytes de la red)
   **their_addr.sin_addr.s_addr: IP en decimal (se usa la primera direccion de la estructura he)
   **memset: (Poner a cero el resto de la estructura)
  */

  their_addr.sin_family = AF_INET;
  printf("Server-Using %s and port %d...\n", argv[1], PORT);
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(their_addr.sin_zero), '\0', 8);

  /*
   **1. Llamada a funcion connect
   **2. Compara lo que devuelve la funcion connect contra -1 para validar que no haya errores
   **2.1 Si es igual, llama a funcion perror y se sale de la ejecucion
   **2.2 Si es diferente, la llamada a la funcion connect fue exitosa
  */

  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("connect()");
    exit(1);
  }
  else
    printf("Client-The connect() is OK...\n");
  
  char bTermina[] = "terminar\n";
  while ( strcmp(bTermina, buf)!=0)
  {
    printf("Escribe un mensaje a enviar\n");
    char linea1[LINE_MAX]; // podemos usarlo por el fgets
    fgets(linea1,LINE_MAX,stdin);
    printf("El mensaje a enviar es: %s", linea1);
    // Envia el mensaje al servidor
    if (send(sockfd, linea1, strlen(linea1), 0) == -1)
      perror("Server-send() error lol!");

    // El cliente ya escribio, ahora va a leer la respuesta del servidor
    /*
    **1. Llamada a funcion recv para recibir mensaje del servidor
    **   La funcion devuelve el numero real de bytes leidos
    **2. Lo que devuelve la funcion accept se asigna a la variable new_fd
    **3. Compara sockfd contra -1 para validar que no haya errores
    **3.1 Si es igual, llama a funcion perror y se sale de la ejecucion
    **3.2 Si es diferente, la llamada a la funcion accept fue exitosa
    */

    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
      perror("recv()");
      exit(1);
    }
    else
      printf("Client-The recv() is OK...\n");

    //Se trunca el buffer al valor indicado por numbytes
    buf[numbytes] = '\0';
    printf("Client-Received: %s", buf);
  }
  //Cliente cierra su socket y regresa al prompt
  printf("Client-Closing sockfd (terminar)\n");
  close(sockfd);
  return 0;
}

