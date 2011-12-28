#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <stdio.h>
#include <fftw3.h>

using namespace std;

const int SRATE = 44100;
const int SSIZE = 1024;
#define SAMPLES 1024

ALbyte buffer[22050];
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
	cout << endl;
	printf( "There was an error while opening device! Error number: %s\n", alGetString(errno));
		

    }

fftw_complex *out;
double* in = (double*) fftw_malloc(sizeof(double)*SAMPLES);

out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*SAMPLES/2+1);

fftw_plan p = fftw_plan_dft_r2c_1d(SAMPLES, in, out, FFTW_ESTIMATE);

    alcCaptureStart(device);
while(true) {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
        alcCaptureSamples(device, (ALCvoid *)buffer, sample);
        
}
    return 0;
}

