Linux Readme
============

On a Debian system ensure you have these libraries installed:

    * freeglut3
    * freeglut3-dev
    * freeglew-dev

These should include all the other dependencies you need to compile.
Obviously you should have build-essential installed.

The Makefile has been updated to correctly set the linking flags to the
END of the gcc call.

Building & Running
------------------

> cd src
> make
> ./prog perspective.glsf ../assets/*.obj


Transformations
---------------

Keyboard keys switch state.

T - Transform
R - Rotate
G - Scale
