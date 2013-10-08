"""
Greatest Product

Find the greatest product of five consecutive digits in the 1000 digit number
in number.txt
"""
from operator import mul

# Take 20 lines of number strings, and combine them into a single number string.
number = "".join(open('number.txt', 'r').read().split('\n'))

"""
Find the greatest product of a sliding window of 5 ints across a 1000 digit number.
 
 - `for i ...` iterate over all the numbers, up to the 996th digit.
 - `max( ...` get the max from the list of products
 - `reduce(mul, ...` get the product of the list
 - `map(int, ...)` convert the list of strings to a list of ints.

Note: this is horribly implicit, and therefore not `pythonic` at all.
"""
def greatest_product():
    print max(reduce(mul, map(int, number[i:i+5])) for i in xrange(len(number)-4))
