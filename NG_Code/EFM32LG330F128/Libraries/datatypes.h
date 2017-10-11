/*
 * datatypes.h
 *
 *  Created on: Nov 30, 2015
 *      Author: riley.culligan
 */

#ifndef LIBRARIES_DATATYPES_H_
#define LIBRARIES_DATATYPES_H_

/******************************************************************************
 * Typedefs
 */
typedef enum {BUTOFF, BUTON, BUTWAIT} BUTTONSTATETYPE;
typedef enum {
    BLACK   = 0,
    WHITE   = 0b111,
    RED     = 0b100,
    GREEN   = 0b010,
    BLUE    = 0b001,
    YELLOW  = 0b110,
    PURPLE  = 0b101,
    CYAN    = 0b011
} LEDCOLOR;
typedef enum {BUCK, BOOST} CONVERTERSTATETYPE;

#endif /* LIBRARIES_DATATYPES_H_ */
