#! /usr/bin/env python
'''
Original Author: Trevor Bramwell (bramwelt@onid.orst.edu)
File: bramwelt_proj1_1.py

This file is my implementation of the first part
    of project one for CS311 (MWF 0800).
'''
import optparse
import sys
import os
import re

PROG = os.path.basename(sys.argv[0])
DESC = 'Look for searchText1, searchText2 in lines of filename '\
    'following the given flag.'
RE_WORD = r'\b%s\b'
RE_START = r'^\b%s\b'

# Offset is needed for printing the file line numbers, as line numbering
#  for humans starts at 1, not 0.
OFFSET = 1

# Default to print help when no arguments listed.
#   Simplest way is to override ArgumentParser error function.
#   Taken from SO question: http://stackoverflow.com/questions/4042452
class SearchyParser(optparse.OptionParser):
    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2) 

# Create a new command line argument parser
parser = SearchyParser(
    usage='%prog [options] searchText1 searchText2 filename',
    version='%prog 0.1',
    description=DESC,
)

# Mutually Exclusive Optional arguments
#   This group will throw an error when it recieves either
#   more than one or these arguments are passed, or none a
group = optparse.OptionGroup(parser, "Flags")
group.add_option('-a', action='store_true', default=False,
    help='Find all lines in filename that contains ' \
        'searchText1 AND searchText2')
group.add_option('-n', action='store_true', default=False,
    help='Find all lines in filename that contains ' \
        'searchText1 but NOT searchText2')
group.add_option('-s', action='store_true', default=False,
    help='Find all lines in filename that start ' \
        'with searchText1 or searchText2')
group.add_option('-t', action='store_true', default=False,
    help='Find all lines in filename that contains ' \
        'searchText1 and THEN searchText2')
parser.add_option_group(group)


def main():
    '''
    Main for Searchy
    '''
    # Get arguments from command line
    options, args = parser.parse_args()

    #print options, args

    args_len = len(args)
    if args_len < 3:
        parser.error('exactly 3 argument needed, %d given' %  args_len)
    searchText1 = args[0]
    searchText2 = args[1]

    # Select correct regular expression pattern depending
    #  on if we are checking at the start, or not
    
    if not (options.a or options.n or options.t or options.s):
        parser.error('please provide an argument')
    elif (options.a ^ options.n ^ options.t ^ options.s) == 0:
        parser.error('options -a, -n, -t, and -s are mutually exclusive')
    elif options.a or options.n or options.t:
        RE = RE_WORD
    else:
        RE = RE_START

    # Compile regular expression ignoring case
    word1 = re.compile(RE % searchText1, re.IGNORECASE)
    word2 = re.compile(RE % searchText2, re.IGNORECASE)

    # The number of matches in the file
    count = 0
    # Holds human readable version of flags (and, and not, then, starting with)
    help_txt = ''
    # List of lines that matched along with their line number
    output = []

    # Open the file in read mode, exit if file can't be read
    fileptr = None
    try:
        fileptr = open(args[2], 'r')
    except IOError, err:
        parser.error(str(err))

    # For each line in the file, check for searchText1 and searchText2
    #   Handle output depending on which argument the user passed
    for i, line in enumerate(fileptr):
        # Perform searches
        search1 = word1.search(line)
        search2 = word2.search(line)

        # Varible to handle if we output the line or not
        output_line = False

        if options.a:
            # AND
            help_txt = 'for %s and %s'
            if search1 and search2:
                output_line = True
        elif options.n:
            # NOT
            # Search returns None when it does not find any matches
            help_txt = 'for %s and not %s'
            if search1 and (search2 is None):
                output_line = True
        elif options.s:
            # Start
            help_txt = 'starting with %s or %s'
            if search1 or search2:
                output_line = True
        elif options.t:
            # THEN
            help_txt = 'for %s and then %s'
            if (search1 and search2):
                # Get the position of the last match. This is needed if we
                #   are going to know that the first preceeded the second.
                position1 = search1.end(0)
                position2 = search2.end(0)
                if position1 > position2:
                    output_line = True

        # If our statements have succeeded, append the line number and line
        #  to output
        if output_line:
            count += 1
            output.append('%d: %s' % (i+OFFSET,line),)

    # Substitue arguments into help_txt
    extra_txt = help_txt % (searchText1, searchText2)
    if count == 0:
        # No matches
        print 'No matches found %s' % extra_txt
    else:
        # Matches
        # Output count of total matches along with line, and line number
        print '%s matches found %s' % (count, extra_txt)
        for line in output:
            print line,


if __name__ == '__main__':
    main()
