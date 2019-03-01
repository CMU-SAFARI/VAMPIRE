#!/usr/bin/env bash

# run_tests.sh

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License

echo "Starting tests"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Uncompress all tar files
echo "Uncompressing traces for tests"
find . -name '*.tar.gz' -exec tar xzvf {} -C "$(dirname {})" \; -exec rm {} \; | sed -e "s/^/    Extracting: /g"

exit_code=0
for f in "$DIR"/test_*.py; do
    echo -e "\nStarting: $f"
    "$f" | sed -e 's/^/    /'
    test_exit_code=${PIPESTATUS[0]}
    exit_code=$(( exit_code | test_exit_code ))

    echo "    exit code: $test_exit_code"
done

# Uncompress all tar files
echo -e "\nCompressing back traces"
find . -name '*.trace' -exec tar czvf {}.tar.gz {} \; -exec rm {} \; | sed -e "s/^/    Compressing: /g"

echo -e "\n\nAll tests completed, exiting with code $exit_code"
exit ${exit_code}