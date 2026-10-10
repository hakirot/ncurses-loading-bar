#!/usr/bin/env bash
pid=$(pgrep loading-bar)
echo "${pid}"
iterations=100
seconds=3
load=0
while [ $iterations -gt 0 ]; do
  echo "hello ${load}" > ./load_pipe
  sleep .05
  echo "${load}" > ./load_pipe
  sleep .05
  load=$((load + 1))
  iterations=$((iterations - 1))
done
