# Filename: src/cmd/test/Makefile
# Project: wdiff-align
# Brief: Test code and data for wdiff-align and wdiff-align-series
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

.PHONY: all test clean

all: test

test:
	@echo "Test: hello -> hello world"
	@echo
	(cd .. && ./wdiff-align-series < test/hello )
	@echo
	@echo "Test: command frequency from shell history"
	@echo
	(cd .. && ./wdiff-align-series < test/history-command-frequency )

clean:
	rm -rf tmp

show-targets:
	@show-makefile-targets

show-%:
	@echo $*=$($*)
