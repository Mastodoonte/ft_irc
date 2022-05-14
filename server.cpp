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
#include "include/user.hpp"
#include <unistd.h>
#include <string.h>
#include <cstdlib> 
#include <fcntl.h>
#include <stdlib.h>
#include <cerrno>
#include <iostream>
#include <ctime>
#include <map>

//TODO Tous les messages envoyés par un client dans un channel doivent être transmis
//à tous les clients ayant rejoint ce channel.

//◦ Vous devez avoir des operators et des utilisateurs basiques.

//Vous devez donc implémenter les commandes spécifiques aux operators.

//Vous devez pouvoir vous authentifier, définir un nickname, un username, rejoindre un channel, envoyer et recevoir des messages privés, avec votre client
//de référence.



//https://ocamil.com/index.php/c-c/c-c-les-sockets

#define SOCKET_ERROR -1
#define PORT 6667 //Attention tentative sur port 22 et c'est le ssh, le 23 permet une initiation de communication tcp (udp = 25)
#define MAX_CLIENT 3

int main(void)
{
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          SOCKET_PARAM                                                                                                    |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    //Parametre serveur
    int server_socket;
    int nb_client = 0;
    int flags = 0;
    struct sockaddr_in sin;   // sin pour abreviation de socket_in, in est pour internet
    
    //Client management 
    std::map<int, User>  user_tab;
    //Data management 
    int                     bufsize = 100;
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
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    //https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr
    int enable = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        std::cout << "setsockopt(SO_REUSEADDR) failed" << std::endl;
    }
    if (server_socket == -1)
    {
        std::cout << "Error sock creation" << std::endl;
        exit(EXIT_FAILURE);
    }

     
    //Cf sujet, mode non bloquant pour la fonction accept
    flags=fcntl(server_socket,F_GETFL);
    fcntl(server_socket,F_SETFL,flags | O_NONBLOCK);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                             CONFIGURATION                                                                                                |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

    std::cout << "Socket: " << server_socket << " is now opened in TCP/IP" << std::endl;
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
    int sock_err = bind(server_socket, (struct sockaddr*)&sin, sizeof(sin));
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
    sock_err = listen(server_socket, MAX_CLIENT);
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
 
    /* for(i=0;i<MAX_CLIENT;i++)
    {
        csock[i] = -1;
    }*/
    
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
    time_t  now;
    //isExit = false;
   // while (isExit == false)

    now = std::time(0);
    fd_set current_socket, ready_socket;
    FD_ZERO(&current_socket);
    FD_SET(server_socket, &current_socket);
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
	
	//Tout les X temps je ping tout les utilisateurs
	if (std::time(0) - now >= 100)
	{
	    std::map<int, User>::iterator begin = user_tab.begin();

	    while (begin != user_tab.end())
	    {
		now = begin->second.ping();	
		begin++;
	    }
	}

	ready_socket = current_socket;
	errno = 0;
	if (select(1024, &ready_socket, NULL, NULL, NULL) < 0)
	{
	    std::cout << "Error, select failed " << strerror(errno) << std::endl;
	    exit(EXIT_FAILURE);
	}
	for (int j = 0; j < 1024; j++)
	{
	    if (FD_ISSET(j, &ready_socket))
	    {
		if (j == server_socket)
		{
		    //Add a new client
		    if (nb_client < MAX_CLIENT)
		    {
			/*gérer la connection de nouveaux clients*/
			User new_user;
    
			/*si le client est valide (il peut être invalide puisque sock est en mode non bloquante)*/
			if (new_user.acceptUsr(server_socket) != -1)
			{
			    /*on trouve une place pour le client*/
			    user_tab.insert(std::pair<int, User>(new_user.getSocket(), new_user));
			    nb_slot++;
			    FD_SET(new_user.getSocket(), &current_socket);
			    std::cout << "Accepted, we have " << nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
			}
			/*si il n'y a plus de place ( normalement il doit y en avoir puisqu'on à vérifier que nb_client < MAX_CLIENT )*/
			else
			    std::cout << "Refused" <<  nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
		    }
		}
		else
		{
		    /*si client valide*/
		    int s_recv = 0;
		    bzero(buffer, 100);
		    s_recv = recv(j, buffer, 100, MSG_DONTWAIT);
		    if (s_recv != -1 && s_recv)
		    {
		        /*tout c'est bien passer*/
		        std::cout << inet_ntoa(user_tab[j].getAddr().sin_addr) << " csock: " << j << " send :" << buffer << std::endl;
		       //  printf("%s (csock : %d) : envoit : «%s»\n",inet_ntoa(csin[i].sin_addr),csock[i],buf);
			 /*Parsing de la commande*/
			user_tab[j].chooseCMD(buffer);
		    }
		    else if (s_recv == 0)
		    {
			/*si la taille reçu égale à 0 : déconnection */
			std::cout << inet_ntoa(user_tab[j].getAddr().sin_addr) << " csock: " << j << "disconected " << std::endl;
                    
			// printf("%s (csock : %d) : déconnection\n",inet_ntoa(csin[i].sin_addr),csock[i]);
			/*on ferme la socket*/
			user_tab[j].clear();
			user_tab.erase(j);
			/*on libère une place de client*/
			nb_client--;
			nb_slot--;
			std::cout << "We have " << nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
		    }
		    else if (s_recv == -1)
			continue ;/* = pas de données reçu ( mode non bloquant de recv)*/
		    FD_CLR(j, &current_socket);
		}
	    }
	}
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
    close(server_socket);
    std::cout << "Server closing is done" << std::endl; 
    return EXIT_SUCCESS;
}
