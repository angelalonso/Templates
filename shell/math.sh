#!/usr/bin/env bash
set -eo pipefail
#
# Brief description of what the script does


IX=0

for i in  {0..10}; do
  IX=$((IX+1))
  echo $IX
done


