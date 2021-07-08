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
#include <vector>

#if defined(RPI)

#include "Log.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


const uint16_t DC_OFFSET = 2048U;

unsigned char wavheader[] = {0x52,0x49,0x46,0x46,0xb8,0xc0,0x8f,0x00,0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0xc0,0x5d,0x00,0x00,0x80,0xbb,0x00,0x00,0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,0xff,0xff,0xff,0xff};

void CIO::initInt()
{

//	std::cout << "IO Init" << std::endl;
	DEBUG1("IO Init done! Thread Started!");

}

void CIO::startInt()
{
    
	DEBUG1("IO Int start()");
    if (::pthread_mutex_init(&m_TXlock, NULL) != 0)
    {
        printf("\n Tx mutex init failed\n");
        exit(1);;
    }
    if (::pthread_mutex_init(&m_RXlock, NULL) != 0)
    {
        printf("\n RX mutex init failed\n");
        exit(1);;
    }

    ::pthread_create(&m_thread, NULL, helper, this);
    ::pthread_create(&m_threadRX, NULL, helperRX, this);
}

void* CIO::helper(void* arg)
{
  CIO* p = (CIO*)arg;

  while (1)
  {
      if(p->m_txBuffer.getData() < 1)
        usleep(20);
    p->interrupt();
  }

  return NULL;
}

void* CIO::helperRX(void* arg)
{
  CIO* p = (CIO*)arg;

  while (1)
  {

    usleep(20);
    p->interruptRX();
  }

  return NULL;
}


void CIO::interrupt()
{

    uint16_t sample = DC_OFFSET;
    uint8_t control = MARK_NONE;
    ::pthread_mutex_lock(&m_TXlock);
   while(m_txBuffer.get(sample, control))
   {

        sample *= 5;		// amplify by 12dB	
        short signed_sample = (short)sample;

        if(m_audiobuf.size() >= 240)
        {
            zmq::message_t reply (240*sizeof(short));
            memcpy (reply.data (), (unsigned char *)m_audiobuf.data(), 240*sizeof(short));
            m_zmqsocket.send (reply, zmq::send_flags::dontwait);
            usleep(9600);
            m_audiobuf.erase(m_audiobuf.begin(), m_audiobuf.begin()+240);
            m_audiobuf.push_back(signed_sample);
        }
        else
        {
            m_audiobuf.push_back(signed_sample);
        }

   }
   ::pthread_mutex_unlock(&m_TXlock);
   
    sample = 2048U;
    //m_rxBuffer.put(sample, control);

#if defined(SEND_RSSI_DATA)
    m_rssiBuffer.put(ADC->ADC_CDR[RSSI_CDR_Chan]);
#else
    m_rssiBuffer.put(0U);
#endif

    m_watchdog++;
	
}

void CIO::interruptRX()
{

    uint16_t sample = DC_OFFSET;
    uint8_t control = MARK_NONE;
    zmq::message_t mq_message;
    zmq::recv_result_t recv_result = m_zmqsocketRX.recv(mq_message, zmq::recv_flags::none);
    //usleep(9600);
    int size = mq_message.size();
    if(size < 1)
        return;
    
    ::pthread_mutex_lock(&m_RXlock);
    u_int16_t rx_buf_space = m_rxBuffer.getSpace();
    
    for(int i=0;i < size;i+=2)
    {
        short signed_sample = 0;
        memcpy(&signed_sample, (unsigned char*)mq_message.data() + i, sizeof(short));
        //m_audiobufRX.push_back(signed_sample);
        m_rxBuffer.put((uint16_t)signed_sample, control);
    }
    ::pthread_mutex_unlock(&m_RXlock);
    return;
    /*
    printf("RX buffer space %d\n", (int)rx_buf_space);
    if(rx_buf_space >= 120 && m_audiobufRX.size() >= 120)
    {
        ::pthread_mutex_lock(&m_RXlock);
        for(int i = 0;i < 120;i++)
        {
            m_rxBuffer.put((uint16_t)m_audiobufRX.at(i), control);
        }
        ::pthread_mutex_unlock(&m_RXlock);
        m_audiobufRX.erase(m_audiobufRX.begin(), m_audiobufRX.begin() + 120);
    }
    printf("Audio RX buffer size %d\n", (int)m_audiobufRX.size());
    */
}

bool CIO::getCOSInt()
{
	return m_COSint;
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
    m_COSint = on;
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

