munin-c
=======

C rewrite of munin node components. It currently consists on a light-weight 
node, and some plugins. These are designed to be very light on resources,
and compatible with the stock ones.

Compatibility
-------------

A main design feature is that munin-c components are drop-ins to stock ones.
Therefore the munin-c node can run the stock plugins, and the plugins can be 
run by the stock node. There are still some missing parts in the node, mostly 
about the permission, and the environnements. 

The config file has also the same syntax.

3rd-party plugins should also work unmodified.

Compiling
=========
This project has been ported to autotools. You have to use::

    autoreconf -i -I m4 && ./configure && make



Contribute and coding style
===========================
Contribute by open an issue or pull-request at
https://github.com/munin-monitoring/munin-c

When contributing code please follow the kernel coding style. 
Use GNU indent with the parameters to follow the Kernighan & Ritchie coding
style and set the indention level to 8 spaces::

    find . -iname "*.c" -exec indent -kr -i8 {} \;

License
=======
munin-c is licensed as gpl-2 or gpl-3 at your choice.
