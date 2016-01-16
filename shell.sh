#!/bin/bash
set -eo pipefail
#
# Brief description of what the script does


# Path to programs used within the script - Alphabetical order
AWK=$(which awk)
SED=$(which sed)


# Some other variables used through the whole script, in CAPS
VAR_1="variable 1"
VAR_2="variable 2"



# Standard Show help function
#
help(){
  echo "ERROR: Wrong or unrecognized parameter received: $1"
  echo "USAGE:"
  echo "$0 [option_1|option_n]"
}



# Document what this function does
#
option_1(){
#Document what is being done next
echo "Hello world"

#Document what is being done next
echo "Hello world"
}



# Document what this function does
#
option_n(){
#Document what is being done next
echo "Hello world"

#Document what is being done next
echo "Hello world"
}



# Document what parameters are controlled within this Main function
#
main(){
case "$1" in
  option_1)
    option_1;;
  option_n)
    option_n;;
  *)
    help "$1";;
esac
}



main "$1"

