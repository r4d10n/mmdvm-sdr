/*
 *   Copyright (C) 2016,2017 by Jonathan Naylor G4KLX
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

#include "SerialPort.h"
#include "SerialController.h"

#include "Log.h"

//#if defined(__SAM3X8E__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

#if defined(RPI)

#include <unistd.h>

#define VSERIAL "/dev/ptmx"  // move to Config.h

#define BUF_MAX 1024
unsigned char read_buffer;

void CSerialPort::beginInt(uint8_t n, int speed)
{
  switch (n) {
    case 1U:
     //      Serial.begin(speed);
      read_buffer = 0x00;
      m_controller = CSerialController(VSERIAL, SERIAL_115200, false);
      m_controller.open();
      break;
    default:
      break;
  }
}

int CSerialPort::availableInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return m_controller.read(&read_buffer,(uint8_t)(1 * sizeof(uint8_t)));
    default:
      return 0;
  }
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return true;
    default:
      return false;
  }
}

uint8_t CSerialPort::readInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return read_buffer;  
    default:
      return 0U;
  }
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
  switch (n) {
    case 1U:
      m_controller.write(data, length);
      break;
    default:
      break;
  }
}

#endif

