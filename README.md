# wdiff-align -- wdiff of 2 lines, horizontally aligned

`wdiff-align` converts the output of `wdiff` to a format
that is horizontally aligned, after the fashion of gene sequence
alignment diagrams used in bioinformatics.
For each line of `wdiff` output, `wdiff-align` produces 3 lines:

 1. before
 2. middle
 3. after

The "before" and "after" lines are kept horizontally aligned
by padding with spaces.  The "before" line is padded with spaces
to keep aligned with added characters, while the "after" line
is padded with spaces where there are deletions.
The middle line marks additions and deletions with '+' and '-'.

## Example

```
wdiff <(echo Hello) <(echo Hello World.)
```

results in

```
Hello {+World.+}
```

This can be converted to:

```
Hello       |
     +++++++|
Hello World.|
```

by sending the output if `wdiff` with the appropriate options
to `wdiff-align`.

This does not look like it is much easier to read
in this small example,
but as things get more complicated
the ease of use of the aligned differences
becomes more apparent.

See the `test` subdirectory for a more complex example.

The middle line is optional.

The "before" and "after" lines can be colorized,
with deletions being colored red and insertions being colored in green.


## Align a series of changes

`wdiff-align-series` is a companion program that assumes
that its input is a series of incremental changes.
It breaks up its input into pairs of "before" and "after" lines.
The "after" line of one pair becomes the "before" line of the
next pair.  Each pair is fed to `wdiff-align`.

### Options to `wdiff-align-series`

--ltrim

--rtrim

--trim

It is a common enough case that there is a long run of content
that is equal at the beginning of each line (before the complications begin),
at the end of each line, or both.
The option, `--ltrim` causes a long prefix to be replaced by an ellipis;
`--rtrim` elides a long common suffix;
`--trim` does both.


## Why

The original motivation for creating `wdiff-align`
is to help clarify what series of changes were
made to a small piece of code.

Often enough, I start to try to solve some problem
by playing with a shell one-liner or perl one-liner,
or perhaps something that turns out to be a hybrid
shell/perl one-liner.
I quickly realize that the one-liner needs some refinement.
I have to make a more sophisticated pattern to select
just what I want.  Or, I have to filter out some special cases,
or otherwise deal with special cases.
The one line grows, incrementally.
The next version gets refined by using shell history editing.
Then my work gets interrupted.
Some time later, I get back to the shell history
which consists of several lines, each a small incremental change
from the previous line.  So, what exactly was I doing?
Where was I?  And, what do I do next?

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

##

-- Guy Shaw

   gshaw@acm.org

