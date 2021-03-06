#!/bin/bash
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
try 0 '0;'
try 42 '42;'
try 21 '5+20-4;'
try 41 " 12 + 34 - 5 ;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 15 '+3*+5;'
try 1 '1<2;'
try 1 '1<=2;'
try 0 '1>2;'
try 0 '1>=2;'
try 1 '2<1+2;'
try 2 'a=2;a;'
try 6 'a=2;b=3;a*b;'
try 3 'a=1;b=2;return a+b;a;'
try 3 'foo = 1; bar = foo + 1;return foo + bar ;'
try 2 'a=1; if (a==1) return 2; else return 3;'
try 3 'a=1; if (a==2) return 2; else return 3;'
try 3 'a=1; if (a==2) return 2; return 3;'
try 20 'a=0;for(i=0;i<10;i=i+1) a = a+2; return a;'
try 10 'a=0;for(;a<10;a=a+1); return a;'
try 10 'a=0;while(a<10)a=a+1;return a;'
try 1 'a=0; if (a<2) { a = a+1; return a;} return a;'

echo OK
