#! /usr/bin/env bash

set -e

function compile() {
	echo "Kompilowanie programu: $1"
	gcc -std=c89 -Wall -Wextra -pedantic -o bin/$1 src/$1.c
}

function clean() {
	if [ -d bin ]; then
		for i in bin/*
		do
			if [ -x $i ]; then
				echo "Removing: $i"
				rm $i
			fi
		done
	fi
}

clean
compile kierownik
compile mechanik
compile obslugaKlienta
compile kolejka
compile kasjer
