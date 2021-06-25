#!/bin/sh

if test $# -eq 1 #./myfind.sh 模式$1
then
	grep -r -Hn --include=*.c --include=*.h -e $1 $PWD > result 2> error
	if test -s result; then #有匹配项
		cat result
	else #无匹配项
		if test -s error; then  #路径不存在
			cat error
		else #路径存在
			echo "NOT FOUND: PATTERN='$1' in PATH='$PWD'."
		fi
	fi
elif test $# -eq 2; then  #./myfind.sh 路径$1 模式$2
	grep -r -Hn --include=*.c --include=*.h -e $2 $1 > result 2> error #重定向标准输出、标准错误
	if test -s result; then  #有匹配项
		cat result
	else #无匹配项
		if test -s error; then  #路径不存在
			cat error
		else #路径存在
			echo "NOT FOUND: PATTERN='$2' in PATH='$1'."
		fi
	fi
else
	echo "Please check your syntax..."
	echo "Usage 1: $0 PATH PATTERN"
	echo "Example: $0 /usr hello"
	echo "Usage 2: $0 PATTERN"
	echo "Example: $0 hello"
fi