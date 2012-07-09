//=================================
// include guard
#ifndef __STEPS_H_INCLUDED__
#define __STEPS_H_INCLUDED__

#ifndef __MATH_H_INCLUDED__
    #define __MATH_H_INCLUDED__
    #include <math.h>
#endif

#ifndef __SYSTEM_CONSTANTS_H_INCLUDED__
    #define __SYSTEM_CONSTANTS_H_INCLUDED__
    
#endif

#define PI 3.14159265

double xBase(float xi, float yi)                // 
{
    return atan(xi/(sqrt(D*D + yi*yi) + AB) );
}


//=================================
// Absolute Steps (rotational position)
float stepsX (float xi, float yi)
{
    return (xBase(xi, yi) / MICROSTEP_ANGLE );
}

float stepsY (float yi)
{
    return ( atan(yi/D) / MICROSTEP_ANGLE );
}

//=================================
// Relative Steps (change in rotational position)
float stepsX (float xi, float yi, float x0)
{
    return (stepsX(xi, yi) - stepsX(x0, yi));
}

float stepsY (float yi, float y0)
{
    return( stepsY(yi) - stepsY(y0) );
}

#endif // __STEPS_H_INCLUDED__ 
