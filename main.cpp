#include "include/server.hpp"
#include "include/user.hpp"
#define MAX_CLIENT 10

bool must_exit = false;

bool isNumber(const std::string& str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void     parsing(int ac, char **av,  global *global)
{
    if (ac != 3)
        throw errorReturn("Usage: ./ircserv <port> <password>");
    std::string port = av[1];
    std::string password = av[2];

    if (atoi(port.c_str()) < 1023)
        throw errorReturn("Usage: ports from 0 to 1023 are reserved for server access and therefore unusable ");
    if (atoi(port.c_str()) > 65535)
        throw errorReturn("Usage: ports are unusable ");
    if ((!isNumber(password)) || (password.size() < 4))
        throw errorReturn("Usage: password needs to be at least 4 digits ");
    global->port = atoi(port.c_str());
    global->password = atoi(password.c_str());
}

void    ft_exit_c(int i)
{
    (void)i;
    must_exit = 1;
}

int main(int ac, char **av)
{
    int                     server_socket;
    int                     sock_err = 0;
    struct sockaddr_in      sin;   // sin pour abreviation de socket_in, in est pour internet
    global  global;

    try
    {
        parsing(ac, av, &global);
        socketInitialisation(&server_socket);
        sock_err = settingsAndConnection(&server_socket, sock_err, &sin, &global);
        listenUsers(&sock_err, &server_socket, MAX_CLIENT, &global);
        signal(SIGINT, ft_exit_c);
        loopServer(server_socket, global);
        close(server_socket);
        return (EXIT_SUCCESS);
    }
    catch(const std::exception& e)
    {
        std::cerr << RED << e.what() << NORMAL << '\n';
    }
}
