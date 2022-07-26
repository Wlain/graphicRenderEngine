#!/usr/bin/env bash

THIS_DIR=$(dirname $0)
cd $THIS_DIR/.. || exit
PROJECT_DIR=$(pwd)

if ! [ -x "$(command -v clang-format)" ]; then
    echo 'clang-format is not installed. run "brew install clang-format" manully' >&2
    #brew install clang-format
    exit 1
fi
CF_VER=$(clang-format --version | sed -E "s/([^0-9]*)([0-9]+)\.(.*)/\2/")
if (($CF_VER < 11)); then
    echo '[ERROR] clang-format version at least 11, please run [brew update && brew upgrade clang-format]'
    exit 1
fi

declare -a CHANGED_FILES=()

while IFS=$'\n' read -r -a line; do
    echo "Find cached file: $line"
    CHANGED_FILES+=("$line")
done < <(git diff --cached --name-only | grep -E '(.*)\.(h|hpp|cpp|mm)$')

while IFS=$'\n' read -r -a line; do
    echo "Find changed file: $line"
    CHANGED_FILES+=("$line")
done < <(git diff --name-only | grep -E '(.*)\.(h|hpp|cpp|mm)$')

declare -a UNIQUE_CHANGED_FILES=()

if [[ "${#CHANGED_FILES[@]}" -ne 0 ]]; then
    while IFS=$'\n' read -r -a line; do
        UNIQUE_CHANGED_FILES+=("$line")
        echo "Processing file $line"
        clang-format -i --style=file $line
    done < <(printf "%s\n" ${CHANGED_FILES[@]} | sort | uniq)
fi

echo "Total changed files: ${#UNIQUE_CHANGED_FILES[@]}"
