/*******************************************************************************
		Generate Artnet packages
*******************************************************************************/
#include <stdio.h>
#include "artnet_send.h"
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	int i;
	char mesg[]="Enter a string: ";		/* message to be appeared on the screen */
	char c;
	int row,col;				/* to store the number of rows and *
								 * the number of colums of the screen */
	
	if (argc != 2) { 
		printf("Usage: server\n");
		return 1;
	}
	
	artnet_send_t* universe;
	
	universe = artnet_send_create(argv[1], 0, 512);
	
	initscr();				/* start the curses mode */
	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
	
	mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);
	
//	mvprintw(LINES - 2, 0, "You Entered: %s", str);
	while (1) {
		c = getch();
		mvprintw(LINES - 2, 0, "You Entered: %c", c);
		switch (c) {
			case 'q':
				universe->data[i] = universe->data[i] + 1;
				artnet_send_send(universe);
				usleep(20000);
				break;
			case 'a':
				universe->data[i] = universe->data[i] - 1;
				artnet_send_send(universe);
				usleep(20000);
				break;
				
			default:
				break;
		}
		usleep(20000);
	}
	endwin();
	
    return 0;
}

