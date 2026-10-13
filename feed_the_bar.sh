#!/usr/bin/env bash

pipe_file="/tmp/hi"

iterations=100
load=0
while [ $iterations -gt 0 ]; do
  if [ $iterations -eq 90 ]; then
    echo "+INITIALIZING" > $pipe_file
  elif [ $iterations -eq 60 ]; then
    echo "+ALTERING" > $pipe_file
  elif [ $iterations -eq 30 ]; then
    echo "+FINISHING" > $pipe_file
  fi
  echo "CONFIGURING ${load}" > $pipe_file
  sleep .01
  echo "${load}" > $pipe_file
  sleep .01
  load=$((load + 1))
  iterations=$((iterations - 1))
done

sleep 1
echo "100" > $pipe_file
