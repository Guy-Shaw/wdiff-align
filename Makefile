# Filename: src/Makefile
# Project: wdiff-align
# Brief: Top-level makefile for wdiff-align libraries and command
#
# Copyright (C) 2016 Guy Shaw
# Written by Guy Shaw <gshaw@acm.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

.PHONY: all test clean .FORCE

CMD_NAME := wdiff-align

all: cmd/$(CMD_NAME)

cmd/$(CMD_NAME): .FORCE
	cd libcscript && make
	cd cmd        && make

test: cmd/test
	cd cmd/test && make test

clean:
	cd libcscript && make clean
	cd cmd        && make clean

.FORCE:

show-targets:
	@show-makefile-targets

show-%:
	@echo $*=$($*)
