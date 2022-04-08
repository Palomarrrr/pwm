all:
	gcc -Os -pedantic -Wall -I/usr/X11R6/include pwm.c -L/usr/X11R6/lib -lX11 -o pwm
	gcc pbox.c -lX11 -o pbox
install:
	gcc -Os -pedantic -Wall -I/usr/X11R6/include pwm.c -L/usr/X11R6/lib -lX11 -o pwm
	install -m 755 pwm /usr/local/bin/pwm
	gcc pbox.c -lX11 -o pbox
	install -m 755 pbox /usr/local/bin/pbox
