/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:58:39 by florianmast       #+#    #+#             */
/*   Updated: 2022/04/29 14:08:25 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define closesocket(param) close(param) // Pour plus de clareté au regard du code et fonction windows
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define PORT 23;

 #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
 
#include <stdio.h>
#include <stdlib.h>
#define PORT 23


typedef int SOCKET; // Pas bon
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
    SOCKET sock;
    SOCKADDR_IN sin;
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
    sock = socket(AF_INET, SOCK_STREAM, 0);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                             CONFIGURATION                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                              CONNECT PART                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
        printf("Connexion à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
    else
        printf("Impossible de se connecter\n");

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                               CLOSING PART                                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    closesocket(sock);
    return (0);
}