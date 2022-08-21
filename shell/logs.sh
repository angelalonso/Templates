#!/usr/bin/env bash

function show_log {
  TSTAMP=$(date "+%Y-%m-%d %H:%M:%S")
  case $1 in
    "debug"|"d")
      echo "[$TSTAMP][DEBUG] - $2"
      ;;
    "info"|"i")
      echo "[$TSTAMP][INFO] - $2"
      ;;
    "warn"|"w")
      echo "[$TSTAMP][WARN] - $2"
      ;;
    "error"|"err"|"e")
      echo "[$TSTAMP][ERROR] - $2"
      ;;
    *)
      echo "[$TSTAMP][DEBUG] - Wrong Logging mode"
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
