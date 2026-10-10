#!/usr/bin/env bash
pid=$(pgrep loading-bar)
echo "${pid}"
iterations=100
seconds=3
load=0
while [ $iterations -gt 0 ]; do
  echo "CONFIGURING ${load}" > ./load_pipe
  sleep .01
  echo "${load}" > ./load_pipe
  sleep .01
  load=$((load + 1))
  iterations=$((iterations - 1))
done
