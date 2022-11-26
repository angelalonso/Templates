#!/usr/bin/env bash
for i in 0 2; do
  $(exit $i)
  if [[ $? == 0 ]]; then
    printf '\e[32m%s\n\e[0m' "OK"
    printf "\x1b[38;5;2mOK\n"
  else
    printf '\e[31m%s\n\e[0m' "ERROR"
    printf "\x1b[38;5;1mERROR\n"
  fi
done

