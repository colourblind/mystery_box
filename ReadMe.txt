A volume ray-marcher for rendering Mandelboxen.

ray_march [CONFIG_FILE] [WIDTH] [HEIGHT]

A sample config file can be found in the docs directory. If width and height 
specified on the command line, they will override the values in the config 
file.

There is an OpenCL implementation which gives some speed improvements assuming
you have a decent GPU, but you must have OpenCL 1.1 drivers and your image 
dimensions must be multiples of 128 (unless you hack the work_size values in 
the go_cl function).

The makefile provided builds a binary for Raspberry Pi. I realise how silly 
that is.

Coattails ridden:

Tom 'Tglad' Lowe
Mikael Hvidtfeldt Christensen
Various @ FractalForums.com

To-do:

- Tidy up Raspberry Pi support
- Get fog back in
- Speed optimisations
- AA and depth of field?
