/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:58:39 by florianmast       #+#    #+#             */
/*   Updated: 2022/04/30 16:04:08 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//source pertinentes : 
//https://www.linuxhowtos.org/C_C++/socket.htm
//http://sdz.tdct.org/sdz/les-sockets.html#Partie1L039applicationserveur
//https://www.youtube.com/watch?v=IydkqseK6oQ



#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define PORT 23

 #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
 
#include <stdio.h>
#include <stdlib.h>


typedef int SOCKET; // Pas bon
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
    SOCKET client; //int client c'est pareil 
    SOCKADDR_IN server;
    int bufsize = 1024;
    char buffer[1024] ;
    bool isExit = false;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                            SOCKET CREATION                                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
       /**            int connect(int socket, struct sockaddr* addr, socklen_t addrlen);        **\
     * 
	 * @brief La fonction permet d'etablir la connection
	 * 
     * @param  socket socket à utiliser  
	 * @param  addr   l'adresse de l'hote avec qui on va se mettre en relation  
	 * @param addrlen taille de l'adresse  
     * */   
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0)
    {
        std::cout << "Error socket creation" << std::endl;
        exit(EXIT_FAILURE);
    }
    

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                             CONFIGURATION                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                              CONNECT PART                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    if(connect(client, (SOCKADDR*)&server, sizeof(server)) != SOCKET_ERROR)
    {
        printf("Connexion à %s sur le port %d\n", inet_ntoa(server.sin_addr), htons(server.sin_port));
        while (1)
        {
          std::cout << "Client: ";
          while (1)
          {
              std::cin >> buffer;
              send(client, buffer, bufsize, 0);
              if (*buffer == '#') {
                  send(client, buffer, bufsize, 0);
                  *buffer = '*';
                  isExit = true;
              }
              if (*buffer == '*')
                break;
          } 
          if (isExit == true)
            break;
          std::cout << "Server: ";
          while (1)
          {
              recv(client, buffer, bufsize, 0);
              std::cout << buffer << " ";
              if (*buffer == '#') {
                  *buffer = '*';
                  isExit = true;
              }
              if (*buffer == '*')
                break;
          }
          std::cout << std::endl;
          if (isExit == true)
            break;
        }
    }
    else
    {
        printf("Impossible de se connecter\n");
        exit(1);
    }  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                               CLOSING PART                                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    close(client);
    return (0);
}