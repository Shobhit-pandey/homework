"""
Alphabet Soup

Find a word's alphabetical value, multiply it by it's position in a
list, and return the sum of all these alphabet name scores.
"""

class AlphabetFun(object):
    """
    Collection of methods related to #3 of HW1.
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
    def alpha_total(self, names):
        """
        The total score of a list of names.
        """
        return sum(self.alpha_position(name, names) * self.alpha_value(name) for name in names)

print AlphabetFun().alpha_total(AlphabetFun.ns)
