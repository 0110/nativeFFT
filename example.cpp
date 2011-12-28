#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <stdio.h>
#include <inttypes.h>
#include <fftw3.h>
#include <unistd.h>

using namespace std;

const int SRATE = 44100;
const int SSIZE = 1024;
#define BSIZE	22050	/* Buffer size */
#define SAMPLES 1024

ALbyte buffer[BSIZE];
ALint sample;

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

    ALCdevice *device = alcCaptureOpenDevice(szDefaultCaptureDevice, SRATE , AL_FORMAT_STEREO16, SAMPLES);
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
	double* fft_in = (double*) fftw_malloc(sizeof(double)*SAMPLES);

	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SAMPLES/2+1);

	fftw_plan p = fftw_plan_dft_r2c_1d(SAMPLES, fft_in, out, FFTW_ESTIMATE);

    alcCaptureStart(device);
	
	uint16_t* pBuffer16 = (uint16_t *) buffer;
	while(true) {
		alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
		alcCaptureSamples(device, (ALCvoid *)buffer, sample);
		if (sample > 0)
		{
			cout << sample <<endl;
		   	for(int i=0; i < (BSIZE >> 1); i+=2) {
			  	double value = (double) (pBuffer16[i] + pBuffer16[i+1]) / 2; // Linken und rechten Kanal addieren
				value = value / 32768;

				fft_in[i/2] = value;
			}

			fftw_execute(p);
		}
		
		usleep(1000);
	}
    return 0;
}

