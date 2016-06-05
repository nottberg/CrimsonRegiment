#include <string.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <iostream>

int
main( int argc, char **argv )
{
    struct sockaddr_in s;
    int bcast_sock;
    int broadcastEnable=1;
    int result;
    uint8_t msgBuf[1024];

    bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    
//    result = setsockopt( bcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable) );

    memset( &s, '\0', sizeof( struct sockaddr_in ) );
    s.sin_family      = AF_INET;
    s.sin_port        = (in_port_t)htons( 10260 );
    inet_aton( "192.168.2.7", &s.sin_addr );

    std::cout << "Send" << std::endl;

    if( sendto( bcast_sock, msgBuf, 60, 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in) ) < 0 )
    {
        perror("sendto");
        return 1;
    }

    return 0;
}

