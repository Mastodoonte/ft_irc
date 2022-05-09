#include "include/user.hpp"

User::User(void) {}

User::~User(void) {
    close(socket);
}

int	User::acceptUsr(int server_socket) {
    socket_len = sizeof(addr);
    socket = accept(server_socket, (struct sockaddr *)&addr, &socket_len);
    if (socket == -1)
    {
	std::cout << "Error to accept client" << std::endl;
	return (-1);
    }
    std::cout << "Un nouveau client tente de se connecter : " << inet_ntoa(addr.sin_addr) << " Avec pour socket : " << socket << std::endl;
    return (0);
}

int	User::getSocket(void) {
 return (socket);
}

sockaddr_in	&User::getAddr(void) {
    return (addr);
}
