/*
* Example TCP/IP Multiple-Client Forking Server Example
* David Collien 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

// socket stuff
#include <sys/socket.h>
#include <sys/types.h>

// IP Address conversion
#include <arpa/inet.h>

// INET constants
#include <netinet/in.h>

// port number to use
#define PORT 1234

// size of the buffer for incoming data
#define BUF_SIZE 1024

// maximum size of the pending connections queue
#define QUEUE_SIZE 10

int main( int argc, char *argv[] ) {
   // descriptor for the socket we'll use as the server
   int serverSocket;

   // descriptor for an accepted connection to a client
   int clientSocket;

   // socket internet address data for the server
   struct sockaddr_in serverData;

   // socket internet address data for a client
   struct sockaddr_in clientData;
   int clientDataLength;

   // buffer for incoming/outgoing data
   char buffer[BUF_SIZE];

   bool isServing;
   bool isConnectedToClient;

   int bytesReceived;

   // process ID
   int pid;

   /*
    * Set up the server
    */

   bzero( &serverData, sizeof( serverData ) );
   // use the Internet Address Family (IPv4)
   serverData.sin_family = AF_INET;
   // accept connections from a client on any address
   serverData.sin_addr.s_addr = htonl( INADDR_ANY );
   // set the port for incoming packets
   serverData.sin_port = htons( PORT );

   /*
    * Open a TCP/IP (stream) socket
    * (and save the descriptor so we can refer to it in the future)
    */
   // we're using the internet protocol family and the TCP/IP protocol
   serverSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

   // bind the socket to the address
   int bindResult = bind( serverSocket, (struct sockaddr *)&serverData, sizeof( serverData ) );
   if ( bindResult < 0 ) {
      printf( "Error: Unable to bind socket" );
      close( serverSocket );
   } else if ( listen( serverSocket, QUEUE_SIZE ) < 0 ) {
      // listening for incoming connections failed
      printf( "Error: Unable to listen" );
      close( serverSocket );
   } else {

      // it's important to specify this size first
      clientDataLength = sizeof( clientData );

      isServing = true;
      while ( isServing ) {
         // accept an incoming connection request
         clientSocket = accept( serverSocket,
                                (struct sockaddr *)&clientData,
                                &clientDataLength );
         assert( clientSocket > 0 ); // ensure we have a valid socket

         char *clientAddress = inet_ntoa( clientData.sin_addr );
         int clientPort = ntohs( clientData.sin_port );

         printf( "Accepted Connection from: %s:%d\n", clientAddress, clientPort );

         // fork off into another process
         // this creates another process with the identical program state
         // except that pid == 0 in the child process
         pid = fork( );


         if ( pid == 0 ) {
            // we are the child process
            // close the connection on the listening socket
            // (it's being used by the parent process)
            close( serverSocket );

            isConnectedToClient = true;
            while ( isConnectedToClient ) {
               // receive any data from the client
               bytesReceived = recv( clientSocket, buffer, BUF_SIZE, 0 );

               // terminate the bytes as a string and print the result
               buffer[bytesReceived]= '\0';
               printf( "Received:\n%s\n", buffer );

               // reply to the client
               char replyText[] = "Packet received!";
               strncpy( buffer, replyText, strlen( replyText ) );
               printf( "Replying with: %s\n", buffer );

               send( clientSocket, buffer, strlen( replyText ), 0 );
            }
            // close the connection to the client before exit
            close( clientSocket );

            exit( EXIT_SUCCESS );
         } else {
            // we are in the parent process

            // close the connection to the client
            // (it's being used by the child process)
            close( clientSocket );
         }
      }

   }

   return 0;
}
