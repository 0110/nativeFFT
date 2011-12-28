#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fftw3.h>
#include <unistd.h>
#include <math.h>

using namespace std;

const int SRATE = 44100;
#define BSIZE	22050	/* Buffer size */
#define SAMPLES 128

#define COMPRESSION_FACTOR 10
#define COMPRESSED_SIZE ((SAMPLES / COMPRESSION_FACTOR) + 1)

ALbyte buffer[BSIZE];
ALint sample;

/*
 * Reduce the amount of data!
 * @param[in] 	plainFFT	The captured sample, that was modified by the FFT
 * @param[out]	compressed	There only a compressed data is returned to work on.
 */
void printFrequencies (double *plainFFT, double *compressed)
{
    int i = 0, compressedIndex = 0;
	double c, s, r, value;
    
	/* handle first line seperatly */
	c = plainFFT[2*i]/SAMPLES;
	r = log(sqrt(c*c));
	/* store the first captured value */
	value = r;
	
//	printf("%f\n", r); //TODO debug code
	
	
    for ( i=1; 2*i < SAMPLES; i++ ) {
		c = 2*plainFFT[2*i]/SAMPLES;
		s = -2*plainFFT[2*i+1]/SAMPLES;
 	    r = log(sqrt(c*c + s*s));
		value += r; /* add the actual value */
		if (i % COMPRESSION_FACTOR == 0) {
			compressed[compressedIndex++] = value / COMPRESSION_FACTOR;
			value = 0;
		}
//		printf("%f\n", r); /TODO debug code
	}

	/* handle last line seperatly */
	if ( SAMPLES % 2 == 0 ) {
		c = plainFFT[2*i]/SAMPLES;
		r = log(sqrt(c*c));
		value += r;
//		printf("%f\n", r); //TODO debug code
	}
	// Add the last
	compressed[compressedIndex++] = (value / (i % COMPRESSION_FACTOR));
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

	/* prepare everything for the FFT */
	fftw_complex *out;
	double value;
	double* fft_in = (double*) fftw_malloc(sizeof(double)*SAMPLES);
	double* compressed = (double*) malloc(sizeof(double) * COMPRESSED_SIZE);

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
		printf("\rSample amount: %d\tAim:%d", sample, SAMPLES);
		
		if (sample >= SAMPLES)
		{
			alcCaptureSamples(device, (ALCvoid *)buffer, SAMPLES);
			printf("\nThere was some data found:\n");
			
		   	for(int i=0; i < SAMPLES; i++) {
				fprintf (fp, "%lld\t%d\n", counter++, pBuffer16[i]);

				/* store the value in the inbuffer */
				value = (double) pBuffer16[i];
				fft_in[i] = value;

			}
			fftw_execute(p);

			fflush(fp);
			
			printFrequencies ((double* )out, compressed); 

			for(int i=0; i < COMPRESSED_SIZE; i++) {
				printf("%lf\n", compressed[i]);
			}
			
			return 0; /* we have one sample captured... exit */
		}
		usleep(4000);
	}

    return 0;
}

