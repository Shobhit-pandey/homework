#! /usr/bin/env python
'''
Original Author: Trevor Bramwell (bramwelt@onid.orst.edu)
File: bramwelt_proj1_2.py

This file is my implementation of the second part
    of project one for CS311 (MWF 0800).
'''
import getopt
import os
import sys

VERSION = '0.1'
PROG = 'DirCreator'
DESC = 'Create directories for a given term and course'

USAGE = '''
    %(prog)s [-h]
    %(prog)s [-v]
    %(prog)s [-t, --term term] [-c, --course course]
''' % {'prog':PROG}

LONG_DESC = """
This script will creates the following directories for a 
    given term and course:
  * assignments
  * examples
  * exams
  * lecture_notes
  * submissions

optional arguments:
  -h, --help        show this help message and exit
  -v, --version     show program's version number and exit

required arguments:
  -t, --term        the directory to hold the course directory
  -c, --course      the directory to hold the subdirectories listed above
"""

DIRS = ['assignments', 'examples', 'exams', 'lecture_notes',
    'submissions']

DIR_EXISTS = '\tDirectory %s/ already exists.'
DIRS_EXIST = '\tDirectories %s/ already exists.'
MK_DIR = 'Creating directory: %s/'
MK_DIRS = 'Creating directories: %s/'

def get_version():
    '''
    Provide the program name and version
    '''
    return '%s %s' % (PROG, VERSION)


def usage():
    '''
    Provide the usage of the program
    '''
    return '%s usage: %s' % (PROG, USAGE)


def mkdir(directory):
    '''
    Creates the given directory in the current working
        directory.
    '''
    try:
        os.mkdir(directory)
    except OSError, err:
        print DIR_EXISTS % directory
        

def mkdirs(root, leafs):
    '''
    Given a list of directories (leafs) create them under
        the root directory, after creating the root
        directory as well.
    '''
    output = MK_DIR
    if isinstance(root, list):
        # Unpack root list to create path for makedirs
        path = os.path.join(*root)
        try:
            print MK_DIRS % path
            os.makedirs(path)
        except OSError, err:
            print DIRS_EXIST % path
        os.chdir(path)
    else:
        path = mkdir(root)

    # Set output to include path, so that subpaths can be
    #  added on.
    output = output % path

    for leaf in leafs:
        print '%s%s' % (output, leaf)
        mkdir(leaf)


def main():
    '''
    Recieve arguments and options from the command line
    '''
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'hvt:c:',
            ['help', 'version', 'term=', 'course=']
        )
    except getopt.GetoptError, err:
        print usage()
        print str(err)
        sys.exit(2)
    
    # Create variables for course and term
    course = None
    term = None

    # Parse option list
    if optlist == []:
        # No options, print usage
        print usage()
        print 'error: no options provided'
    else:
        # Handle options
        for opt, arg in optlist:
            if arg.startswith('-'):
                print 'error: an argument "%s" was provided to ' \
                    'option "%s"'% (arg, opt)
                sys.exit(2)
            if opt in ('-h', '--help'):
                print usage(),
                print LONG_DESC 
                sys.exit()
            elif opt in ('-v', '--version'):
                print get_version()
                sys.exit()
            elif opt in ('-t', '--term'):
                term = arg
            elif opt in ('-c', '--course'):
                course = arg
            else:
                assert False, 'Unhandled option'

    if course and term:
        mkdirs([term, course], DIRS)


if __name__ == '__main__':
    main()
