#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

// port number to use
#define PORT 1234

// size of the buffer for incoming data
#define BUF_SIZE 1024

int main( int argc, char *argv[] ) {
   if ( argc != 2 ) {
      printf( "Usage: %s <IP Address>\n", argv[0] );
      exit( 0 );
   }

   // get the host address from the first commandline argument
   char *address = argv[1];

   // descriptor number for the socket we'll use
   int serverSocket;

   // socket internet address data for the connection to the server
   struct sockaddr_in serverData;

   // host data
   struct hostent *host;

   // buffer for sending and receiving data
   char buffer[BUF_SIZE];

   bool isConnected;

   int bytesReceived;

   /*
    * Set up the connection to a server
    */

   bzero( &serverData, sizeof( serverData ) );
   // use the Internet Address Family (IPv4)
   serverData.sin_family = AF_INET;
   // get host data from the host address
   host = gethostbyname( address );
   // copy the address data from the host struct over to the server address struct
   bcopy( host->h_addr, &(serverData.sin_addr.s_addr), host->h_length);
   // set the port to connect to
   serverData.sin_port = htons( PORT );

   /*
    * Open a TCP/IP socket to the server
    * (and save the descriptor so we can refer to it in the future)
    */
   serverSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

   // connect to the server
   if ( connect( serverSocket,
                 (struct sockaddr *)&serverData,
                 sizeof( serverData ) ) < 0 ) {
      printf( "Error connecting\n" );
      close( serverSocket );
   } else {

      isConnected = true;
      while ( isConnected ) {
         // send some data
         printf( "Send> " );
         //strncpy( buffer, "Hi", 3 );
         fgets( buffer, BUF_SIZE, stdin );

         send( serverSocket, buffer, strlen( buffer ), 0 );

         // wait to receive data from the server
         bytesReceived = recv( serverSocket, buffer, BUF_SIZE, 0 );

         // terminate the bytes as a string and print the result
         buffer[bytesReceived]= '\0';
         printf( "Received: %s\n", buffer );
      }

      close( serverSocket );
   }

   return 0;
}
