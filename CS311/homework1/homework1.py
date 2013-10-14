"""
CS311 - Homework 1

This file holds the collected functions for questions 1-4 of homework
1 for CS311-FA13.

:author: Trevor Bramwell
"""
import os
import sys

from operator import mul
from os import path
from getopt import getopt, GetoptError


class Options(object):
    """
    Options class holds a collection of settings
    """
    verbose = False
    course = None
    term = None


class CreateDirs(object):
    """
    Question 1
    """

    def create_dirs(self, term, course):
        """
        Creates a directory `./<term>/<course>/` that contains the
        subdirectories: 
            assignment, examples, exams, lecture_notes, submissions 
        and the symlinks:
            handin -> /usr/local/classes/eecs/<term>/<course>/handin
            website -> /usr/local/classes/eecs/<term>/<course>/public_html

        :arg term: The term of the school year
        :arg course: Name of a course. Generally in the form:
                     [A-Z]{2}[0-9]{3}
        """
        # Paths for <term>/<course>/ and symlink source.
        course_path = path.join(os.getcwd(), term, course)
        # symlink paths
        symlink_base_path = path.join(os.sep, 'usr', 'local', 'classes', 'eecs')
        symlink_source_path = path.join(symlink_base_path, term, course)
        handin_path = path.join(symlink_source_path, 'handin')
        website_path = path.join(symlink_source_path, 'website')
        # Directories to create for a course
        course_dirs = ['assignments', 'examples', 'exams', 'lecture_notes',
                       'submissions']
        course_flags = os.F_OK | os.W_OK
        
        def created_dir(directory):
            print "Created directory: {0}".format(directory)

        # Ensure the directory doesn't exist before creating it.
        if not path.exists(course_path):
            os.makedirs(course_path)
            if Options.verbose:
                created_dir(course_path)
        elif not path.isdir(course_path):
            raise os.error("Course path '{0}' exists, "
                        "but is not a directory".format(course_path))
        elif not os.access(course_path, course_flags):
            raise os.error("You do not have permissions to create "
                        "directories in: {0}".format(course_path))
        else:
            if Options.verbose:
                print "Directory: {0} already exists.".format(course_path)

        if path.exists(course_path):
            os.chdir(course_path)
            if Options.verbose:
                print "Decending into: {0}".format(course_path)
            for new_dir in course_dirs:
                if not path.exists(new_dir):
                    os.mkdir(new_dir)
                    if Options.verbose:
                        created_dir(new_dir)
                elif Options.verbose:
                    print "Directory: {0} already exists.".format(new_dir)
            if path.exists(symlink_base_path):
                h = path.join(course_path, 'handin')
                w = path.join(course_path, 'website')
                if path.exists(h) and not path.exists(handin_path):
                    os.symlink(handin_path, 'handin')
                elif path.exists(w) and not path.exists(website_path):
                    os.symlink(website_path, 'website')
                elif Options.verbose:
                        print "Symlinks already exists."
                if Options.verbose:
                    print "Created symlinks: handin, website."
            elif not os.access(symlink_base_path, os.W_OK):
                raise os.error("Can't create symlinks 'handin' or "
                            "'website'. You do not have access to: "
                            "{0}".format(symlink_base_path))
            else:
                raise os.error("Not symlinking 'handing' or 'website' as "
                            "you don't seem to be on os-class. "
                            "Path does not exist: "
                            "{0}".format(symlink_base_path))

        if Options.verbose:
            print ("Finished creating directories for "
                   "{0}/{1}".format(term, course))


class AlphabetFun(object):
    """
    Question 3

    Using names.txt, a 46K text file containing over five-thousand first
    names, begin by sorting it into alphabetical order. Then working out
    the alphabetical value for each name, multiply this value by its
    alphabetical position in the list to obtain a name score.

    For example, when the list is sorted into alphabetical order, COLIN,
    which is worth 3 + 15 + 12 + 9 + 14 = 53, is the 938th name in the
    list. So, COLIN would obtain a score of 938 x 53 = 49714.

    What is the total of all the name scores in the file?

    See the posted examples for how to work with a file. For this,
    A = 1, B = 2, etc. 
    """
    ns = open('names.txt', 'r').read().strip('"').split('","')
    ns.sort()

    @staticmethod
    def alpha_position(name, names):
        """
        1-offset position in name in a list of names
        """
        return names.index(name)+1


    @staticmethod
    def alpha_value(name):
        """
        Convert a name to its alphabetical value.

        Example COLIN would convert to 53, because::
        
            COLIN = 3 + 15 + 12 + 9 + 14 = 53

        :param name: A name to be converted.
        :type str:
        """
        return sum(ord(i)-ord('@') for i in name)

    @classmethod
    def alpha_total(self):
        """
        The total score of a list of names.
        """
        names = self.ns
        return sum(self.alpha_position(name, names) * self.alpha_value(name) for name in names)


class GreatestProduct(object):
    """
    Question 2

    Finds the greatest product of five consecutive digits in the
    1000-digit number:

          73167176531330624919225119674426574742355349194934
          96983520312774506326239578318016984801869478851843
          85861560789112949495459501737958331952853208805511
          12540698747158523863050715693290963295227443043557
          66896648950445244523161731856403098711121722383113
          62229893423380308135336276614282806444486645238749
          30358907296290491560440772390713810515859307960866
          70172427121883998797908792274921901699720888093776
          65727333001053367881220235421809751254540594752243
          52584907711670556013604839586446706324415722155397
          53697817977846174064955149290862569321978468622482
          83972241375657056057490261407972968652414535100474
          82166370484403199890008895243450658541227588666881
          16427171479924442928230863465674813919123162824586
          17866458359124566529476545682848912883142607690042
          24219022671055626321111109370544217506941658960408
          07198403850962455444362981230987879927244284909188
          84580156166097919133875499200524063689912560717606
          05886116467109405077541002256983155200055935729725
          71636269561882670428252483600823257530420752963450
    """
    number = "".join(open('number.txt', 'r').read().split('\n'))

    @classmethod
    def greatest_product(self):
        """
        Find the greatest product of a sliding window of 5 ints across a
        1000 digit number.
         
         - `for i ...` iterate over all the numbers, up to the 996th digit.
         - `max( ...` get the max from the list of products
         - `reduce(mul, ...` get the product of the list
         - `map(int, ...)` convert the list of strings to a list of ints.

        Note: this is horribly implicit, and therefore not `pythonic` at all.
        """
        number = self.number
        return max(reduce(mul, map(int, number[i:i+5])) for i in xrange(len(number)-4))

class TriangleNumbers(object):
    """
    Question 4

    The nth term of the sequence of triangle numbers is given by:

        tn = (1/2)n (n+1)

    so the first ten triangle numbers are:

        1, 3, 6, 10, 15, 21, 28, 36, 45, 55, ...

    By converting each letter in a word to a number corresponding to its
    alphabetical position and adding these values we form a word value.
    For example, the word value for SKY is 19 + 11 + 25 = 55 = t10. If
    the word value is a triangle number then we shall call the word a
    triangle word.

    Using words.txt, a 16K text file containing nearly two-thousand
    common English words, how many are triangle words? 
    """
    #words = open('words.txt', 'r')


class Main(object):
    """
    Holds answers/functions for all the questions.
    """

    def exit_with_help(self):
        print self.usage()
        sys.exit()

    def run(self):
        """
        Parses the arguments and options passed to the script.
        """
        try:
            opts, args = getopt(sys.argv[1:], 'c:t:hv', ['--course', '--term'])
        except GetoptError as err:
            print err
            sys.exit(2)
            #self.error("Option not recognized: {0}".format(opt))

        #if len(opts) == 0:
        #    self.error("Please pass in at least one option.")

        self.parse_opts(opts)
    
        self.parse_args(args)
    
        if Options.question == 1:
            if Options.term and Options.course:
                self.question1(Options.term, Options.course)       
                sys.exit()
            elif Options.term or Options.course:
                self.error("Provided term or course by itself.")
        elif Options.question == 2:
            self.question2()
        elif Options.question == 3:
            self.question3()
        elif Options.question > 3:
            self.error("Argument references a question that does not"
                       " exists.")
        else:
            self.exit_with_help()

    def parse_opts(self, opts):
        """
        Parse the options sent to the program.
        """
        for opt, arg in opts:
            if opt in ('-v'):
                Options.verbose = True
            elif opt in ('-h'):
                self.exit_with_help()
            elif opt in ('-c', '--course'):
                Options.course = arg
            elif opt in ('-t', '--term'):
                Options.term = arg

    def parse_args(self, args):
        """
        Parse the arguments passed to the program.
        """
        if len(args) == 0:
            Options.question = 1
        elif len(args) > 1:
            self.error("Too many arguments given")
        else:
            if Options.verbose:
                print "Arguments: {0}".format(args)
            try:
                Options.question = int(args[0])
            except ValueError as err:
                print "Argument passed is not a valid integer."
                sys.exit(2)

    def usage(self):
        header = ('Usage: {0} [OPTIONS] [QUESTION]\n\nRun question QUESTION '
                  'for assignment 1, where QUESTION is the question '
                  'number. Defaults to 1.\nExample:\n  {0} 2\nWould run '
                  'question 2 ' 'with out any arguments.\n\nMandatory '
                  'arguments are mandatory.'.format(__file__))
        arguments = [
            ('-h', '', 'Show this text'),
            ('-v', '', 'Enable verbose output'),
            ('-c,', '--course', 'The course being taught '
                               '(requires: -t, or --term)'),
            ('-t,', '--term', 'The term of courses '
                             '(requires: -c, or --course'),
        ]
        
        usage = [header]
        usage.extend(["  {0} {1}\t{2}".format(a1, a2, desc) for
            a1,a2,desc in arguments])
        return "\n".join(usage)

    def error(self, error_msg):
        print "ERROR:",error_msg
        sys.exit(1)

    def question1(self, term, course):
        CreateDirs().create_dirs(term, course)

    def question2(self):
        print GreatestProduct().greatest_product()

    def question3(self):
        print AlphabetFun().alpha_total()

    def question4(self):
        print TriangleNumbers().triangle_word_total()


if __name__ == "__main__":
    #Main().question1('fa13','cs421')
    #Main().question2()
    #Main().question3()
    Main().run()
    #Main().greatest_product()
