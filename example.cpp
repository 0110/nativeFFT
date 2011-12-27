#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <stdio.h>
using namespace std;

const int SRATE = 44100;
const int SSIZE = 1024;

ALbyte buffer[22050];
ALint sample;

int main(int argc, char *argv[]) {
	// retrieve all old errors
	alGetError();

	// Enumerate OpenAL devices
	if (alcIsExtensionPresent (NULL, (const ALCchar *) "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char *s = (const char *) alcGetString(NULL, ALC_DEVICE_SPECIFIER);
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

	
    ALCdevice *device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);
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
		
        return 0;
    }
    alcCaptureStart(device);

    while (true) {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
        alcCaptureSamples(device, (ALCvoid *)buffer, sample);

        // ... do something with the buffer 
	
    }

    alcCaptureStop(device);
    alcCaptureCloseDevice(device);

    return 0;
}

