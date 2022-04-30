/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: florianmastorakis <florianmastorakis@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/28 18:05:43 by florianmast       #+#    #+#             */
/*   Updated: 2022/04/30 16:12:47 by florianmast      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/server.hpp"


#define SOCKET_ERROR -1
#define PORT 23




typedef int SOCKET; // Pas bon
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
    

int main(void)
{
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
//                                                                                          SOCKET_PARAM                                                                                                    |
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    int client;
    SOCKADDR_IN csin; // sin pour abreviation de socket_in, in est pour internet
    socklen_t crecsize = sizeof(csin);
     int bufsize = 1024;
    char buffer[bufsize];
    bool isExit = false;
    int clientCount = 1;
    

    
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
    if (client == -1)
    {
        std::cout << "Error sock creation" << std::endl;
        exit(EXIT_FAILURE);
    }
    //configuration 
    std::cout << "Socket: " << client << "is now opened" << std::endl;
    csin.sin_addr.s_addr = htonl(INADDR_ANY);    //convert unsigned int pour des network byte ordonnés pour adresse IP
    csin.sin_family = AF_INET;                   // pour employer protocole TCP pour IP
    csin.sin_port = htons(PORT);                 // pour les unsigned short, pour des network byte ordonnés , listage du port
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
    int sock_err = bind(client, (SOCKADDR*)&csin, sizeof(csin));
    if (sock_err == SOCKET_ERROR)
    {
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
    sock_err = listen(client, 5);
    std::cout << "listage du port: " << "23" << std::endl;
    if (sock_err == SOCKET_ERROR)
    {
        std::cout << "Error listen" << std::endl;
        exit(EXIT_FAILURE);
    } 
    std::cout << "Please wait, the client is connecting to the port 23" << std::endl;
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
   // SOCKADDR_IN csin;
    
    //socklen_t taille = sizeof(csin);
    int server = accept(client, (SOCKADDR*)&csin, &crecsize);
    std::cout << "Client is connecting with the socket " << server << " from (a travailler)" << std::endl;
    if (server > 0)
    {
        while (isExit == false) 
        {
            strcpy(buffer, "=> Server connected...\n");
            send(server, buffer, bufsize, 0);
            std::cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << std::endl;
            std::cout << "\n=> Enter # to end the connection\n" << std::endl;
    
            /* 
                Note that we would only get to this point after a 
                client has successfully connected to our server. 
                This reads from the socket. Note that the read() 
                will block until there is something for it to read 
                in the socket, i.e. after the client has executed a 
                the send().
                It will read either the total number of characters 
                in the socket or 1024
            */
       
            std::cout << "Client: ";
            while (1)
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
            } 
        }
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
   
    std::cout << "Close the socket server" << std::endl;
    closesocket(server);
    std::cout << "Close the socket client" << std::endl;
    close(client);
    std::cout << "Server closing is done" << std::endl; 
    return EXIT_SUCCESS;
}