#!/usr/bin/env bash
set -vex

#########
# SETUP #
#########

# set up python venv
virtualenv test-venv
source test-venv/bin/activate
pip install numpy
