/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flmastor <flmastor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/28 18:05:43 by florianmast       #+#    #+#             */
/*   Updated: 2022/05/04 11:46:07 by flmastor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/server.hpp"
#include <unistd.h>
#include <string.h>
#include <cstdlib> 
#include <fcntl.h>
#include <stdlib.h>
#include <cerrno>
#include <iostream>
//TODO Tous les messages envoyés par un client dans un channel doivent être transmis
//à tous les clients ayant rejoint ce channel.

//◦ Vous devez avoir des operators et des utilisateurs basiques.

//Vous devez donc implémenter les commandes spécifiques aux operators.

//Vous devez pouvoir vous authentifier, définir un nickname, un username, rejoindre un channel, envoyer et recevoir des messages privés, avec votre client
//de référence.



//https://ocamil.com/index.php/c-c/c-c-les-sockets

#define SOCKET_ERROR -1
#define PORT 8888 //Attention tentative sur port 22 et c'est le ssh, le 23 permet une initiation de communication tcp (udp = 25)
#define MAX_CLIENT 3

int main(void)
{
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          SOCKET_PARAM                                                                                                    |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    //Parametre serveur
    int client;
    int i = 0;
    int nb_client = 0;
    int flags = 0;
    struct sockaddr_in sin;   // sin pour abreviation de socket_in, in est pour internet
    
    //Client management 
    int                     csock[MAX_CLIENT];
    struct sockaddr_in      csin[MAX_CLIENT];

    //Data management 
    int                     bufsize = 32;
    socklen_t               size = sizeof(sin);
    char                    buffer[bufsize];
 //   bool                    isExit = false;
    int                     clientCount = 1;
    

    
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
    //https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr
    int enable = 1;
    if (setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        std::cout << "setsockopt(SO_REUSEADDR) failed" << std::endl;
    }
    if (client == -1)
    {
        std::cout << "Error sock creation" << std::endl;
        exit(EXIT_FAILURE);
    }

     
    //Cf sujet, mode non bloquant pour la fonction accept
    flags=fcntl(client,F_GETFL);
    fcntl(client,F_SETFL,flags | O_NONBLOCK);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                             CONFIGURATION                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

    std::cout << "Socket: " << client << " is now opened in TCP/IP" << std::endl;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    //convert unsigned int pour des network byte ordonnés pour adresse IP
    sin.sin_family = AF_INET;                   // pour employer protocole TCP pour IP
    sin.sin_port = htons(PORT);                 // pour les unsigned short, pour des network byte ordonnés , listage du port
    
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          CONNECTION AVEC CLIENT                                                                                          |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    /**             int bind(int socket, const struct sockaddr* addr, socklen_t addrlen);            **\
     * 
	 * @brief comme dans ft_containers, bind permet de "joindre", et ainsi associer une adresse
     * spécifiée par addr à la socket, elle retourne SOCKET_ERROR en cas d'erreur et donc -1
	 * 
     * @param  socket  On a besoin de la socket pour join
	 * @param  addr    Passage par une structure spéciale pour cast le pointer et eviter les erreurs de compilatioh
	 * @param addrlen  size in btye de l'adresse de la strucutre pointée par addr
     * */
    int sock_err = bind(client, (struct sockaddr*)&sin, sizeof(sin));
    if (sock_err == SOCKET_ERROR)
    {
        if (errno == EACCES)
            std::cout << " EACCES" << std::endl;
        if (errno == EADDRINUSE)
            std::cout << " EADDRINUSE" << std::endl;
        if (errno == EBADF)
            std::cout << " EBADF" << std::endl;
        if (errno == EINVAL)
            std::cout << " EINVAL" << std::endl;
        if (errno == ENOTSOCK)
            std::cout << " ENOTSOCK" << std::endl;
        if (errno == EADDRNOTAVAIL)
            std::cout << " EADDRNOTAVAIL" << std::endl;
        if (errno == EFAULT)
            std::cout << " EFAULT" << std::endl;
        if (errno == EINVAL)
            std::cout << " EINVAL" << std::endl;
        if (errno == ELOOP)
            std::cout << " ELOOP" << std::endl;
        if (errno == ENAMETOOLONG)
            std::cout << " ENAMETOOLONG" << std::endl;
        if (errno == ENOENT)
            std::cout << " ENOENT" << std::endl;
        if (errno == ENOMEM)
            std::cout << " ENOMEM" << std::endl;
        if (errno == ENOTDIR)
            std::cout << " ENOTDIR" << std::endl;
        if (errno == EROFS)
            std::cout << " EROFS" << std::endl;
        std::cout << "Error bind" << std::endl;
        exit(EXIT_FAILURE);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          LISTEN AVEC CLIENT                                                                                              |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 /**             int listen(int socket, int backlog);            **\
  * 
	 * @brief mise en état d'écoute, en gros on établie la connexion
     *  elle retourne SOCKET_ERROR en cas d'erreur et donc -1
	 * 
     * @param  socket   On a besoin de la socket evidemment
	 * @param  backlog  le backlog représente le max de connexions pouvant être mise en attente  
     * */
    sock_err = listen(client, MAX_CLIENT);
    std::cout << "listage du port: " << PORT << std::endl;
    if (sock_err == SOCKET_ERROR)
    {
        std::cout << "Error listen" << std::endl;
        exit(EXIT_FAILURE);
    } 
    std::cout << "Please wait, the client is connecting to the port:" << PORT << std::endl;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                   SET TOUTES LES SOCKETS EN INVALIDES (-1)                                                                               |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 
     for(i=0;i<MAX_CLIENT;i++)
    {
        csock[i] = -1;
    }
    
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                                ACCEPT                                                                                                    |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
/**             int accept(int socket, struct sockaddr* addr, socklen_t* addrlen);            **\
     * 
	 * @brief La fonction va nous permettre de connecter client et serveur en acceptant l'appel de connexion
     * elle retourne INVALID_SOCKET en cas d'erreur et retourne le socket
	 * 
     * @param  socket  On a besoin du socket pour accepter la potentielle connection
	 * @param  addr    pointeur sur le contexte d'addressage du client
	 * @param addrlen  different de bind, ont attend une variable taille de type socklen_t (un entier) representnt la taille du contexte d'adressage du client
     * on passe l'addresse de cette variable à ce parametre
     * */
    //Ici donc on prend le relai et on accepte le "connect", de notre client 

    
   // int server = accept(client, (struct sockaddr*)&csin, &size);
   // std::cout << "Client is connecting with the socket " << server << " from:" << inet_ntoa(sin.sin_addr) << std::endl;
    bool start = true;
    int nb_slot = 0;
    //isExit = false;
   // while (isExit == false)
    while (1) 
    {
        if (start == true)
        {
            
            strcpy(buffer, "=> Server connected...\n");
            std::cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << std::endl;
            std::cout << "\n=> Enter # to end the connection\n" << std::endl;
             std::cout << "Client: ";
            start = false;
        }
        
    
         i=0;
    
        /*si on peut encore recevoir des clients*/
        if (nb_client < MAX_CLIENT)
        {
            /*gérer la connection de nouveaux clients*/
            struct sockaddr_in new_sin;
            int new_client = 0;
            int success =0;
    
            new_client = accept(client,(struct sockaddr*)&new_sin, &size);
            /*si le client est valide (il peut être invalide puisque sock est en mode non bloquante)*/
            if (new_client != -1)
            {
                std::cout << "Un nouveau client tente de se connecter : " << inet_ntoa(new_sin.sin_addr) << " Avec pour socket : " << new_client << std::endl;
                /*on trouve une place pour le client*/
                while (i < MAX_CLIENT)
                {
                    if(csock[i] == -1)
                    {
                        csock[i]= new_client;
                        csin[i]= new_sin;
                        success = 1;
                        break;
                    }
                    i++;
                }
                if(success)
                {
                    nb_slot++;
                    std::cout << "Accepted, we have " << nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
                }
                /*si il n'y a plus de place ( normalement il doit y en avoir puisqu'on à vérifier que nb_client < MAX_CLIENT )*/
                else
                    std::cout << "Refused" <<  nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
            }
        }
    
        i=0;
 
        /*on parcours le tableau de client*/
        while (i < MAX_CLIENT)
        {
            /*si client valide*/
            if (csock[i] != -1)
            {
                int s_recv = 0;
                if ( (s_recv = recv(csock[i], buffer, 32,MSG_DONTWAIT) ) == 32)
                {
                    /*tout c'est bien passer*/
                    std::cout << inet_ntoa(csin[i].sin_addr) << "csock: " << csock[i] << " send :" << buffer << std::endl;
                  //  printf("%s (csock : %d) : envoit : «%s»\n",inet_ntoa(csin[i].sin_addr),csock[i],buf);
                    /*on renvoit*/
                    send(csock[i],buffer,32,0);
                }
                else if (s_recv == 0)
                {
                    /*si la taille reçu égale à 0 : déconnection */
                    std::cout << inet_ntoa(csin[i].sin_addr) << "csock: " << csock[i] << "disconected " << std::endl;
                    
                   // printf("%s (csock : %d) : déconnection\n",inet_ntoa(csin[i].sin_addr),csock[i]);
                    /*on ferme la socket*/
                    close(csock[i]);
                    /*on libère une place de client*/
                    csock[i]=-1;
                    nb_client--;
                    nb_slot--;
                    std::cout << "We have " << nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
                }
                else if (s_recv == -1)
                    continue ;/* = pas de données reçu ( mode non bloquant de recv)*/
 
            }
            i++;
        }
            /*while (1)
            {
                recv(client, buffer, bufsize, 0);
                std::cout << buffer << " ";
                if (*buffer == '#') {
                    *buffer = '*';
                    isExit = true;
                }
                if (*buffer == '*')
                    break;
                else
                    continue;
            }
            if (isExit == true)
                break;
        }*/
        
        /*
        mettre gestion du temps avec usleep() si posix ou sleep() si windows
        */  
        
           /* while (1)
            {
                recv(server, buffer, bufsize, 0);
                std::cout << buffer << " ";
                if (*buffer == '#') {
                    *buffer = '*';
                    isExit = true;
                }
                if (*buffer == '*')
                    break;
            }
            if (isExit == true)
                break;
    
            while (!isExit)
            {
               std::cout << "\nServer: ";
                while (1)
                {
                    std::cin >> buffer;
                    send(server, buffer, bufsize, 0);
                    if (*buffer == '#') {
                        send(server, buffer, bufsize, 0);
                        *buffer = '*';
                        isExit = true;
                    }
                    if (*buffer == '*')
                        break;
                } 
                if (isExit == true)
                break;
                std::cout << "Client: ";
                while (1)
                {
                    recv(server, buffer, bufsize, 0);
                    std::cout << buffer << " ";
                    if (*buffer == '#')
                    {
                        *buffer = '*';
                        isExit = true;
                    }
                    if (*buffer == '*')
                        break;
                } 
                if (isExit == true)
                break;
            } */
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                         FERMETURE DE CONNEXION                                                                                           |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
/**             int closesocket(int sock);            **\
     * 
	 * @brief On close la procédure
	 * 
     * @param  socket  On a besoin du socket pour close la connection
     * */
//https://stackoverflow.com/questions/4160347/close-vs-shutdown-socket (tres interessant sur le choix de close ou shutdown, better close())
    std::cout << "Close the socket client" << std::endl;
    close(client);
    std::cout << "Server closing is done" << std::endl; 
    return EXIT_SUCCESS;
}