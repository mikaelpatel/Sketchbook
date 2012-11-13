/**
 * @file Point.h
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
 * Simple data structure definition that may be initiated in program
 * memory.
 */

#ifndef __POINT_H__
#define __POINT_H__

#include <stdint.h>
#include <avr/pgmspace.h>

// Data structure definition to allow constant value in program memory
typedef struct Point Point;
typedef const PROGMEM Point* Point_P;

// Data structure with static functions to make the namespace Point::
struct Point {

  // Member variables
  int8_t m_x;
  int8_t m_y;

  // Access functions for data structure 
  static inline uint8_t get_x(const Point& p) 
  { 
    return (p.m_x); 
  }

  static inline uint8_t get_y(const Point& p) 
  { 
    return (p.m_y); 
  }
  
  // Access functions for program memory
  static inline uint8_t get_y(Point_P p) 
  { 
    return (pgm_read_byte(&p->m_y)); 
  }

  static inline uint8_t get_x(Point_P p) 
  { 
    return (pgm_read_byte(&p->m_x)); 
  }

  // Vector access function
  static inline Point_P get_at(const Point_P p[], uint8_t ix)
  {
    return ((Point_P) pgm_read_word(&p[ix]));
  }

  // Copy from program memory
  static inline void copy(Point& p, Point_P q)
  {
    memcpy_P(&p, q, sizeof(Point));
  }

  static inline Point copy(Point_P q)
  {
    Point p;
    memcpy_P(&p, q, sizeof(Point));
    return (p);
  }

  // Debug support functions: Print contents of data structure 
  static void print(const Point& p)
  {
#ifndef NDEBUG
    Serial_print("Point(x = ");
    Serial.print(Point::get_x(p));
    Serial_print(", y = ");
    Serial.print(Point::get_y(p));
    Serial_print(")\n");
#endif
  }

  static void print(Point_P p)
  {
#ifndef NDEBUG
    Point tmp = copy(p);
    print(tmp);
#endif
  }

};

#endif
