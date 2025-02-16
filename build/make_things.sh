#!/usr/bin/env bash

echo Making stl, this will take a while.

set -x
openscad -o ../things/left.stl left.scad
openscad -o ../things/bottom_left.stl bottom_left.scad
