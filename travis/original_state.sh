#!/bin/bash

bash -n $TRAVIS_BUILD_DIR/tools/plantuml/gen.sh
bash -n $TRAVIS_BUILD_DIR/tools/doxygen/extract.sh
bash -n $TRAVIS_BUILD_DIR/tools/swap_mr.sh
cd $TRAVIS_BUILD_DIR

make setup_env
make test
