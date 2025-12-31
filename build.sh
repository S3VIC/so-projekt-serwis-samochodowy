#! /usr/bin/env bash

set -e

function compile() {
	echo "Kompilowanie programu: $1"
	gcc -std=c89 -Wall -Wextra -pedantic -o bin/$1 src/$1.c
}

compile kierownik
compile serwis
