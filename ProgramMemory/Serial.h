/**
 * @file Serial.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Extension of the jeelabs workaround for strings in program memory
 * See http://jeelabs.org/2011/05/23/saving-ram-space/
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <avr/pgmspace.h>
#include <HardwareSerial.h>

void __Serial_print(PGM_P s)
{
  char c;
  while ((c = pgm_read_byte(s++)) != 0)
    Serial.print(c);
}

#define Serial_print(x) __Serial_print(PSTR(x))

#endif

