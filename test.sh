##!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./build/9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

build() {
  pushd .
  mkdir -p build
  cd build
  cmake ..
  make
  popd
}

cd "$(dirname "$0")"
build
try 0 0
try 42 42
try 21 '5+20-4'

echo OK
