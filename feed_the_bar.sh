#!/usr/bin/env bash

endbar() {

  flashes=6
  while [ $flashes -gt 0 ]; do
    echo "0" > $CONTROL_PIPE
    sleep .05
    echo "100" > $CONTROL_PIPE
    sleep .05
    flashes=$((flashes - 1))
  done

  while pgrep loading-bar >/dev/null ; do
    echo "PIPE_END" > $CONTROL_PIPE
    echo "Still Running"
  done
  rm -f $CONTROL_PIPE
  # Run post ops
}

if [ -z ${CONTROL_PIPE+x} ] ; then
  CONTROL_PIPE="/tmp/load_pipe"
fi

iterations=100
load=0
while [ $iterations -gt 0 ]; do
  if [ $iterations -eq 90 ]; then
    echo "+INITIALIZING" > $CONTROL_PIPE
  elif [ $iterations -eq 60 ]; then
    echo "+ALTERING" > $CONTROL_PIPE
  elif [ $iterations -eq 30 ]; then
    echo "+FINISHING" > $CONTROL_PIPE
  fi
  echo "CONFIGURING ${load}" > $CONTROL_PIPE
  sleep .025
  if [ $iterations -gt 80 -o $iterations -lt 20 ] ;then
    echo "${load}" > $CONTROL_PIPE
  fi
  sleep .025
  if [ $iterations -gt 40 ] ;then
    echo "LOGGING ${iterations}"
  fi
  load=$((load + 1))
  iterations=$((iterations - 1))
done

echo "+yay" > $CONTROL_PIPE
#yay --noconfirm
echo ${load} > $CONTROL_PIPE
echo "INSTALLED" > $CONTROL_PIPE
echo "+REPLICANT" > $CONTROL_PIPE
echo "100" > $CONTROL_PIPE
endbar
