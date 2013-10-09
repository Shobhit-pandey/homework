#! /usr/bin/env python
'''
Original Author: Trevor Bramwell (bramwelt@onid.orst.edu)
File: bramwelt_proj1_2.py

This file is my implementation of the third part
    of project one for CS311 (MWF 0800).
'''
import getopt
import re
import sys
import urllib

VERSION = '0.1'
PROG = 'PhoneScraper'
DESC = 'Print all phone numbers found on the page retrieved by the given URL'

USAGE = '''
    %(prog)s [-h]
    %(prog)s [-v]
    %(prog)s URL
''' % {'prog':PROG}

LONG_DESC = """
optional arguments:
  -h, --help        show this help message and exit
  -v, --version     show program's version number and exit

required arguments:
  url           the url to check for phone numbers
"""

# This regular expression modified from
#   http://regexlib.com/REDetails.aspx?regexp_id=588

RE_PHONE = r'''
    ([01]       # Optional 0 or 1 with
    [- .])?     #   optional seperator of a dash, space, or period
    \(?         # Optional parenthesis
    [2-9]       # First digit of area code must not be 0 or 1
    \d{2}       # Next two digits can be 0-9
    \)?         # Optional closing parathesis
    [- .]?      # Optional seperator is either a dash, space, or period
    \d{3}       # Next 3 digits
    [- .]?      # Optional seperator
    \d{4}       # Last 4 digits
'''


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


def main():
    # Get url from the command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hv',
            ['help', 'version']
        )
    except getopt.GetoptError, err:
        print usage()
        print str(err)
        sys.exit(2)

    # Handle options
    for opt, val in opts:
        if opt in ('-v', '--version'):
            print get_version(),
            sys.exit()
        elif opt in ('-h', '--help'):
            print usage(),
            print LONG_DESC,
            sys.exit()

    # Handle Arguments
    args_len = len(args)
    if args_len > 0:
        url = args[0]
    elif args_len == 0:
        print usage()
        print 'error: no URL provided'
        sys.exit()

    # Allow URLs without leading http(s)
    if not url.startswith('http://') and not url.startswith('https://'):
        url = 'http://%s' % url

    # Access the URL
    try:
        page = urllib.urlopen(url)
    except IOError, err:
        print usage()
        print 'Could not access URL "%s"' % url
        sys.exit(2)

    # Hold on to all matches
    matches = []
    # Loop through page and add each match to the matches list
    for match in re.finditer(RE_PHONE, page.read(), re.VERBOSE):
        matches.append(match.group(0))
    
    # Print each match found
    for match in matches:
        print match

if __name__ == '__main__':
    main()
