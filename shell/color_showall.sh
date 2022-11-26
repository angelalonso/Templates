#!/usr/bin/env bash
for i in {0..255} ; do
  printf "\x1b[38;5;${i}mcolour${i}\n"
done
for i in {0..255} ; do
  color=$((30+$i))
  printf '\e[${color}m%s\n\e[0m' "colour${i}"
done
