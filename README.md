<div align="center">
  <img src="src/school_42_logo.jpeg" height="128px" alt="badge de 42" >
  <h1>Ft_irc</h1>
  <p>🚀 Internet Relay Chat</p>
  <img src="src/grade.png" alt="grade of the irc project">
<pr> 
</div>

## 👀 Preview
The objective of this project is to reproduce the functioning of an IRC server.
You will use a real IRC client to connect to your server and test it.
The Internet works with many standards and protocols to allow interoperability between connected machines. It is always interesting to know this kind of thing.
Internet Relay Chat is a text-based communication protocol on the Internet. It is used for instant communication primarily in the form of group discussions through chat channels, but can also be used for direct communication between two people.
IRC clients connect to IRC servers to access channels. IRC servers are connected to each other to create networks.

![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
![forthebadge](https://forthebadge.com/images/badges/check-it-out.svg)



## 👀 Objectives
You have to develop an IRC server in C++ 98. You do not have to develop a client.
You do not have to handle server-to-server communication. Your binary should be called like this:

```./ircserv <port> <password>```

- port: The port number on which your server will accept incoming connections.
connections.
- password: The password to identify to your IRC server, which should be provided by any IRC client wishing to connect to it.
- Your program must never crash (even if you run out of memory) or stop unexpectedly except in the case of undefined behavior.
If this happens, your project will be considered non-functional and you will get 0.
- You must render a Makefile that will compile your source files. It must not relink.
- Your Makefile must contain at least the following rules: $(NAME), all, clean, fclean and re.
- Compile your code with c++ and the flags -Wall -Wextra -Werror
- You must comply with the C++ 98 standard. Therefore, your code must
compile if you add the flag -std=c++98
- In your work, try to use mostly C++ functions (for example, prefer <cstring> to <string.h>). You can use C functions, but do your best to choose the C++ version when you can.
- Any use of external libraries or the Boost package is prohibited. 
- There are several IRC clients. You must choose one of them as a reference. Your reference client will be used for evaluation.
- Your reference client must be able to connect to your server without encountering any errors.
- The communication between the client and the server will be done over TCP/IP (v4 or v6).

Using your reference client with your server should be similar to using it with an official IRC server. However, only the following features are mandatory:

◦ You must be able to authenticate, set a nickname, username, re-join a channel, send and receive private messages, with your reference client.

◦ All messages sent by a client in a channel must be forwarded to all clients who have joined that channel.

◦ You must have operators and basic users.

◦ So you need to implement operator-specific commands.

Of course, you are expected to have clean code.

<!--badges : https://badge42.vercel.app-->
