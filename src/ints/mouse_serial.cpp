/*
 *  Copyright (C) 2022       The DOSBox Staging Team
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

#include "mouse.h"

#include <algorithm>

#include "checks.h"

#include "../hardware/serialport/serialmouse.h"

CHECK_NARROWING();

// Implementation here is very primitive, it mainly just passes notifications
// to registered listeners, which emulate a particular mouse on a particular
// serial (COM) port

static std::vector<CSerialMouse*> listeners;        // list of registered listeners
static float                      delta_x, delta_y; // accumulated mouse movement since last reported

// ***************************************************************************
// Serial interface implementation
// ***************************************************************************

void MOUSESERIAL_RegisterListener(CSerialMouse &listener)
{
	listeners.push_back(&listener);
}

void MOUSESERIAL_UnRegisterListener(CSerialMouse &listener)
{
	auto iter = std::find(listeners.begin(), listeners.end(), &listener);
	if (iter != listeners.end())
		listeners.erase(iter);
}

void MOUSESERIAL_NotifyMoved(const int16_t x_rel, const int16_t y_rel)
{
	static constexpr float MAX = 16384.0f;

	delta_x += std::clamp(static_cast<float>(x_rel) * mouse_config.sensitivity_x,
	                      -MAX,
	                      MAX);
	delta_y += std::clamp(static_cast<float>(y_rel) * mouse_config.sensitivity_y,
	                      -MAX,
	                      MAX);

	const int16_t dx = static_cast<int16_t>(std::round(delta_x));
	const int16_t dy = static_cast<int16_t>(std::round(delta_y));

	if (dx != 0 || dy != 0) {
		for (auto &listener : listeners)
			listener->OnMouseEventMoved(dx, dy);
		delta_x -= dx;
		delta_y -= dy;
	}
}

void MOUSESERIAL_NotifyPressed(const uint8_t buttons_12S, const uint8_t idx)
{
	for (auto &listener : listeners)
		listener->OnMouseEventButton(buttons_12S, idx);
}

void MOUSESERIAL_NotifyReleased(const uint8_t buttons_12S, const uint8_t idx)
{
	for (auto &listener : listeners)
		listener->OnMouseEventButton(buttons_12S, idx);
}

void MOUSESERIAL_NotifyWheel(const int16_t w_rel)
{
	for (auto &listener : listeners)
		listener->OnMouseEventWheel(static_cast<int8_t>(
		        std::clamp(w_rel,
		                   static_cast<int16_t>(INT8_MIN),
		                   static_cast<int16_t>(INT8_MAX))));
}
