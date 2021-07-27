/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2020-2021  The DOSBox Staging Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOSBOX_TRANSLATION_H
#define DOSBOX_TRANSLATION_H

// DOSBox Staging uses the Gettext library for internationalization.
// https://www.gnu.org/software/gettext/manual/gettext.html

#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

// Messages can be registered and their translations fetched using DOSBox's convenient label system.  For example:
//
//   Register -- MSG_Add("SHORT_LABEL", _("possibly a very long string"));
//   Fetch    -- MSG_Get("SHORT_LABEL");
//
// Note: the message string-literals need to be wrapped at the source using gettext() or gettext_noop() to prevent the compiler from stripping them.
// 
void MSG_Add(const char * label, const char * message);
const char *MSG_Get(char const * label);

#endif
