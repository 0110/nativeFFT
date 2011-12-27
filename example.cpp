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
	const ALCchar *defaultDevice = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	cout << "Default capture device " << defaultDevice << endl;
	
    ALCdevice *device = alcCaptureOpenDevice(defaultDevice, SRATE, AL_FORMAT_STEREO16, SSIZE);
	if (device == NULL) {
		cout << "Could not create a capture device" << endl;
	}
	
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
	printf( "There was an error while opening device! Error: %s (%d)\n", alGetString(errno), errno);
		
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

