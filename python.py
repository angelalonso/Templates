#/bin/env python
#
# First try at a python template
# based on: http://snipplr.com/view.php?codeview&id=13786

# Import everything needed 
import getopt
import sys
import os


def main():
  print "in"

# First method, an arguments checker
def argChecker(argv):
    # check arguments and options
    try:
        opts, args = getopt.getopt(argv, "hd:", ["help", "database"])
        # show usage printout if something is bad
        for opt, arg in opts:
            if opt in ("-h, --help"):
                printUsage()
                sys.exit(2)
            if opt in ("-d, --db"):
                print "Supposed to create db '" + arg +"'"
        main(args[0])
    except getopt.GetoptError:
        printUsage(True)
        sys.exit(2)
    except IndexError:
        printUsage(True)
        sys.exit(2)

def printUsage(error = False):
    if error:
        print "Unknown option or missing argument."
# TODO: scriptname.py name should be automatic.
    print """
    Usage: scriptname.py [options] <xml file>
        
    -h              show this help
    -d              use specific database
    """

if __name__ == "__main__":
    argChecker(sys.argv[1:])
