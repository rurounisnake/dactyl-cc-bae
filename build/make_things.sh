#!/usr/bin/env bash

echo Making stl, this will take a while.

set -x
openscad -o ../things/macro_bae.stl macro_bae.scad
openscad -o ../things/bottom_macro_bae.stl bottom_macro_bae.scad
