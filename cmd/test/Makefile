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
	wdiff hello1 hello2 | ../wdiff-align -m
	@echo
	@echo Same, but using wdiff-align-series
	@echo
	cat hello1 hello2 | ../wdiff-align-series
	@echo
	@echo "Test: command frequency from shell history"
	@echo
	../wdiff-align-series < history-command-frequency
	@echo
	@echo "Test: similar bookmarklets -- nav/up-dir vs nav/up-dir-new-tab"
	@echo
	../wdiff-align-series < bookmarklets
	@echo
	@echo "Test: similar bookmarklets.  Same as above, but with --trim option."
	@echo
	../wdiff-align-series --trim < bookmarklets

clean:
	rm -rf tmp

show-targets:
	@show-makefile-targets

show-%:
	@echo $*=$($*)
