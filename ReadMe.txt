A volume ray-marcher for rendering Mandelboxen.

ray_march [CONFIG_FILE] [WIDTH] [HEIGHT]

A sample config file can be found in the docs directory. If width and height 
specified on the command line, they will override the values in the config 
file.

There is an OpenCL implementation which gives some speed improvements but 
locks the GPU pretty hard. On Windows Vista with will cause TDR to kick in 
and break the renderer. You can turn it off via the registry:

http://msdn.microsoft.com/en-us/library/windows/hardware/ff569918.aspx

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
