#!/usr/bin/env bash

LED_PIN=21

function blink_x_times {
  for j in $( eval echo {1..$1} )
  do
    echo "1" > /sys/class/gpio/gpio$LED_PIN/value
    sleep 0.2
    echo "0" > /sys/class/gpio/gpio$LED_PIN/value
    sleep 0.2
  done
  echo "----"

}

if [ -d /sys/class/gpio/gpio$LED_PIN/ ]; then
  echo "$LED_PIN" > /sys/class/gpio/export  
  echo "out" > /sys/class/gpio/gpio$LED_PIN/direction

  blink_x_times $1

  echo "0" > /sys/class/gpio/gpio$LED_PIN/value
else
  echo "ERROR! GPIO $LED_PIN NOT FOUND!"
fi
