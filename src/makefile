HEADERS = ray_march.h save_png.h vector.h config.h
OBJECTS = main.o ray_march.o save_png.o config.o vector.o
LIBS = -lz -lpng14

default : ray_march

%.o : %.c $(HEADERS)
	gcc -c $< -I../lib -o $@

ray_march : $(OBJECTS)
	gcc $(OBJECTS) -o $@ -L../lib/rpi -lm $(LIBS)

clean :
	-rm -f $(OBJECTS)
	-rm -f ray_march

