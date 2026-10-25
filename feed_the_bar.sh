#!/usr/bin/env bash

# TODO implement these
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RESET='\033[0m'

if [ -z ${CONTROL_PIPE+x} ] ; then
  CONTROL_PIPE="/tmp/load_pipe"
fi

send_pipe () {
  echo $1 > $CONTROL_PIPE
}

endbar() {

  flashes=9
  while [ $flashes -gt 0 ]; do
    send_pipe "0"
    sleep .05
    send_pipe "100"
    sleep .05
    flashes=$((flashes - 1))
  done
  sleep .5
  while pgrep loading-bar >/dev/null ; do
    send_pipe "PIPE_END"
    echo "Still Running"
  done
  rm -f $CONTROL_PIPE
  # Run post ops
}

# other config commands

iterations=100
load=0
#while [ $iterations -gt 0 ]; do
#  if [ $iterations -eq 90 ]; then
#    send_pipe "+INITIALIZING"
#  elif [ $iterations -eq 60 ]; then
#    send_pipe "+ALTERING"
#  elif [ $iterations -eq 30 ]; then
#    send_pipe "+FINISHING"
#  fi
#  send_pipe "CONFIGURING ${load}"
#  sleep .025
#  if [ $iterations -gt 80 -o $iterations -lt 20 ] ; then
#    send_pipe "${load}"
#  elif [ $iterations -gt 60 ] ;then
#    echo "SPECIAL SAUCE"
#  else
#    send_pipe "50"
#  fi
#  sleep .025
#  if [ $iterations -gt 40 ] ;then
#    echo -e "${GREEN}LOGGING ${iterations}${RESET}"
#  elif [ $iterations -eq 39 ] ;then
#    echo -e "LOGGING ${iterations}"
##   send_pipe "-CRIT critted!"
#  fi
#  load=$((load + 1))
#  iterations=$((iterations - 1))
#done

send_pipe "+yay"
#yay --noconfirm
send_pipe ${load}
send_pipe "INSTALLED"
send_pipe "+REPLICANT"
send_pipe "100"
endbar
