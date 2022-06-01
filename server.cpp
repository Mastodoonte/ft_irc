/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fayel-ha <fayel-ha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/28 18:05:43 by florianmast       #+#    #+#             */
/*   Updated: 2022/05/31 15:42:43 by fayel-ha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/server.hpp"
#include "include/user.hpp"
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <stdlib.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <ctime>
#include <map>


#define PORT 5666 //Attention tentative sur port 22 et c'est le ssh, le 23 permet une initiation de communication tcp (udp = 25)

extern bool must_exit;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          SOCKET_PARAM                                                                                                    |
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
void    socketInitialisation(int *server_socket)
{
	*server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	int enable = 1;
	if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		throw errorReturn("Error: setsockopt(SO_REUSEADDR) failed");
	}
	if (*server_socket == -1)
	{
		throw errorReturn("Error: sock creation");
	}
	 //Cf sujet, mode non bloquant pour la fonction accept
	fcntl(*server_socket,F_SETFL,O_NONBLOCK);

}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                             CONFIGURATION AND CONNECTION                                                                                               |
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
int    settingsAndConnection(int *server_socket, int sock_err, struct sockaddr_in *sin, global *global)
{

	std::cout << "Socket: " << server_socket << " is now opened in TCP/IP" << std::endl;
	(void)sin;
	*(&sin->sin_addr.s_addr) = htonl(INADDR_ANY);    //convert unsigned int pour des network byte ordonnés pour adresse IP
	*(&sin->sin_family) = AF_INET;                   // pour employer protocole TCP pour IP
	*(&sin->sin_port) = htons(global->port);                 // pour les unsigned short, pour des network byte ordonnés , listage du port
	sock_err = bind(*server_socket, (struct sockaddr*) (*(&sin)), sizeof(*sin));
	if (sock_err == -1)
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
		throw errorReturn(" Error: Error bin");
	} 
	return (sock_err);
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
void    listenUsers(int *sock_err, int *server_socket, int nb_client, global *global)
{
	*sock_err = listen(*server_socket, nb_client);
	std::cout << "listage du port: " << global->port << std::endl;
	if (*sock_err == -1)
	{
		throw errorReturn("Error: listen");
	} 
	std::cout << "Listen function has operated well on port :" << global->port << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          LOOP FUNCTION                                                                                                   |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

void          loopServer(int server_socket, global global)
{
	bool start =            true;
	int                     nb_slot = 0;
	fd_set                  current_socket, ready_socket;
	int                     nb_client = 0;
	char                    buffer[1024];
	std::map<int, User>     user_tab;
	
	FD_ZERO(&current_socket);
	FD_SET(server_socket, &current_socket);
	while (1) 
	{
		if (must_exit == true)
			return ;
		if (start == true)
		{            
			strcpy(buffer, "=> Server connected...\n");
			std::cout << "\n=> Please enter : CTRL + C to switch off the server\n" << std::endl;
			start = false;
		}
		ready_socket = current_socket;
		errno = 0;
		if (select(1024, &ready_socket, NULL, NULL, NULL) < 0)
		{
			throw errorReturn("Error: select has failed");
		}
		for (int j = 0; j < 1024; j++)
		{
			try
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
								std::cout << GREEN << "#   Serveur info: " << "Accepted, we have " << nb_slot << "/" << MAX_CLIENT << "slot " << NORMAL << std::endl;
							}
							/*si il n'y a plus de place ( normalement il doit y en avoir puisqu'on à vérifier que nb_client < MAX_CLIENT )*/
							else
								std::cout << "#   Refused" <<  nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
						}
					}
					else
					{
						/*si client valide*/
						int s_recv = 0;
						bzero(buffer, 100);
						s_recv = recv(j, buffer, 100, MSG_DONTWAIT);
						if (s_recv != -1 && s_recv && user_tab[j].isBufferReady(buffer))
						{
						 /*Parsing de la commande*/
							user_tab[j].chooseCMD(user_tab, j, global);
					user_tab[j].clearBuffer();
							for (std::map<std::string, Channel>::iterator it = user_tab[j].channels.begin(); it != user_tab[j].channels.end(); it++)
							{
								std::cout << YELLOW << "Channel: " << it->second._name << NORMAL << std::endl;
								for (std::vector<t_client>::iterator it1 = it->second._chan_clients.begin(); it1 != it->second._chan_clients.end(); it1++)
									std::cout << BLUE << "Il y a " << it1->nickname << " qui est present avec la socket : " << RED << it1->socket << NORMAL << std::endl;
							}
						}
						else if (s_recv == 0)
						{
							/*si la taille reçu égale à 0 : déconnection */
							std::cout << GREEN << "#   " <<inet_ntoa(user_tab[j].getAddr().sin_addr) << " csock: " 
								<< j << "disconected " << NORMAL << std::endl;
							/*on ferme la socket*/
							user_tab[j].clear();
							user_tab.erase(j);
							/*on libère une place de client*/
							nb_client--;
							nb_slot--;
							std::cout << GREEN << "# " << "We have " << nb_slot << "/" << MAX_CLIENT << "slot "<< std::endl;
							FD_CLR(j, &current_socket);
						}
						else if (s_recv == -1)
						continue ;/* = pas de données reçu ( mode non bloquant de recv)*/
					}
				}
			}
			catch(const std::exception& e)
			{
			   std::cerr << GREEN << "#   " << e.what() << '\n';
			}
		}
	}
}
