#!/bin/sh
# Nicolai Brand (lytix.dev) 2022

f="/tmp/valery_test_data"
heap="in use at exit: 0 bytes in 0 blocks"
failed=0

for test_vector in "ls -la" "echo a && echo b && echo c && echo d && echo e" "ls | wc -l" \
    "ls -la | grep . | grep . | wc -l" "echo \$PS1" "1 && 2 && 3 && 4 && 5 && 6 && 7 && 8 && 9 && 10"
do
    echo "VALERY TEST: '$test_vector' started."
    if ./valery -c "$test_vector" >/dev/null
    then
        echo "VALERY TEST: '$test_vector' COMPLETED."
    else
        echo "VALERY TEST: '$test_vector' FAILED." && failed=1
    fi

    valgrind -s --leak-check=full ./valery -c "$test_vector" 2> "$f" >/dev/null
    grep -q "$heap" "$f"

    [ $? -eq 1 ] && echo "VALERY TEST: '$test_vector' MEMORY LEAK DETECTED." && failed=1 && cat "$f"
done

exit $failed
