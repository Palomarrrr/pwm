#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <signal.h>
#include <unistd.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

/* this is a shitty proof of concept thing to work on when im bored and dont want to work on anything i probably should be working on */
/* TODO LIST: add dockapp support, figure out why ueberzug is broken, add virtual monitor support, add window borders*/

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define COLOR(r, g, b) ((r<<16) + (g<<8) + b)

static int xErr(Display *display, XErrorEvent *event);

Display *mainDisplay;
XWindowAttributes winAttr;
XButtonEvent start;
XEvent event;	
Window winRoot;
XWindowChanges vals;


Atom WM_WINDOW_TYPE, WM_WINDOW_TYPE_NORMAL, WM_WINDOW_PID;

int xErr(Display *display, XErrorEvent *event) {
	(void)display;
	(void)event;
	return 0;
}


void placeBorder() {
	unsigned int n;
	Window d1, d2, *wins = NULL;
	XQueryTree(mainDisplay, winRoot, &d1, &d2, &wins, &n);
	for(int i = 0; i < n; i++) {
		wins[i] = start.subwindow;

		//XConfigureWindow(mainDisplay, start.subwindow, CWBorderWidth, &vals);
		XSetWindowBorder(mainDisplay, start.subwindow, COLOR(0, 43, 54));
		XSetWindowBorderWidth(mainDisplay, start.subwindow, 1);
	}

	if(wins) {
		XFree(wins);
	}

}


int main(int argc, char **argv) {

	if(!(mainDisplay = XOpenDisplay(0x0))){
		return 1;
	}
	winRoot = XDefaultRootWindow(mainDisplay);
	int mainScr = XDefaultScreen(mainDisplay);

	int scrHgt = XDisplayHeight(mainDisplay, mainScr);
	int scrWid = XDisplayWidth(mainDisplay, mainScr);

	XDefineCursor(mainDisplay, winRoot, XCreateFontCursor(mainDisplay, 68));
	WM_WINDOW_TYPE = XInternAtom(mainDisplay, "_NET_WM_WINDOW_TYPE", False);
	WM_WINDOW_TYPE_NORMAL = XInternAtom(mainDisplay, "_NET_WM_WINDOW_TYPE_NORMAL", False);
	WM_WINDOW_PID = XInternAtom(mainDisplay, "_NET_WM_PID", False);
	XSetWindowBorder(mainDisplay, winRoot, 1);

	XSetErrorHandler(xErr);	

	XGrabKey(mainDisplay, XKeysymToKeycode(mainDisplay, XStringToKeysym("f")), Mod4Mask, winRoot, True, GrabModeAsync, GrabModeAsync);
	XGrabButton(mainDisplay, 3, Mod4Mask, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(mainDisplay, 1, Mod4Mask, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None); 
	XGrabButton(mainDisplay, 2, Mod4Mask, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(mainDisplay, 9, None, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(mainDisplay, 4, Mod4Mask, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(mainDisplay, 5, Mod4Mask, winRoot, True, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

	start.subwindow = None;


	for(;;) {
		placeBorder();
		XNextEvent(mainDisplay, &event);
		if(event.type == KeyPress && event.xkey.subwindow != None) {
			XRaiseWindow(mainDisplay, event.xkey.subwindow);
			XSetInputFocus(mainDisplay, event.xkey.subwindow, RevertToParent, CurrentTime);
		} else if (event.type == ButtonPress && event.xbutton.subwindow != None) {
			XGetWindowAttributes(mainDisplay, event.xbutton.subwindow, &winAttr);
			start = event.xbutton;
			XSetInputFocus(mainDisplay, start.subwindow, RevertToParent, CurrentTime);
			XRaiseWindow(mainDisplay, start.subwindow);
			placeBorder();
			if(start.button == 2) { 
				XEvent killEv;
				killEv.type = ClientMessage;
				killEv.xclient.window = start.subwindow;
				killEv.xclient.message_type = XInternAtom(mainDisplay, "WM_PROTOCOLS", True);
				killEv.xclient.format = 32;
				killEv.xclient.data.l[0] = XInternAtom(mainDisplay, "WM_DELETE_WINDOW", True);
				killEv.xclient.data.l[1] = CurrentTime;
				XSendEvent(mainDisplay, start.subwindow, False, NoEventMask, &killEv);
			} else if (start.button == 9) {
				XSetInputFocus(mainDisplay, start.subwindow, RevertToParent, CurrentTime);	
			} else if (start.button == 4) {

			}
				
		} else if (event.type == MotionNotify && start.subwindow != None) {
			int xDiff = event.xbutton.x_root - start.x_root;
			int yDiff = event.xbutton.y_root - start.y_root;
			XMoveResizeWindow(mainDisplay, start.subwindow, winAttr.x + (start.button == 1 ? xDiff : 0), winAttr.y + (start.button == 1 ? yDiff : 0), MAX(1, winAttr.width + (start.button == 3 ? xDiff : 0)), MAX(1, winAttr.height + (start.button == 3 ? yDiff : 0)));
		} else if (event.type == ButtonRelease)  
			start.subwindow = None;
		
	}
	XCloseDisplay(mainDisplay);
}
