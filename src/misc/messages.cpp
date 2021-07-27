/*
 *  Copyright (C) 2002-2021  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "dosbox.h"

#include <string>
#include <locale.h>
#include <unordered_map>

#include "control.h"
#include "cross.h"
#include "setup.h"

#define LINE_IN_MAXLEN 2048

static std::unordered_map<std::string, std::string> messages;

// Add but don't replace existing
void MSG_Add(const char *label, const char *message)
{
	// Only add the message if it doesn't exist yet
	if (messages.find(label) == messages.end())
		messages[label] = message;
}

const char *MSG_Get(char const *requested_label)
{
	const auto it = messages.find(requested_label);
	if (it != messages.end())
		return gettext(it->second.c_str());
	return "Message not Found!\n";
}

// MSG_Init initializes gettext with the user's locale setting (if any)
void MSG_Init(Section_prop *section)
{
	// Get the command line's lanaguge setting
	std::string langauge;
	control->cmdline->FindString("-lang", langauge, true);

	// Fallback to the conf file's language setting
	if (langauge.empty())
		langauge = section->Get_path("language")->realpath;

	// Initialize Gettext
	constexpr char domain[] = "dosbox-staging";
	const auto locale_dir = CROSS_GetPlatformConfigDir() + "translations";
	bindtextdomain(domain, locale_dir.c_str());
	textdomain(domain);

	// Set the local to the user's setting, if if any
	setlocale(LC_ALL, langauge.c_str());
}
