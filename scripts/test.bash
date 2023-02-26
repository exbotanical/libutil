#!/usr/bin/env bash

GREEN='\033[1;32m'
DEFAULT='\033[0m'

green () {
  printf "\033[1;32mRunning test $file_name...\n\n\033[0m"
}

run_test () {
	local file_name="$1"

	green "\n[+] Running test $file_name...\n\n"
	./test
	green "\n[+] Tests for $file_name passed\n\n"
}

run_test "$*"
