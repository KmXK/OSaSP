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

	{
		echo "Wrong count of arguments. Correct count is 2."
		echo "Use -h to see help."
	} >&2

	exit 1
elif [ ! -d $1 ]
then
	{
		echo "First argument must be directory."
		echo "Use -h to see help."
	} >&2
	exit 1
fi

echo -n > "$2"

# Попытался сделать в одну команду. Получается слишком уродливо. Я что-то не так делаю?
# find $1 -type d -exec bash -c 'echo -e "$(find {} -maxdepth 1 -type f | wc -l)\t$(du -d 1 -bch {} | tail -1)\t$(realpath {})"' \;

for D in `find $1 -type d`
do
	full_path=$(realpath $D)
	full_size=$(du -d 1 -bch $D | tail -1 | awk '{ print $1 }')
	files_count=$(find $D -maxdepth 1 -type f | wc -l)

	echo -e "$files_count\t$full_size\t$full_path" >> "$2"
done
