#!/bin/bash

echo "Files with potentially dangerous functions"

for file in "$@"
do
    if test -f $file
    then
        echo $file
        if egrep "[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)" $file
        then
            echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
        else
            echo "  ---NONE---"
        fi
        echo "-------------------------------"
    fi
done
