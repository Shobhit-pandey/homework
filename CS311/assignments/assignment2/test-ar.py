#! /usr/bin/env python
"""
Interface Tests for ar

Ensure the interface is correctly defined for all arguments, and each
operation executes as expected. These tests are very black box; so much
so that all that is required is a path to the ar binary to check.
"""
import sys


class TestFlagMixin(object):
    """
    Mixin class for testing flags on the binary.
    """


def run_suite(ar_bin):
    print ar_bin
    pass

if __name__ == "__main__":
    if (len(sys.argv) != 2):
        print("Usage: {0} <ar-binary>".format(sys.argv[0]))
        sys.exit(1)

    run_suite(sys.argv[1])
