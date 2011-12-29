/*
 * audiocapture.c
 *
 * Copyright (C) 2011 - ollo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "audiocapture.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <fftw3.h>

/* handle c99 fuckups */
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define TRUE	1

#define SRATE	44100
#define BSIZE	22050	/* Buffer size */
#define SAMPLES 256

#define COMPRESSION_FACTOR 10
#define COMPRESSED_SIZE ((SAMPLES / COMPRESSION_FACTOR) + 1)

ALbyte buffer[BSIZE];
ALint sample;


static double max(double a, double b)
{
	if (a > b)
		return a;
	else
		return b;
}

/*
 * Reduce the amount of data!
 * @param[in] 	plainFFT	The captured sample, that was modified by the FFT
 * @param[out]	compressed	There only a compressed data is returned to work on.
 */
void convertData(double *plainFFT, double *compressed)
{
    int i = 0, compressedIndex = 0;
	double c, s, r, value = 0.0;
    
	/* handle first line seperatly */
	c = plainFFT[2*i]/SAMPLES;
	r = log(sqrt(c*c));
	/* store the first captured value */
	value = max(value, r);
	
//	printf("%f\n", r); //TODO debug code
	
	
    for ( i=1; 2*i < SAMPLES; i++ ) {
		c = 2*plainFFT[2*i]/SAMPLES;
		s = -2*plainFFT[2*i+1]/SAMPLES;
 	    r = log(sqrt(c*c + s*s));
		value = max(value, r); /* add the actual value */
		if (i % COMPRESSION_FACTOR == 0) {
			compressed[compressedIndex++] = value;
			value = 0;
		}
//		printf("%f\n", r); /TODO debug code
	}

	/* handle last line seperatly */
	if ( SAMPLES % 2 == 0 ) {
		c = plainFFT[2*i]/SAMPLES;
		r = log(sqrt(c*c));
		value = max(value, r); /* add the actual value */
//		printf("%f\n", r); //TODO debug code
	}
	// Add the last
	compressed[compressedIndex++] = value;
}


AUDIO_CAPTURE_RET audiocapture_init()
{
	// retrieve all old errors
	alGetError();

	printf( "There was an error while opening device! Error number: %d\n", alGetError());

	// Enumerate OpenAL devices
	if (alcIsExtensionPresent (NULL, (const ALCchar *) "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char *s = (const char *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
		while (*s != '\0')
		{
			printf("OpenAL available device: %s\n", s);
			while (*s++ != '\0');
		}
	}
	else
	{
		printf("OpenAL device enumeration isn't available.");
	}

    /* Display default device */
    const char *szDefaultCaptureDevice = alcGetString(NULL,ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER); 
    printf("%s\n", szDefaultCaptureDevice);

	/* Here the outputdevice could be specified */
    ALCdevice *device = alcCaptureOpenDevice(NULL, SRATE ,  AL_FORMAT_MONO16, SAMPLES);
    ALenum errno = alGetError();
    if (errno != AL_NO_ERROR) {
		switch(errno) {
		case ALC_INVALID_VALUE:
			printf("Invalide Value\n");
			break;
		case ALC_OUT_OF_MEMORY:
			printf("Out of memory\n");
			break;
		default:
			printf("Unkown error\n");
		}
    }

	/* prepare everything for the FFT */
	fftw_complex *out;
	double value;
	int i;
	double* fft_in = (double*) fftw_malloc(sizeof(double)*SAMPLES);
	double* compressed = (double*) malloc(sizeof(double) * COMPRESSED_SIZE);

	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SAMPLES/2+1);

	fftw_plan p = fftw_plan_dft_r2c_1d(SAMPLES, fft_in, out, FFTW_ESTIMATE);

    alcCaptureStart(device);
	
	int16_t* pBuffer16 = (int16_t *) &buffer;
	uint64_t counter = 0;
	FILE *fp = fopen("/tmp/daimudda.dump", "w+");
	if (fp == NULL) {
		printf("nooooooooooooooooooooo\n");
		return(666);
	}

	/* build lookuptable for windowfunction (antilazing) */
	/* algorithm does: fading in at the left side and fading out at the right side */
	double windowLookup[SAMPLES];
	/* calulate the vales*/
	for(i=0; i < SAMPLES; i++) {
		windowLookup[i] = 0.5 * (1 - cos((2 * M_PI * i) / (SAMPLES - 1)));
	}

	/* -------------------------- ncurses -------------------------- */	
	while(TRUE) {
		alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
		printf("\rSample amount: %d\tAim:%d", sample, SAMPLES);
		
		if (sample >= SAMPLES)
		{
			alcCaptureSamples(device, (ALCvoid *)buffer, SAMPLES);
			printf("\nThere was some data found:\n");
			
		   	for(i=0; i < SAMPLES; i++) {
				fprintf (fp, "%ld\t%d\n", counter++, pBuffer16[i]);

				/* store the value in the inbuffer */
				value = ((double) pBuffer16[i]);

			    /* use the table for fading in and out */
				fft_in[i] = value * windowLookup[i];

			}
			fftw_execute(p);

			fflush(fp);
			
			convertData ((double*)out, compressed); 
			printf("The compressed size is %d (compressing %d), the orignal was %d\n", COMPRESSED_SIZE, COMPRESSION_FACTOR, SAMPLES);
			
			for(i=0; i < COMPRESSED_SIZE; i++) {
				printf("%lf\n", compressed[i]);
			}			
		}
		usleep(4000);
	}
	return AUDIO_CAPTURE_RET_OK;
}
