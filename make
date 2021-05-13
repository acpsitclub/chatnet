#!/bin/sh
info="\033[0;32m[info]\033[0m"
err="\033[0;31m[err]\033[0m"


detect_compiler() {
	if echo $(which clang) | grep -qF "clang"; then
		CC="clang"
	elif echo $(which gcc) | grep -qF "gcc"; then
		CC="gcc"
	fi
	echo "$info Using compiler: $CC"
}


detect_os() {
	osfound=$(uname -o)
	case $osfound in
		"GNU/Linux")
			os="gnu"
			;;
		"Android")
			os="android"
			;;
		*)
			echo "$err OS not recognized"
			exit 1
	esac
	echo "$info Found OS: $os"
}


detect_machine(){
	m=$(uname -m)
	echo "$info Found machine: $m"
}


compile() {
	detect_compiler
	detect_os
	detect_machine
	

	$CC ./client4.c $components -o "client-$os-$m"
	#$CC ./server.c . -o "server-$os-$m"
}


program=$1
# With libcurl inside, it is no longer possible to have static binaries.
# because libcurl contains: getaddrinfo()
components="-Wall -Wextra -g ./libcurl.a ./libcjson.a -lssl -lcrypto -lpthread"
#components="-g -Wall -Wextra -lcurl "
main() {
	if [ "$program" != "" ]; then
		detect_compiler
		detect_os
		detect_machine

		lenpn=${#program}   #length of Program's name
		leni=$(($lenpn-2))  #len(".c")=2  #length index
		exen=$(echo $program | cut -c1-$leni)  #executable name
		exenfull="$exen-$os-$m"
		$CC $program $components -o $exenfull
	

		if echo $? | grep -qF "0"; then 
			if [ ! -d ./chatnet_role_change/ ]; then
				mkdir ./chatnet_role_change/
			fi
			cp ./$exenfull ./chatnet_role_change/
			echo "$info Program copied to ./chatnet_role_change\n";
			./$exenfull
		else echo "$err Program not compiled"
		fi
	else
		echo "$err Program name missig"
		exit
		compile
		
		if echo $? | grep -qF "0"; then 
			echo "$info Programs compiled."
		else 
			echo "$err Compilation failed."
		fi
	fi
}
main
