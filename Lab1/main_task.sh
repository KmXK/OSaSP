#!/bin/bash

if [ $# -ne 2 ]
then
	if [ $# -eq 1 -a "$1" == "-h" ]
	then
		echo "Using: main_task.s [directory] [filename]"
		echo "First argument is target directory."
		echo "Second argument is output file name."
		exit 0
	fi

	(
		echo "Wrong count of arguments. Correct count is 2."
		echo "Use -h to see help."
	) >&2

	exit 1
elif [ ! -d $1 ]
then
	(
		echo "First argument must be directory."
		echo "Use -h to see help."
	) >&2
	exit 1
fi

for D in `find $1 -type d`
do
	full_path=$(realpath $D)
	full_size=$(du $D -bh -d 0 | awk '{print $1}')
	files_count=$(ls $D | wc | awk '{print $1}')

	echo -e "$files_count\t\t$full_size\t\t$full_path" >> "$2"
done
