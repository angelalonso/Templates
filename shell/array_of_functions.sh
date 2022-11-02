#!/usr/bin/env bash

functions=(
one 
two 
three
four
)

function one {
  echo "UNO $1"
}

function two {
  echo "DOS $1"
}

function three {
  echo "TRES $1"
}

function four {
  echo "CUATRO $1"
}

# We make a countdown and launch each function on the array
functions_length=${#functions[@]}
for (( ix=0; ix<${functions_length}; ix++ ));
do
  echo "$(($functions_length-$ix)) functions left to run"
  echo "running function ${functions[$ix]}"
  ${functions[$ix]} `expr $functions_length - $ix`
  echo "-----"
done
