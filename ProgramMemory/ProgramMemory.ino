/**
 * @file ProgramMemory.ino
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
 * This Arduino sketch shows how to get data structures into
 * program memory and how to access them.
 *
 */

#include "Types.h"
#include "Memory.h"
#include "Serial.h"
#include "Point.h"

// Constant data structure in program memory
const PROGMEM Point fie = { 
  4,
  2 
};

const PROGMEM Point fum = { 
  6,
  7
};

// Vector of pointers to data structure in program memory
const PROGMEM Point_P foo[] = {
  &fie,
  &fum
};

void setup()
{
  // Start serial interface
  Serial.begin(9600);
  delay(100);

  // Check amount of free memory
  Serial_print("free memory = ");
  Serial.println(free_memory());

  // Print contents of data structure in program memory
  Serial_print("fie = ");
  Point::print(&fie);

  // Copy data from program memory to stack variable and print
  Point tmp;
  Point::copy(tmp, &fum);
  Serial_print("fum = ");
  Point::print(tmp);
  
  // Access pointers to data in program memory and print
  for (uint8_t i = 0; i < membersof(foo); i++) {
    Serial_print("foo[");
    Serial.print(i);
    Serial_print("] = ");
    Point::print(Point::get_at(foo, i));
  }
}

void loop()
{
}

