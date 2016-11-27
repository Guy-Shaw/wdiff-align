/*
 * Filename: src/cmd/wdiff-align.c
 * Project: wdiff-align
 * Brief: wdiff-align core logic, separate from main driver program
 *
 * Copyright (C) 2016 Guy Shaw
 * Written by Guy Shaw <gshaw@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
    // Import type bool
    // Import constant false
    // Import constant true
#include <stdio.h>
    // Import constant EOF
    // Import type FILE
    // Import fflush()
    // Import fgetc()
    // Import fprintf()
    // Import fputc()
    // Import fputs()
#include <string.h>
    // Import strcmp()
    // Import strncmp()
#include <unistd.h>
    // Import type size_t

#include <cscript.h>

#define insert_start ((size_t)0xf001)
#define insert_end   ((size_t)0xf002)
#define delete_start ((size_t)0xf003)
#define delete_end   ((size_t)0xf004)

struct syntax {
    const char *str;
    size_t     suchar;
};

typedef struct syntax syntax_t;

#ifdef STANDARD_WDIFF

static syntax_t syntax_tbl[] = {
    { "{+", insert_start },
    { "+}", insert_end   },
    { "[-", delete_start },
    { "-]", delete_end   },
};

#else

static syntax_t syntax_tbl[] = {
    { "\x1c", insert_start },
    { "\x1d", insert_end   },
    { "\x1e", delete_start },
    { "\x1f", delete_end   },
};

#endif

/*
 * Get a single Super-Unicode character.
 *
 * Super-Unicode is a fictitious character set that includes extra
 * wide characters that cannot appear in ordinary Unicode, and can
 * safely be used to represent our own super-characters, such as
 * {start of insert, end of insert, start of delete, end of delete}.
 *
 * These super-characters cannot occur in the input file.
 * They must be represented by some sequence of characters.
 *
 * Here, we translate those input sequences to super-characters,
 * so that all other program logic need not deal with multi-byte
 * sequences.
 *
 * If an input byte is the start of a multi-byte sequence, then
 * more bytes are read from the input.  If a complete multi-byte
 * sequence is matched, then the corresponding super-character
 * is returned.  Since, the syntax table can have more than one
 * mutli-byte seqence with the same prefix, bytes must be read
 * until an unambiguous match is found, or until we know that the
 * partial sequence cannot match any super-character, in which case
 * an ordinary character is returned, and the input cursor is advanced
 * by only one character.
 *
 * This is functionally equivalent to an fgrep-like finte automaton
 * that is limitied to mathing a list of fixed strings.
 *
 */
static int
get_su_char(FILE *srcf)
{
    static char ungetbuf[16];
    static char sbuf[16];
    static size_t spos = 0;
    static size_t upos = 0;
    size_t n_syntax = 4;
    int c;

    while (true) {
        size_t i;
        bool any_pfx;

        if (upos) {
            --upos;
            c = ungetbuf[upos] & 0xff;
        }
        else {
            c = fgetc(srcf);
        }

        if (c == EOF) {
            return (EOF);
        }

        sbuf[spos] = c;
        sbuf[spos + 1] = '\0';
        any_pfx = false;
        for (i = 0; i < n_syntax; ++i) {
            const char *s = syntax_tbl[i].str;
            size_t len;

            len = 0;
            while (s[len]) {
                ++len;
            }

            if (strcmp(sbuf, s) == 0) {
                spos = 0;
                return (syntax_tbl[i].suchar);
            }

            any_pfx = any_pfx || (spos < len && strncmp(sbuf, s, spos + 1) == 0);
        }

        if (any_pfx) {
            // The characters accumulated in sbuf, so far,
            // is a prefix of at least one entry in syntax_tbl
            // So, keep reading raw characters.
            //
            ++spos;
            continue;
        }

        // sbuf is not a prefix of any members of syntax_tbl
        //

        if (spos) {
            // If we have accumulated any characters (not including c)
            // then put them back to be read the next time.
            //
            size_t pos;

            for (pos = spos; pos != 0; --pos) {
                ungetbuf[upos++] = sbuf[pos];
            }
            spos = 0;

            ungetbuf[upos] = '\0';
            c = sbuf[0];
            dbg_printf("putback: sbuf=[%s], ungetbuf=[%s], c=%c\n",
                       sbuf, ungetbuf, c);
        }

        return (c);
    }
}

int
test_get_su_char(FILE *srcf, FILE *dstf)
{
    size_t nprint = 0;

    while (true) {
        int c;

        c = get_su_char(srcf);
        if (c == EOF) {
            return (0);
        }
        if (c >= 0x20 && c <= 0x7e) {
            fputc(c, dstf);
        }
        else if (c == '\r' || c == '\n') {
            fputc(c, dstf);
        }
        else {
            fprintf(dstf, " %x ", c);
        }

        ++nprint;
        if (nprint >= 200) {
            fprintf(dstf, " ...\n");
            fflush(dstf);
            eprintf("Maxed out at %zu characters.\n", nprint);
            return (2);
        }
    }
}

void
switch_color(FILE *dstf, int prev_lc, int lc, int lnr)
{
    if (lc == prev_lc) {
        return;
    }

    if (lc == '-' && lnr == 1) {
        fputs("\e[01;31m\e[K", dstf);
    }
    else if (lc == '+' && lnr == 2) {
        fputs("\e[01;32m\e[K", dstf);
    }
    else {
        fputs("\e[m\e[K", dstf);
    }
}

void
wdiff_align(FILE *srcf, FILE *dstf, bool color, bool show_midline)
{
    static char l1buf[1024];
    static char l2buf[1024];
    static char lcbuf[1024];
    size_t pos;
    int c;
    bool in_insert = false;
    bool in_delete = false;

    pos = 0;
    while (true) {
        c = get_su_char(srcf);
        if (c == '\r' || c == '\n' || (c == EOF && pos != 0)) {
            /*
             * At the end of an input line, three display lines have been
             * computed:  1) before; 2) middle; 3) after.
             */
            size_t len = pos;
            int prev_lc;
            l1buf[pos] = '\0';
            l2buf[pos] = '\0';
            lcbuf[pos] = '\0';

            /*
             * Show line 1 -- before changes
             */
            prev_lc = 0;
            for (pos = 0; pos < len; ++pos) {
                if (color) {
                    switch_color(dstf, prev_lc, lcbuf[pos], 1);
                }
                fputc(l1buf[pos], dstf);
                prev_lc = lcbuf[pos];
            }
            fputc('|', dstf);
            fputc('\n', dstf);

            /*
             * Maybe show middle line, which marks insertions and deletions +/-
             */
            if (show_midline) {
                fputs(lcbuf, dstf);
                fputc('|', dstf);
                fputc('\n', dstf);
            }

            /*
             * Show line 2 -- after changes
             */
            prev_lc = 0;
            for (pos = 0; pos < len; ++pos) {
                if (color) {
                    switch_color(dstf, prev_lc, lcbuf[pos], 2);
                }
                fputc(l2buf[pos], dstf);
                prev_lc = lcbuf[pos];
            }
            fputc('|', dstf);
            fputc('\n', dstf);

            if (color) {
                fputs("\e[m\e[K", dstf);
            }
            pos = 0;

            if (c == EOF) {
                break;
            }
            continue;
        }

        if (c == EOF) {
            break;
        }

        /*
         * Manage switching between insert, delete (or stating the same)
         */
        switch (c) {
        case insert_start:
            in_insert = true;
            if (in_delete) {
                eprintf("WARNING:"
                    " not allowed to be inserting and deleting"
                    " at the same time.\n");
                eprintf("Canceling delete.\n");
                in_delete = false;
            }
            break;
        case insert_end:
            in_insert = false;
            break;
        case delete_start:
            in_delete = true;
            if (in_insert) {
                eprintf("WARNING:"
                    " not allowed to be inserting and deleting"
                    " at the same time.\n");
                eprintf("Canceling insert.\n");
                in_insert = false;
            }
            break;
        case delete_end:
            in_delete = false;
            break;
        }

        /*
         * Set the current character in all three display lines,
         * depending on whether we are inserting, deleting, or no change
         * in this character position.
         *
         * Compute all three display lines, even if we will not be showing
         * the middle line.
         */
        switch (c) {
        case insert_start:
        case insert_end:
        case delete_start:
        case delete_end:
            break;
        default:
            if (in_insert) {
                l1buf[pos] = ' ';
                l2buf[pos] = c;
                lcbuf[pos] = '+';
            }
            else if (in_delete) {
                l1buf[pos] = c;
                l2buf[pos] = ' ';
                lcbuf[pos] = '-';
            }
            else {
                l1buf[pos] = c;
                l2buf[pos] = c;
                lcbuf[pos] = ' ';
            }
            ++pos;
        }
    }
}
