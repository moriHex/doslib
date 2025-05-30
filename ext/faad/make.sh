#!/bin/bash
rel=../..
if [ x"$TOP" == x ]; then TOP=`pwd`/$rel; fi
. $rel/linux-ow.sh

if [ "$1" == "clean" ]; then
	do_clean
	rm -fv test.dsk test2.dsk nul.err tmp.cmd tmp1.cmd tmp2.cmd
	exit 0
fi

if [ "$1" == "disk" ]; then
	make_msdos_data_disk test.dsk || exit 1
	mcopy -i test.dsk dos386f/test.exe ::test386.exe
	mcopy -i test.dsk dos486f/test.exe ::test486.exe
	mcopy -i test.dsk dos586f/test.exe ::test586.exe
	mcopy -i test.dsk dos686f/test.exe ::test686.exe
	mcopy -i test.dsk dos386f/dos4gw.exe ::dos4gw.exe
fi

if [[ "$1" == "build" || "$1" == "" ]]; then
	make_buildlist
	begin_bat

	what=all
	if [ x"$2" != x ]; then what="$2"; fi
    if [ x"$3" != x ]; then build_list="$3"; fi

	for name in $build_list; do
		do_wmake $name "$what" || exit 1
		bat_wmake $name "$what" || exit 1
	done

	end_bat
fi

