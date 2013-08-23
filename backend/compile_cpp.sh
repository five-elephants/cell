#!/bin/sh

FRONTEND=./build/frontend
BACKEND_INCDIR=./backend

INTERMED_M4=`mktemp --tmpdir=/tmp --suff=.m4 compile.cpp.XXXXX`
INTERMED_CPP=`mktemp --tmpdir=/tmp --suff=.cpp compile_cpp.XXXXX`

$FRONTEND -g m4 -f $1 > $INTERMED_M4
m4 -I $BACKEND_INCDIR $INTERMED_M4 > $INTERMED_CPP
g++ -std=c++11 -c $INTERMED_CPP -o $2 || cat $INTERMED_M4 $INTERMED_CPP
rm $INTERMED_M4 $INTERMED_CPP
