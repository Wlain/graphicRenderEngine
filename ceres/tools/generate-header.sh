#!/usr/bin/env bash

CURRENT_DIR=$(dirname $0)
cd $CURRENT_DIR/.. || exit

PROJECT_DIR=$(pwd)

HEADER_DIR=$PROJECT_DIR/include/ceres

rm -rf $HEADER_DIR
mkdir $HEADER_DIR

echo $HEADER_DIR

cd $HEADER_DIR

find $PROJECT_DIR/core $PROJECT_DIR/utils -type f | grep -E "(.*)\.h" | sed -E "s/(.*)\/(.*)/cp -f & \2/" | sh

exit 0



