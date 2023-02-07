#!/bin/bash

echo "Running unit tests:"

if valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=tests/tests.log ./tests/all_tests
then
	echo "$i PASS"
else
	echo "ERROR in test $i: here's tests/tests.log"
	echo "------"
	tail tests/tests.log
	exit 1
fi

echo ""
echo -e "Memory result:\n"
tail tests/tests.log

echo -e "======================================================================================="

echo -e "\nDISREGARD COMPILER WARNINGS, EVERY ONE OF THEM IS FOR PURPOSE.\n"
