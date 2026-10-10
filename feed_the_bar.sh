#!/usr/bin/env bash
pid=$(pgrep loading-bar)
echo "${pid}"
seconds=3
load=10
while [ $seconds -gt 0 ]; do
  echo "hello ${seconds}" > ./load_pipe
  sleep 1
  echo "${load}" > ./load_pipe
  sleep 1
  seconds=$((seconds - 1))
  load=$((load + 20))
done
