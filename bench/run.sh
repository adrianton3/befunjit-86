#!/bin/bash

cd "$(dirname "$0")"

if [ ! -f "../test/spec/run-more/$1.bf" ]; then
  echo "Could not find '../test/spec/run-more/$1.bf'"
  exit 1
fi

cooldown=10


echo "jit"
sudo perf stat -r 100 ../cmake-build-release/b86 ../test/spec/run-more/$1.bf > /dev/null

echo "cooldown ${cooldown}s"
sleep $cooldown


echo "jit, --always-opt"
sudo perf stat -r 100 ../cmake-build-release/b86 --always-opt ../test/spec/run-more/$1.bf > /dev/null

echo "cooldown ${cooldown}s"
sleep $cooldown


echo "jit --never-opt"
sudo perf stat -r 100 ../cmake-build-release/b86 --never-opt ../test/spec/run-more/$1.bf > /dev/null

echo "cooldown ${cooldown}s"
sleep $cooldown


echo "interpreter"
sudo perf stat -r 100 ./interpreter/bfi ../test/spec/run-more/$1.bf > /dev/null