#!/bin/sh

name="symbolbrowser"

qmake && make && cp lib${name}.so ~/.config/juff/plugins/lib${name}.so && juffed