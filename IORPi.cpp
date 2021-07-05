/*
 *   Copyright (C) 2015,2016,2017,2018 by Jonathan Naylor G4KLX
 *   Copyright (C) 2015 by Jim Mclaughlin KI6ZUM
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Config.h"
#include "Globals.h"
#include "IO.h"
#include <pthread.h>
#include "audiointerface.h"
#include <vector>

#if defined(RPI)

#include "Log.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
int tcp_sock;
struct sockaddr_in serv_addr;

int tcp_connect()
{
    int portno, n;
    
    struct hostent *server;

    portno = 5990;
    tcp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (tcp_sock < 0) 
        perror("ERROR opening socket");
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    while (connect(tcp_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        perror("ERROR connecting");
        usleep(2000);
    }
    printf("Connected to localhost 5990");




    
    return 0;
}


const uint16_t DC_OFFSET = 2048U;
FILE* fptr;
AudioInterface *audio_interface;
std::vector<short> audio_buf;


unsigned char wavheader[] = {0x52,0x49,0x46,0x46,0xb8,0xc0,0x8f,0x00,0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0xc0,0x5d,0x00,0x00,0x80,0xbb,0x00,0x00,0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,0xff,0xff,0xff,0xff};

void CIO::initInt()
{

//	std::cout << "IO Init" << std::endl;
	DEBUG1("IO Init done! Thread Started!");

}

void CIO::startInt()
{
	DEBUG1("IO Int start()");
    //audio_interface = new AudioInterface(24000);
    tcp_connect();

        //fptr = fopen("RXSamples.wav", "wb");

	//::fwrite(wavheader, 44, 1, fptr); // pipe wave header for PiFmRDS

        ::pthread_create(&m_thread, NULL, helper, this);
}

void* CIO::helper(void* arg)
{
  CIO* p = (CIO*)arg;

  while (1)
  {
    usleep(2000);
    p->interrupt();
  }

  return NULL;
}


void CIO::interrupt()
{

    uint16_t sample = DC_OFFSET;
    uint8_t control = MARK_NONE;

   while(m_txBuffer.get(sample, control))
   {

     sample *= 5;		// amplify by 12dB	
     short signed_sample = (short)sample;

    //fwrite(&sample, sizeof(uint16_t), 1, fptr);
    //fflush(fptr);

        if(audio_buf.size() >= 320)
        {
            short *samples = new short[320];
            memcpy(samples, audio_buf.data(), 640);
            int n = ::sendto(tcp_sock, samples, 640, 0, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
            if (n < 0) 
            {
                perror("ERROR writing to socket");
            }
            audio_buf.clear();
            delete[] samples;
            audio_buf.push_back(signed_sample);
        }
        else
        {
            audio_buf.push_back(signed_sample);
        }
   }

    

	
    //::write(1, &sample, sizeof(uint16_t));  // pipe out samples to stdout

    sample = 2048U;
    //m_rxBuffer.put(sample, control);

#if defined(SEND_RSSI_DATA)
    m_rssiBuffer.put(ADC->ADC_CDR[RSSI_CDR_Chan]);
#else
    m_rssiBuffer.put(0U);
#endif

    m_watchdog++;
	
}

bool CIO::getCOSInt()
{
	return true;
}

void CIO::setLEDInt(bool on)
{
}

void CIO::setPTTInt(bool on)
{
	//handle enable clock gpio4

}

void CIO::setCOSInt(bool on)
{
}

void CIO::setDStarInt(bool on)
{
}

void CIO::setDMRInt(bool on)
{
}

void CIO::setYSFInt(bool on)
{
}

void CIO::setP25Int(bool on)
{
}

void CIO::setNXDNInt(bool on)
{
}

void CIO::delayInt(unsigned int dly)
{
  usleep(dly*1000);
}



#endif

