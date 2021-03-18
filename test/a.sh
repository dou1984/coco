#!/bin/bash

pushd ../libco

make clean && make -j

popd

make clean && make -j
