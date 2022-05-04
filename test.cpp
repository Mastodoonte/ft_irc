#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <unistd.h>
#include <fcntl.h>
 
#include <sys/socket.h>
#include <arpa/inet.h>
 
#define MAX_CLIENT 3
 
int main(int argc,char** argv)
{
    /*gérer le serveur*/
    int client;
    int i=0;
    int nb_client=0;
    int port=1026;
    int flags=0;
    struct sockaddr_in sin;
 
    /*gérer les clients*/
    int csock[MAX_CLIENT];
    struct sockaddr_in csin[MAX_CLIENT];
 
    /*gérer les données*/
    char buf[32];
    socklen_t size=sizeof(struct sockaddr_in);
 
    if(argc > 1)
    {
        port=strtol(argv[1],NULL,10);
    }
 
    client =socket(AF_INET,SOCK_STREAM,0);
    if (client == -1)
    {
        perror("socket");
        exit(-1);
    }
 
    /*mode non bloquant ( pour accept() )*/
    flags=fcntl(client,F_GETFL);
    fcntl(client,F_SETFL,flags | O_NONBLOCK);
 
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    if (bind(client,(struct sockaddr*)&sin,sizeof sin) == -1)
    {
        perror("bind");
        exit(-1);
    }
    listen(client,MAX_CLIENT);
 
    /*on met toutes les sockets en invalides*/
    for(i=0;i<MAX_CLIENT;i++)
    {
        csock[i]=-1;
    }
 
    while (1)
    {
        i=0;
 
        /*si on peut encore recevoir des clients*/
        if (nb_client < MAX_CLIENT)
        {
            /*gérer la connection de nouveaux clients*/
            struct sockaddr_in new_sin;
            int new_client=0;
            int success=0;
 
            new_client = accept(client,(struct sockaddr*)&new_sin,&size);
            /*si le client est valide (il peut être invalide puisque sock est en mode non bloquante)*/
            if (new_client != -1)
            {
                printf("Un nouveau client tente de se connecter : «%s» .Socket : %d . ",inet_ntoa(new_sin.sin_addr),new_client);
 
                /*on trouve une place pour le client*/
                while(i < MAX_CLIENT)
                {
                    if(csock[i] == -1)
                    {
                        csock[i]=new_client;
                        csin[i]=new_sin;
                        success=1;
                        break;
                    }
                    i++;
                }
 
                if(success)
                {
                    puts("Accepter");
                    nb_client++;
                }
                /*si il n'y a plus de place ( normalement il doit y en avoir puisqu'on à vérifier que nb_client < MAX_CLIENT )*/
                else
                {
                    puts("Refuser");
                }
            }
        }
 
        i=0;
 
        /*on parcours le tableau de client*/
        while (i < MAX_CLIENT)
        {
            /*si client valide*/
            if (csock[i] != -1)
            {
                int s_recv=0;
                if ( (s_recv=recv(csock[i],buf,32,MSG_DONTWAIT) ) == 32)
                {
                    /*tout c'est bien passer*/
                    printf("%s (csock : %d) : envoit : «%s»\n",inet_ntoa(csin[i].sin_addr),csock[i],buf);
                    /*on renvoit*/
                    send(csock[i],buf,32,0);
                }
                else if (s_recv == 0)
                {
                    /*si la taille reçu égale à 0 : déconnection */
                    printf("%s (csock : %d) : déconnection\n",inet_ntoa(csin[i].sin_addr),csock[i]);
                    /*on ferme la socket*/
                    close(csock[i]);
                    /*on libère une place de client*/
                    csock[i]=-1;
                    nb_client--;
                }
                else if (s_recv == -1);/* = pas de données reçu ( mode non bloquant de recv)*/
 
            }
            i++;
        }
        /*
        mettre gestion du temps avec usleep() si posix ou sleep() si windows
        */
    }
 
    close(client);
 
    return 0;
}