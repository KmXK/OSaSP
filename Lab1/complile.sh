#!/bin/bash
arg_count=$#

if [ "$arg_count" -ne 2 -o ! -f $1 ]
then
	{
		echo "Invalid arguments count! Correct count is 2."
		echo "First argument is source file name. "
		echo "Second argument is compiled file name."
	} >&2
	exit 1
fi

gcc $1 -o $2 && exec "./$2"
exit $?