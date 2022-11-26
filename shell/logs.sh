#!/usr/bin/env bash

function show_log {
  TSTAMP=$(date "+%Y-%m-%d %H:%M:%S")
  case $1 in
    "debug"|"d")
      #echo "[$TSTAMP][DEBUG] - $2"
      printf '\e[35m%s\n\e[0m' "[$TSTAMP][DEBUG] - $2"
      ;;
    "info"|"i")
      #echo "[$TSTAMP][INFO] - $2"
      printf '\e[32m%s\n\e[0m' "[$TSTAMP][INFO] - $2"
      ;;
    "warn"|"w")
      #echo "[$TSTAMP][WARN] - $2"
      printf '\e[33m%s\n\e[0m' "[$TSTAMP][WARN] - $2"
      ;;
    "error"|"err"|"e")
      #echo "[$TSTAMP][ERROR] - $2"
      printf '\e[31m%s\n\e[0m' "[$TSTAMP][ERROR] - $2"
      ;;
    *)
      #echo "[$TSTAMP][DEBUG] - Wrong Logging mode"
      printf '\e[35m%s\n\e[0m' "[$TSTAMP][DEBUG] - $2"
      exit 2
      ;;
  esac
}


show_log debug "test one, two, three"
show_log info "test one, two, three"
show_log warn "test one, two, three"
show_log error "test one, two, three"
show_log err "test one, two, three"
show_log d "test one, two, three"
show_log i "test one, two, three"
show_log w "test one, two, three"
show_log e "test one, two, three"
show_log wrong "test one, two, three"
