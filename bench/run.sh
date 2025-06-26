#!/bin/bash

cd "$(dirname "$0")"

if [ ! -f "../test/spec/run-more/$1.bf" ]; then
  echo "Could not find '../test/spec/run-more/$1.bf'"
  exit 1
fi

echo "jit, with optimizations"
sudo perf stat -r 100 ../cmake-build-release/b86 ../test/spec/run-more/$1.bf > /dev/null

echo "cooldown 2s"
sleep 2

echo "jit without optimizations"
sudo perf stat -r 100 ../cmake-build-release/b86 --no-opt ../test/spec/run-more/$1.bf > /dev/null

echo "cooldown 2s"
sleep 2

echo "interpreter"
sudo perf stat -r 100 ./interpreter/bfi ../test/spec/run-more/$1.bf > /dev/null