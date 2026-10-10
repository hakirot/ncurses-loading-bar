#!/usr/bin/env bash
pid=$(pgrep loading-bar)
echo "${pid}"
seconds=3
while [ $seconds -gt 0 ]; do
  sudo echo "hello " > /proc/${pid}/fd/0
  sleep 1
  seconds=$((seconds - 1))
done
