#!/usr/bin/env bash

echo "Starting tests"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Uncompress all tar files
echo "Uncompressing traces for tests"
SRC=$DIR
find . -name '*.tar.gz' -exec tar xzvf {} -C $(dirname {}) \; -exec rm {} \; | sed -e "s/^/    Extracting: /g"

exit_code=0
for f in "$DIR"/test_*.py; do
    echo -e "\nStarting: $f"
    "$f" | sed -e 's/^/    /'
    test_exit_code=${PIPESTATUS[0]}
    exit_code=$(( $exit_code | $test_exit_code ))

    echo "    exit code: $test_exit_code"
done

echo -e "\n\nAll tests completed, exiting with code $exit_code"
exit ${exit_code}