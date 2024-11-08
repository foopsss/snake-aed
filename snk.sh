compile_program() {
	if [ "$target" = "linux" ]
	then
		gcc snake.c -o snake
		./snake
	elif [ "$target" = "w64" ]
	then
		x86_64-w64-mingw32-gcc snake.c -o snake.exe
		wine snake.exe
	elif [ "$target" = "w32" ]
	then
		i686-w64-mingw32-gcc snake.c -o snake-i686.exe
		wine snake-i686.exe
	else
		echo "Introduzca un target válido."
	fi
}

delete_files() {
	if [ "$target" = "linux" ]
	then
		rm snake
	elif [ "$target" = "w64" ]
	then
		rm snake.exe
	elif [ "$target" = "w32" ]
	then
		rm snake-i686.exe
	elif [ "$target" = "all" ]
	then
		if [ -e snake ]
		then
			rm snake
		fi

		if [ -e snake.exe ]
		then
			rm snake.exe
		fi

		if [ -e snake-i686.exe ]
		then
			rm snake-i686.exe
		fi
	else
		echo "Introduzca un target válido."
	fi
}

while getopts r:d: flag;
do
    case "${flag}" in
        r) target=$OPTARG; compile_program;;
        d) target=$OPTARG; delete_files;;
    esac
done