/*
 **Codigo del Servidor 
 **Autores: Martínez Ramírez Pablo César
 **         Zacatelco Zenteno Rodrigo Alberto
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

/*
 **MYPORT: Puerto al que conectan los usuarios
 **BACKLOG: Numero de conexiones pendientes se mantienen en cola
 **LINE_MAX: 
 **MAXDATASIZE: Numero maximo de bytes que se pueden obtener de respuesta
*/

#define MYPORT 3490
#define BACKLOG 100     
#define LINE_MAX 200
#define MAXDATASIZE 300

/*
 **sigchld_handler
*/

void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}

int main(int argc, char *argv[])
{

  /*
   **my_addr: Estructura socket de internet del servidor (informacion sobre mi direccion)
   **their_addr: Estructura socket de internet del cliente (informacion sobre su direccion)
   **sockfd: File descriptor para estructura socket servidor (escuchar sobre sock_fd)
   **new_fd: File descriptor para estructura socket cliente (nuevas conexiones sobre new_fd)
   **numbytes: Numero real de bytes leidos
   **buf: Buffer
   **sin_size: Tamanio de la estructura sockaddr_in
   **sa:
   **yes: 
  */

  int sockfd, new_fd, numbytes;  
  char buf[MAXDATASIZE];
  struct sockaddr_in my_addr; 
  struct sockaddr_in their_addr; 
  int sin_size;
  struct sigaction sa;
  int yes=1;

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
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Server-socket() error lol!");
    exit(1);
  }
  else
    printf("Server-socket() sockfd is OK...\n");
  /*
  Se especifican las opciones a nivel SOCKET (SOL_SOCKET). 
  SO_REUSEADDR:Usando bind() se puede reusar la dirección después. 
  */
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("Server-setsockopt() error lol!");
    exit(1);
  }
  else
    printf("Server-setsockopt is OK...\n");
  
  /*
   **Inicializar la estructura my_addr para pasarle la IP y el puerto
   **my_addr.sin_family: Socket TCP/IP (Ordenación de bytes de la máquina) 
   **my_addr.sin_port: Numero de Puerto (short, Ordenación de bytes de la red)
   **my_addr.sin_addr.s_addr: Direccion IP (Rellenar con mi dirección IP)
   **INADDR_ANY: indica que puede utilizar cualquiera de las interfaces de red del servidor
   **memset: (Poner a cero el resto de la estructura)
  */

  my_addr.sin_family = AF_INET;          
  my_addr.sin_port = htons(MYPORT);      
  my_addr.sin_addr.s_addr = INADDR_ANY; 
  printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);
  memset(&(my_addr.sin_zero), '\0', 8);

  /*
   **1. Llamada a funcion bind para ligar el socket a una IP y un puerto
   **2. Compara lo que devuelve bind contra -1 para validar que no haya errores
   **2.1 Si es igual, llama a funcion perror y se sale de la ejecucion
   **2.2 Si es diferente, la llamada a la funcion bind fue exitosa
  */

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("Server-bind() error");
    exit(1);
  }
  else
    printf("Server-bind() is OK...\n");

  /*
   **1. Llamada a funcion listen para definir la cola de conexiones
   **2. Compara lo que devuelve listen contra -1 para validar que no haya errores
   **2.1 Si es igual, llama a funcion perror y se sale de la ejecucion
   **2.2 Si es diferente, la llamada a la funcion listen fue exitosa
  */

  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("Server-listen() error");
    exit(1);
  }
  printf("Server-listen() is OK...Listening...\n");

  /*
   **sa.sa_handler: Elimina procesos muertos
   **
   **
  */

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    perror("Server-sigaction() error");
    exit(1);
  }
  else
    printf("Server-sigaction() is OK...\n");

  /*
   **Loop principal de accept()
   **
   **
  */

  while(1)
  { 
    //Se inicializa el tama�o de la estructura sockaddr_in
    sin_size = sizeof(struct sockaddr_in);

    /*
     **1. Llamada a funcion accept para establecer la comunicacion con el cliente
     **2. Lo que devuelve la funcion accept se asigna a la variable new_fd
     **3. Compara sockfd contra -1 para validar que no haya errores
     **3.1 Si es igual, llama a funcion perror y se sale de la ejecucion
     **3.2 Si es diferente, la llamada a la funcion accept fue exitosa
    */

    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
    {
      perror("Server-accept() error");
      continue;
    }
    else
      printf("Server-accept() is OK...\n");
    
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

    if (!fork())
    { // Este es el proceso hijo
      close(sockfd); // El hijo no necesita este descriptor

      char bTermina[] = "terminar\n";
      int banderaTermina;
      do 
      {
        // Como el cliente escribe, yo leo
        if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1)
        {
          perror("recv()");
          exit(1);
        }
        else
          printf("Servidor-The recv() is OK...\n");
        buf[numbytes] = '\0';
        printf("Servidor-Comando-Recibido: %s", buf);

        //Si recibe "termina", envia "termina" para terminar
        banderaTermina=strcmp(bTermina, buf);
        if (banderaTermina==0)
        {
          if (send(new_fd, bTermina, strlen(bTermina), 0) == -1)
            perror("Server-send() error lol!");
          printf("Server-Envia: \"terminar\"\n");
        }
        else
        {
          // Ahora yo capturo del teclado para responder al cliente
          printf("Escribe un mensaje a enviar\n");
          char linea1[LINE_MAX]; // podemos usarlo por el fgets
          fgets(linea1,LINE_MAX,stdin);
          printf("El mensaje a enviar es: %s", linea1);

          /*
          **1.  Llamada a funcion send para escribirle al cliente
          **    Utiliza el fd del cliente, la cadena almacenada y el tamanio de la cadena
          **2   Compara lo que devuelve la funcion send contra -1 para validar que no haya errores
          **2.1 Si es igual, llama a funcion perror
          */

          if (send(new_fd, linea1, strlen(linea1), 0) == -1)
            perror("Server-send() error lol!");
        }
      }while(banderaTermina!=0);
            
      //Servidor hijo cierra socket cliente y termina su ejecucion
      printf("Se recibió el comando de terminar\n");
      close(new_fd);
      exit(0);
    }

    //Servidor padre cierra el descriptor del cliente porque no lo necesita
    printf("Este es el proceso padre, cierra el descriptor del socket cliente y se regresa a esperar otro cliente\n");
    close(new_fd);
    printf("Server-new socket, new_fd closed successfully...\n");
  } 

  return 0;
}
