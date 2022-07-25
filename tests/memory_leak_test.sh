#!/bin/sh
# Nicolai Brand (lytix.dev) 2022

f="/tmp/valery_test_data"
heap="in use at exit: 0 bytes in 0 blocks"

valgrind ./valery_test 2> $f >/dev/null
grep -q "$heap" "$f"
rc=$?

[ $rc -ne 1 ] && echo "VALERY TEST SUCCESS" && exit 0
[ $rc -eq 1 ] && echo "VALERY TEST FAILED: Memory leak detected" && exit 1
