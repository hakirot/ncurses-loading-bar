#!/usr/bin/env bash

 
if [ -z ${CONTROL_PIPE+x} ] ; then
  CONTROL_PIPE="/tmp/load_pipe"
fi

iterations=100
load=0
sleep 2
while [ $iterations -gt 0 ]; do
  if [ $iterations -eq 90 ]; then
    echo "+INITIALIZING" > $CONTROL_PIPE
#   echo "OUTPUT 1"
  elif [ $iterations -eq 60 ]; then
    echo "+ALTERING" > $CONTROL_PIPE
    echo "HOTMAMAJAMMAHOTMAMAJAMMAHOTMAMAJAMMAHOTMAMAJAMMAHOTMAMAJAMMAHOTMAMAJAMMAHOTMAMAJAMMA"
#   echo "OUTPUT 2"
  elif [ $iterations -eq 30 ]; then
    echo "+FINISHING" > $CONTROL_PIPE
#   echo "OUTPUT 3"
  fi
  echo "CONFIGURING ${load}" > $CONTROL_PIPE
  sleep .025
  echo "${load}" > $CONTROL_PIPE
  sleep .025
  echo "LOGGING ${iterations} yayayayay"
  load=$((load + 1))
  iterations=$((iterations - 1))
done

echo "ITS WORKING ITS WORKING"
echo "ITS WORKING ITS WORKING"
sleep 1
echo "ITS WORKING ITS WORKING"
echo "ITS WORKING ITS WORKING"
sleep 1
yay --noconfirm
sleep 1
echo "100" > $CONTROL_PIPE
