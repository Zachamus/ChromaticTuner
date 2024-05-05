#include "fft.h"
#include "complex.h"
#include "trig.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#define SAMPLES 1024
#define MAX_BINS 1024
static complex out[SAMPLES] = {0};
static float new_[SAMPLES] = {0};
static float new_im[SAMPLES];// (complex*) malloc(sizeof(struct complex_t) * N);
static complex scratch[SAMPLES] = {0}; //(complex*) malloc(sizeof(struct complex_t) * N);


void FFT_calculate(complex* x, int N /* must be a power of 2 */,
        complex* X, complex* scratch, complex* twiddles) {
    int k, m, n;
    int skip;
    bool evenIteration = N & 0x55555555;
    complex* E;
    complex* Xp, * Xp2, * Xstart;
    if (N == 1) {
    	X[0] = x[0];
    	return;
    }
    E = x;
    for (n = 1; n < N; n *= 2) {
        Xstart = evenIteration? scratch : X;
        skip = N/(2 * n);
        /* each of D and E is of length n, and each element of each D and E is
        separated by 2*skip. The Es begin at E[0] to E[skip - 1] and the Ds
        begin at E[skip] to E[2*skip - 1] */
        Xp = Xstart;
        Xp2 = Xstart + N/2;
        for(k = 0; k != n; k++) {
        	float tim = twiddles[k * skip].im;
        	float tre = twiddles[k * skip].re;
            for (m = 0; m != skip; ++m) {
            	complex* D = E + skip;
            	/* twiddle *D to get dre and dim */
            	float dre = D->re * tre - D->im * tim;
            	float dim = D->re * tim + D->im * tre;
                Xp->re = E->re + dre;
                Xp->im = E->im + dim;
                Xp2->re = E->re - dre;
                Xp2->im = E->im - dim;
                ++Xp;
                ++Xp2;
                ++E;
            }
            E += skip;
        }
        E = Xstart;
        evenIteration = !evenIteration;
    }
}
void FFT_get_twiddle_factors(int N, complex* twiddles) {
    //complex* twiddles = (complex*) malloc(sizeof(struct complex_t) * N);
    int k;
    for (k = 0; k != N; ++k) {
        twiddles[k] = complex_from_polar(1.0, -2.0*PI*k/N);
    }
}



float FFT_simple(complex* x, int N /* must be a power of 2 */, complex* twiddles, int drawHist, float sample_f) {

     //FFT_get_twiddle_factors(N);
    FFT_calculate(x, N, out, scratch, twiddles);

//    float sample_f = 100*1000*1000/2048.0;
    	float s=sample_f/N;
    	//printf("s: %f\r\n", s);
    	float max=0;
    	float frequency = 0;
    	int place=1;
    		for(int i=3;i<(N>>1);i++) {
    			new_[i]=out[i].re*out[i].re+out[i].im*out[i].im;
    //			printf("new_[i]: %f\r\n", new_[i]);
    			if(max < new_[i]) {
    				max=new_[i];
    				place = i;
    			}
    		}

    		frequency = s*place;

    		float y1=new_[place-1],y2=new_[place],y3=new_[place+1];
    			float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
    			x0=x0/s-1;

    			if(x0 <0 || x0 > 2) { //error
    				return 0;
    			}
    			if(x0 <= 1)  {
    				frequency=frequency-(1-x0)*s;
    			}
    			else {
    				frequency=frequency+(x0-1)*s;
    			}
    			int hist[MAX_BINS];
    				if (drawHist) {
    					int y = 0;
    					for (int i = 1; i < N>>1; i++) {
    						float value = (new_[i] / max * 100.0);
    						hist[i] = (int) (value + 0.5);
    						y = i + 25;
    						setColor(147, 139, 161);
    						drawHLine(20 + hist[i], y, 150);
    						setColor(28, 68, 142);
    						drawHLine(20, y, hist[i]/2);
    					}
    				}
    			return frequency;


}
