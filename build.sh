#! /usr/bin/env bash
function compile() {
	gcc -std=c89 -Wall -Wextra -pedantic -o bin/$1 src/$1.c
}

compile kierownik
