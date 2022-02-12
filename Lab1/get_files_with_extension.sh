#!/bin/bash
arg_count=$#

if [ "$arg_count" -ne "3" -o ! -d "$2" ]
then
	(
		echo "Invalid argument count! Correct count is 3."
		echo "First argument is file name."
		echo "Second argument is target directory."
		echo "Third argument is target file extension."
	) >&2
	exit 2
fi
find "$2" -name "*.$3" > "$1"
