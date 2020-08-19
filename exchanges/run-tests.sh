#!/bin/sh
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
tests=(`find $DIR -name "*tests.so"`)
for t in "${tests[@]}"
do
echo -ne "$t/tests.so\t"
$DIR/baz/baz $t
done
