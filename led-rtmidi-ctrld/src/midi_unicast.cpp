// qmidiin.cpp
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <strings.h>

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "RtMidi.h"
bool done;
static void finish(int ignore){ done = true; }

#define EXAMPLE_PORT 10000
#define EXAMPLE_ADDR "192.168.2.5"
#define EXAMPLE_ADDR2 "192.168.2.7"

#define PACKET_TYPE_MIDI_IN  1
#define PACKET_TYPE_TIME_UP  2

int main()
{
  RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> message;
  int nBytes, i;
  double stamp;
  struct sockaddr_in addr;
  struct sockaddr_in addr2;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char packet[1024];
  unsigned long long mestamp_ns;
  struct timeb currentTS;

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(EXAMPLE_PORT);
  addrlen = sizeof(addr);

  bzero((char *)&addr2, sizeof(addr2));
  addr2.sin_family = AF_INET;
  addr2.sin_addr.s_addr = htonl(INADDR_ANY);
  addr2.sin_port = htons(EXAMPLE_PORT);

  /* set up socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  // Check available ports.
  unsigned int nPorts = midiin->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }
  midiin->openPort( 1 );
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );
  // Install an interrupt handler function.
  done = false;
  (void) signal(SIGINT, finish);
  // Periodically check input queue.
  std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
  while ( !done ) {
    stamp = midiin->getMessage( &message );

    nBytes = message.size();
    if ( nBytes > 0 )
    {
      std::cout << "stamp = " << stamp << std::endl;

      /* send */
      addr.sin_addr.s_addr = inet_addr(EXAMPLE_ADDR);
      addr2.sin_addr.s_addr = inet_addr(EXAMPLE_ADDR2);

      ftime( &currentTS );

      packet[0] = PACKET_TYPE_MIDI_IN;

      packet[1]  = ( currentTS.time >> 24 ) & 0xFF;
      packet[2]  = ( currentTS.time >> 16 ) & 0xFF;
      packet[3]  = ( currentTS.time >> 8 ) & 0xFF;
      packet[4]  = ( currentTS.time ) & 0xFF;

      packet[5]  = ( currentTS.millitm >> 8 ) & 0xFF;
      packet[6]  = ( currentTS.millitm ) & 0xFF;

      mestamp_ns = stamp * (1000*1000);
      packet[7]  = ( mestamp_ns >> 56 ) & 0xFF;
      packet[8]  = ( mestamp_ns >> 48 ) & 0xFF;
      packet[9]  = ( mestamp_ns >> 40 ) & 0xFF;
      packet[10]  = ( mestamp_ns >> 32 ) & 0xFF;
      packet[11]  = ( mestamp_ns >> 24 ) & 0xFF;
      packet[12]  = ( mestamp_ns >> 16 ) & 0xFF;
      packet[13]  = ( mestamp_ns >> 8 ) & 0xFF;
      packet[14]  = ( mestamp_ns ) & 0xFF;

      packet[15]  = nBytes & 0xFF;

      for( i=0; i<nBytes; i++ )
      {
          packet[16+i] = message[i];
      }

      cnt = sendto(sock, packet, (16+i), 0,
                        (struct sockaddr *) &addr, addrlen);
      if(cnt < 0)
      {
        perror("sendto");
        exit(1);
      }

      cnt = sendto(sock, packet, (16+i), 0,
                        (struct sockaddr *) &addr2, addrlen);
      if(cnt < 0)
      {
        perror("sendto");
        exit(1);
      }

    }
 
    // Sleep for 10 milliseconds ... platform-dependent.
    usleep( 10000 );
  }
  // Clean up
 cleanup:
  delete midiin;
  return 0;
}

