#!/usr/bin/env bash

premake5 gmake2 && \
make -j4 && \
./bin/caerith