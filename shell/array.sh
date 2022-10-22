#!/usr/bin/env bash

functions=(
one 
two 
three
four
)

function one {
  echo "UNO"
}

function two {
  echo "DOS"
}

function three {
  echo "TRES"
}

function four {
  echo "CUATRO"
}

# We make a countdown and launch each function on the array
functions_length=${#functions[@]}
for (( ix=0; ix<${functions_length}; ix++ ));
do
  echo "$(($functions_length-$ix)) functions left to run"
  echo "running function ${functions[$ix]}"
  ${functions[$ix]}
  echo "-----"
done
