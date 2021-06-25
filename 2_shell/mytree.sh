#!/bin/sh

if test $# -eq 0; then
	find . -print 2> mytree_errMsg | sort | awk -F/ '!/\.$/ { for (i=2;i<NF;i++){d=4;printf("%"d"s","") } print $i}'
elif test -s errMsg;then
	cat errMsg
else
	echo "Please check your syntax..."
	echo "Usage: $0"
	echo "Note: no parameters needed."
	echo "Function: List current directory's Tree-Structured Directory."
fi
