#!/bin/bash

cd "$(dirname "$0")"

g++ ./main.cpp -o bfi -std=c++20 -O3 -Wall -Wextra