#!/usr/bin/python3

# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2021-2021  kcgen <1557255+kcgen@users.noreply.github.com>

# pylint: disable=invalid-name

"""
Convert a pair of DOSBox utf-8 text files to a gettext PO file.
"""

import io
import sys

def read_lng(infile, msg):

    '''Return a dictionary of labels and combined string values'''

    # iterator over the lines in the file
    with io.open(infile, 'r', encoding='utf8') as f:
        label = None
        message = list()
        label_messages = dict()

        for line in f:
            if label is None and line.startswith(':'):
                label = line[1:].strip()

            elif label is not None and not line.startswith('.'):
                msg_prefix = msg if len(message) == 0 else ""
                escaped = line.encode("unicode_escape").decode("utf-8")
                message.append('%-7s "%s"' % (msg_prefix, escaped))

            elif line.startswith('.') and label is not None and len(message) > 0:
                label_messages[label] = '\n'.join(message)
                label = None
                message = list()

    return label_messages

def write_po_header(outfile):

    '''write string literals as header to the po file'''

    lines = [
        "# SOME DESCRIPTIVE TITLE.",
        "# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER",
        "# This file is distributed under the same license as the dosbox-staging package.",
        "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.",
        "#",
        "#, fuzzy",
        'msgid   ""',
        'msgstr  "Project-Id-Version: dosbox-staging\\n"',
        '        "Report-Msgid-Bugs-To: \\n"',
        '        "POT-Creation-Date: 2021-07-26 19:12-0700\\n"',
        '        "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"',
        '        "Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n"',
        '        "Language-Team: LANGUAGE <LL@li.org>\\n"',
        '        "Language: \\n"',
        '        "MIME-Version: 1.0\\n"',
        '        "Content-Type: text/plain; charset=UTF-8\\n"',
        '        "Content-Transfer-Encoding: 8bit\\n"',
        '']
    with io.open(outfile, 'w', encoding='utf8') as f:
        # write the lines to the po file:
        f.write('\n'.join(lines))
        f.write(u'\n')

def write_lngs_to_po(english_lng, translated_lng, outfile):

    '''Writes matching english and translated (if any) values in PO format to the outfile'''

    write_po_header(outfile)

    # open the po file for writing:
    with io.open(outfile, 'a', encoding='utf8') as f:

        # iterate over labels and values in english_lng:
        for label, value in english_lng.items():

            # write the label as a comment to the po file:
            f.write('# %s\n' % label)

            # write formating tag, if needed:
            if '%' in value:
                f.write('#, c-format\n')

            # write the value to the po file:
            f.write('%s\n' % value)

            if label in translated_lng:
                # write the translated value to the po file:
                f.write('%s\n' % translated_lng[label])
            else:
                # write an empty message block to the po file:
                f.write('msgstr  ""\n')

            # write a blank line after the messages
            f.write('\n')

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: {} <english.lng> <translated.lng> <output.po>'.format(sys.argv[0]))
        sys.exit(1)

    # open the english and translated lng files and po file:
    english = read_lng(sys.argv[1], "msgid")
    translated = read_lng(sys.argv[2], "msgstr")
    write_lngs_to_po(english, translated, sys.argv[3])

    sys.exit(0)
