#! /usr/bin/env python
'''
Original Author: Trevor Bramwell (bramwelt@onid.orst.edu)
File: bramwelt_proj1_4.py

This file is my implementation of the fourth part
    of project one for CS311 (MWF 0800).
'''
print sum([num for num in xrange(1000) if (num % 3) == 0 or (num % 5) == 0])
