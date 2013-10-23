#! /usr/bin/env python
"""
Interface Tests for ar

Ensure the interface is correctly defined for all arguments, and each
operation executes as expected. These tests are very black box; so much
so that all that is required is a path to the ar binary to check.
"""
import unittest

from subprocess import Popen, PIPE
from unittest import TestCase

AR = '/usr/bin/ar'


class TestFlagMixin(object):
    """
    Mixin class for testing flags on the binary.
    """
    preflag = ''
    arfile = 'test_archive'

    def get_output(self):
        process = Popen([AR, self.preflag, self.arfile],
                        stdout=PIPE).communicate()[0]
        return process.rstrip('\n').split('\n')


class tFlagTestCase(TestFlagMixin, TestCase):
    """
    Ensures '-t' flag works as intended
    """
    preflag = '-t'

    def test_empty_archive(self):
        """
        Given an empty archive, ensure nothing is listed with the '-t'
        flag.
        """
        self.arfile = 'empty_archive'
        expected_files = ['']

        file_list = self.get_output()
        self.assertEqual(file_list, expected_files)

    def test_single_file(self):
        """
        Given a archive with a single file, ensure only that file is
        listed.
        """
        self.arfile = 'test_archive'
        expected_files = ['Makefile']

        file_list = self.get_output()
        self.assertEqual(file_list, expected_files)


if __name__ == "__main__":
    unittest.main()
