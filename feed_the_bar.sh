#!/usr/bin/env bash

endbar() {
  while pgrep loading-bar ; do
    sleep 1
    echo "PIPE_END" > $CONTROL_PIPE
    echo "Still Running"
  done
  rm -f $CONTROL_PIPE
}
 
if [ -z ${CONTROL_PIPE+x} ] ; then
  CONTROL_PIPE="/tmp/load_pipe"
fi

iterations=100
load=0
#sleep 1
while [ $iterations -gt 0 ]; do
  if [ $iterations -eq 90 ]; then
    echo "+INITIALIZING" > $CONTROL_PIPE
  elif [ $iterations -eq 60 ]; then
    echo "+ALTERING" > $CONTROL_PIPE
    echo "WAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKAWAKA"
  elif [ $iterations -eq 30 ]; then
    echo "+FINISHING" > $CONTROL_PIPE
  fi
  echo "CONFIGURING ${load}" > $CONTROL_PIPE
  sleep .025
  echo "${load}" > $CONTROL_PIPE
  sleep .025
  echo "LOGGING ${iterations}"
  load=$((load + 1))
  iterations=$((iterations - 1))
done

#echo "last msg"
echo "+yay" > $CONTROL_PIPE
echo "Updating System" > $CONTROL_PIPE
echo "Updating System" > $CONTROL_PIPE
echo "Updating System" > $CONTROL_PIPE
#yay --noconfirm
sleep .25
echo 100 > $CONTROL_PIPE
sleep 1
endbar
