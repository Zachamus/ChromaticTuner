#ifndef _COMPLEX_NUMS_H
#define _COMPLEX_NUMS_H

typedef struct complex_t {
    float re;
    float im;
} complex;

complex complex_from_polar(double r, double theta_radians);
double  complex_magnitude(complex c);
complex complex_add(complex left, complex right);
complex complex_sub(complex left, complex right);
complex complex_mult(complex left, complex right);


#endif
