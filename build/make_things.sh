#!/usr/bin/env bash

echo Making stl, this will take a while.

set -x
openscad -o ../things/right.stl right.scad
openscad -o ../things/bottom_right.stl bottom_right.scad
