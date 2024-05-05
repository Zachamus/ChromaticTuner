#include "complexnums.h"
#include <math.h>

complex complex_from_polar(double r, double theta_radians) {
    complex result;
    result.re = r * cos(theta_radians);
    result.im = r * sin(theta_radians);
    return result;
}
double complex_magnitude(complex c) {
    return sqrt(c.re*c.re + c.im*c.im);
}
complex complex_add(complex left, complex right) {
    complex result;
    result.re = left.re + right.re;
    result.im = left.im + right.im;
    return result;
}
complex complex_sub(complex left, complex right) {
    complex result;
    result.re = left.re - right.re;
    result.im = left.im - right.im;
    return result;
}
complex complex_mult(complex left, complex right) {
    complex result;
    result.re = left.re*right.re - left.im*right.im;
    result.im = left.re*right.im + left.im*right.re;
    return result;
}
