/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:58:39 by florianmast       #+#    #+#             */
/*   Updated: 2022/04/30 17:46:08 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//source pertinentes : 
//https://www.linuxhowtos.org/C_C++/socket.htm
//http://sdz.tdct.org/sdz/les-sockets.html#Partie1L039applicationserveur
//https://www.youtube.com/watch?v=IydkqseK6oQ

#include <sys/types.h> 
#include <sys/socket.h> //Pour socket()
#include <netinet/in.h>
#include <arpa/inet.h> // Pour inet_addr() et inet_ntoa()
#include <iostream>
#include <unistd.h> // Pour close()

#define PORT 23

int main()
{
    int client; 
    struct sockaddr_in server;
    int bufsize = 1024;
    char buffer[1024] ;
    bool isExit = false;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                            SOCKET CREATION                                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    /**             int socket(int domain, int type, int protocol);             **\
     * 
	 * @brief La fonction permet de creer un socket et de specifier son style de communication
	 * 
     * @param  domain   représente la famille de protocole, par exemple AF_INET lorsqu'on veut employer un protocole TCP
	 * @param  type     spécifie le type de service SOCK_STREAM|SOCK_DGRAM, l'un est pour tcp l'autre udp:
     *  Stream est bidirectionnel | Datagram ou paquet, sont des sockets de connexion par paquet pas de connexion ouverte(donc moins gourmand)
	 * @param protocol  Dans le cas d'un tcp, le procotol c'est pas nécessaire, on le met à 0 
     * pour dire qu'on attend pas de protocol spécifique (par default)
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
    // On configure ici la structure prévue à cet effet build by sockaddr_in 
    // on va mettre l'adresse du local host, (le serveur est sur notre machine et 127.0.0.1  est ip standard du local host)
    // le trosieme c'est juste l'indication du port pour que notre serveur puisse etre ouvert au port en question 
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                              CONNECT PART                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    //Ici on se connecte en envoyant, le socket FD, l'adresse de l'host (incluant le port number = IP+PORT) et la taille de l'adresse
    
    /**            int connect(int socket, struct sockaddr* addr, socklen_t addrlen);        **\
     * 
	 * @brief La fonction permet d'etablir la connection
	 * 
     * @param  socket socket à utiliser  
	 * @param  addr   l'adresse de l'hote avec qui on va se mettre en relation  
	 * @param addrlen taille de l'adresse  
     * */   
    
    if(connect(client, (struct sockaddr*)&server, sizeof(server)) != -1)
    {
        // inet_ntoa () convertit l'adresse Internet de l'hôte in donne dans l'ordre des octets du réseau en une 
        //chaîne de caractères dans la notation numérique pointée. La chaîne est renvoyée dans un tampon alloué
        // statiquement, qui est donc écrasé à chaque appel.
        //htons() converti unsigned short du reseau byte order au client byte order 
        
        std::cout << "Connexion on " << inet_ntoa(server.sin_addr) << " on port " << htons(server.sin_port) << std::endl;
        while (1)
        {
          std::cout << "Client: ";
          while (1)
          {
              std::cin >> buffer;
              send(client, buffer, bufsize, 0);
              if (*buffer == '#')
              {
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
              if (*buffer == '#')
              {
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
       std::cout << "Connexion impossible" << std::endl;
        exit(EXIT_FAILURE);
    }  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                               CLOSING PART                                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    close(client);
    return (0);
}