/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 11:33:22 by florianmast       #+#    #+#             */
/*   Updated: 2022/05/16 17:49:59 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/types.h>
#include <sys/socket.h> // Permet de d'utiliser la fonction socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

/*struct sockaddr_in
{
    short               sin_family;     // Toujours égale à AF_INET il identifi le format de data suivis
    unsigned short      sin_port;       // value de htons avec le port utilisé
    struct   in_addr    sin_addr;       // Adresse ip qui sera given au serveur pour le reveler = htonl
    char                sin_zero[8];    // not used 
};*/


#endif
