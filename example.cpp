#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fftw3.h>
#include <unistd.h>
#include <curses.h> /* needed to clear the screen */	
#include <math.h>

using namespace std;

const int SRATE = 44100;
#define BSIZE	22050	/* Buffer size */
#define SAMPLES 512

ALbyte buffer[BSIZE];
ALint sample;

void printFrequencies ( int n, double *out )
{
    int i = 0;
	double c, s, r;
    
	/* print first line seperatly */
	c = out[2*i]/n;
	r = log(sqrt(c*c));
	printf("%f\n", r);
	
	
    for ( i=1; 2*i<n; i++ ) {
		c = 2*out[2*i]/n;
		s = -2*out[2*i+1]/n;
 	    r = log(sqrt(c*c + s*s));
		printf("%f\n", r);
	}

	/* print last line seperatly */
	if ( n % 2 == 0 ) {
		c = out[2*i]/n;
		r = log(sqrt(c*c));
		printf("%f\n", r);
	}
}
	
int main(int argc, char *argv[]) {
	// retrieve all old errors
	alGetError();

	printf( "There was an error while opening device! Error number: %d\n", alGetError());

	// Enumerate OpenAL devices
	if (alcIsExtensionPresent (NULL, (const ALCchar *) "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char *s = (const char *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
		while (*s != '\0')
		{
			cout << "OpenAL available device: " << s << endl;
			while (*s++ != '\0');
		}
	}
	else
	{
		cout << "OpenAL device enumeration isn't available." << endl;
	}

    /* Display default device */
    const char *szDefaultCaptureDevice = alcGetString(NULL,ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER); 
    cout << szDefaultCaptureDevice << endl;

    ALCdevice *device = alcCaptureOpenDevice(szDefaultCaptureDevice, SRATE ,  AL_FORMAT_MONO16, SAMPLES);
    ALenum errno = alGetError();
    if (errno != AL_NO_ERROR) {
		switch(errno) {
		case ALC_INVALID_VALUE:
			cout << "Invalide Value";
			break;
		case ALC_OUT_OF_MEMORY:
			cout << "Out of memory";
			break;
		default:
			cout << "Unkown error";
		}
    }

	fftw_complex *out;
	double value;
	double* fft_in = (double*) fftw_malloc(sizeof(double)*SAMPLES);

	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SAMPLES/2+1);

	fftw_plan p = fftw_plan_dft_r2c_1d(SAMPLES, fft_in, out, FFTW_ESTIMATE);

    alcCaptureStart(device);
	
	int16_t* pBuffer16 = (int16_t *) &buffer;
	uint64_t counter = 0;
	FILE *fp = fopen("/tmp/daimudda.dump", "w+");
	if (fp == NULL) {
		cout << "nooooooooooooooooooooo" << endl;
		return(666);
	}

	/* -------------------------- ncurses -------------------------- */	
	while(true) {
		alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
		system("clear");
		printf("Sample amount: %d\tAim:%d\n", sample, SAMPLES);
		
		if (sample >= SAMPLES)
		{
			alcCaptureSamples(device, (ALCvoid *)buffer, SAMPLES);
			
		   	for(int i=0; i < SAMPLES; i++) {
				fprintf (fp, "%lld\t%d\n", counter++, pBuffer16[i]);

				/* store the value in the inbuffer */
				value = (double) pBuffer16[i];
				fft_in[i] = value;

			}
			fftw_execute(p);

			fflush(fp);
			
			printFrequencies ( SAMPLES, (double* )out ); 
		}
		usleep(4000);
	}

    return 0;
}

