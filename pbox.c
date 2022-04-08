#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define BOX_SIZE 200
#define COLOR(r, g, b) ((r << 16) + (g << 8) + b)
Display *dpy;
int scr;
Window win;
XEvent event;
GC gc;
unsigned long blkPxl, whtPxl;


int sys_output(char **buf, char *command)
{
        FILE *fp;
        char output[1035];
        int size;

        /* Execute the command */
        fp = popen(command, "r");
        if(fp == NULL){
                fprintf(stderr, "Failed to run command: %s\n", command);
                exit(1);
        }

        /* Read the output from the command's filepointer */
        fgets(output, sizeof(output) - 1, fp);

        /* Copy the command into the buffer */
        size = strlen(output);
        *buf = malloc(size);
        strcpy(*buf, output);
        (*buf)[size - 1] = '\0';

        fclose(fp);

        return size;
}

void init() {
	dpy = XOpenDisplay(NULL);
	scr = DefaultScreen(dpy);
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 10, 10, BOX_SIZE, BOX_SIZE, 1, COLOR(0, 43, 54), COLOR(249, 245, 241));
	gc = XCreateGC(dpy, win, 0, 0);
	XSelectInput(dpy, win, ExposureMask);
	XMapWindow(dpy, win);
	XFlush(dpy);
}
	
void draw(int x, int y, char *cmd) { 
	char *output = malloc(sizeof(char));
	unsigned int len = sys_output(&output, cmd) - 1;
	XDrawString(dpy, win, gc, x, y, output, len);
	
	if(output == NULL){
		XSync(dpy, True);
		printf("ye");
	} else {
		free(output);
		XSync(dpy, True);
	}
	
}
int main(int argc, char **argv) {
	init();
	int checkIfExpose = 0;
	while(checkIfExpose == 0) {
		XNextEvent(dpy, &event);
		if(event.type == Expose) {
			checkIfExpose = 1;
		}
	}
	
	while(1){ 
	//	XNextEvent(dpy, &event);
		draw(60, 25, "echo Henlo Koishi");
		draw(60, 25, "echo ____________");
		draw(17, 50, "date");
		draw(5, 75, "~/Scripts/battery.sh");
		draw(5, 90, "~/Scripts/net.sh");
		draw(5, 105, "~/Scripts/music.sh");
		draw(5, 120, "~/Scripts/memuse.sh");
		draw(5, 135, "~/Scripts/cputemp.sh");
		usleep(500000); // in microseconds since its more flexible than sleep()
		XClearWindow(dpy, win);
	}

	return 0;
}

