#!/usr/bin/env bash

echo "Starting tests"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

exit_code=0
for f in "$DIR"/test_*.py; do
    "$f" | sed -e 's/^/    /'
    test_exit_code=${PIPESTATUS[0]}
    exit_code=$(( $exit_code | $test_exit_code ))

    echo "Done[$test_exit_code]: $f"
done

echo "exiting with code $exit_code"
exit ${exit_code}