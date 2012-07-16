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

double xBase(double xi, double yi)                // 
{
    return atan(xi/(sqrt(D*D + yi*yi) + AB) );
}


//=================================
// Absolute Steps (rotational position)
double stepsX (double xi, double yi)
{
    return (xBase(xi, yi) / MICROSTEP_ANGLE );
}

double stepsY (double yi)
{
    return ( atan(yi/D) / MICROSTEP_ANGLE );
}

//=================================
// Relative Steps (change in rotational position)
double stepsX (double xi, double yi, double x0)
{
    return (stepsX(xi, yi) - stepsX(x0, yi));
}

double stepsY (double yi, double y0)
{
    return( stepsY(yi) - stepsY(y0) );
}

#endif // __STEPS_H_INCLUDED__ 
