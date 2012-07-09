#ifndef __SYSTEM_CONSTANTS_H_INCLUDED__
#define __SYSTEM_CONSTANTS_H_INCLUDED__
             
             
const float PI = 3.14159265;

const unsigned int STEPSPERREV = 2096;    // Steps per revolution
const unsigned int MICROSTEPRES = 16;     // Microsteps in each step

const float AB = 10.0;                        // Distance between center of X and Y mirrors (mm)
const float D = 10.0;                         // Distance from center of Y mirror to focal plane (mm)


const double MICROSTEP_ANGLE = 2 * PI / (MICROSTEPRES * STEPSPERREV);

#endif
