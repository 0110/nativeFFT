/*******************************************************************************
		Capturing data from the soundcard and visualize its bands
*******************************************************************************/
#include "audiocapture.h"
#include <stdio.h>
#include "artnet_send.h"


void handleIncomeingSample(const double* array, const int length)
{

}

int main(int argc, char *argv[]) {

	AUDIO_CAPTURE_RET ret = audiocapture_init(&handleIncomeingSample);
	if (AUDIO_CAPTURE_RET_OK != ret)
	{
		printf("There was an error while initing the capturing device\n");
		return 1;
	}

	
	
    return 0;
}

