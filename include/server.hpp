/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 11:33:22 by florianmast       #+#    #+#             */
/*   Updated: 2022/05/30 17:52:05 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h> // Permet de d'utiliser la fonction socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#define MAX_CLIENT 10

struct global
{
    int port;
    int password;
};

void    socketInitialisation(int *server_socket);
int     settingsAndConnection(int *server_socket, int sock_err, struct sockaddr_in *sin, global *global);
void    listenUsers(int *sock_err, int *server_socket, int nb_client, global *global);
void    loopServer(int server_socket, global global);

#endif
