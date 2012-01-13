/*******************************************************************************
		Generate Artnet packages
*******************************************************************************/
#include <stdio.h>
#include "artnet_send.h"

int main(int argc, char *argv[]) {

	int i;
	
	if (argc != 2) { 
		printf("Usage: server\n");
		return 1;
	}
	
	artnet_send_t* universe;
	
	universe = artnet_send_create(argv[1], 0, 512);
	for (i=0; i < 4; i++) {
		universe->data[i] = 255;
	}
	artnet_send_send(universe);
	
    return 0;
}

