/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2019-2022  The DOSBox Staging Team
 *  Copyright (C) 2002-2017  The DOSBox Team
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

#include "gameblaster.h"

#include "setup.h"
#include "support.h"
#include "pic.h"

void GameBlaster::Open(const int port_choice, const std::string &card_choice,
                       const std::string &filter_choice)
{
	Close();
	assert(!is_open);

	is_standalone_gameblaster = (card_choice == "gb");

	// Ports are filtered and corrected by the conf system, so we simply
	// assert here
	const std::vector<io_port_t> valid_gb_ports = {0x210, 0x220, 0x230, 0x240, 0x250, 0x260};
	const std::vector<io_port_t> valid_cms_ports = {0x220, 0x240, 0x260, 0x280, 0x2a0, 0x2c0, 0x2e0, 0x300};
	const auto valid_ports = is_standalone_gameblaster ? valid_gb_ports
	                                                   : valid_cms_ports;
	base_port = check_cast<io_port_t>(port_choice);
	assert(contains(valid_ports, base_port));

	// Create the SAA1099 devices
	for (auto &d : devices) {
		d = std::make_unique<saa1099_device>(machine_config(), "", nullptr, chip_clock, render_divisor);
		d->device_start();
	}

	// Creative included CMS chips on several Sound Blaster cards, which
	// games could use (in addition to the SB features), so we always setup
	// those handlers - even if the card type isn't a GameBlaster.
	using namespace std::placeholders;
	const auto data_to_left = std::bind(&GameBlaster::WriteDataToLeftDevice, this, _1, _2, _3);
	const auto control_to_left = std::bind(&GameBlaster::WriteControlToLeftDevice, this, _1, _2, _3);
	const auto data_to_right = std::bind(&GameBlaster::WriteDataToRightDevice, this, _1, _2, _3);
	const auto control_to_right = std::bind(&GameBlaster::WriteControlToRightDevice, this, _1, _2, _3);

	write_handlers[0].Install(base_port, data_to_left, io_width_t::byte);
	write_handlers[1].Install(base_port + 1, control_to_left, io_width_t::byte);
	write_handlers[2].Install(base_port + 2, data_to_right, io_width_t::byte);
	write_handlers[3].Install(base_port + 3, control_to_right, io_width_t::byte);

	// However, standalone GameBlaster cards came with a dedicated chip on
	// it that could be used for detection. So we setup those handlers for
	// this chip only if the card-type is a GameBlaster:
	if (is_standalone_gameblaster) {
		const auto read_from_detection_port = std::bind(&GameBlaster::ReadFromDetectionPort, this, _1, _2);
		const auto write_to_detection_port = std::bind(&GameBlaster::WriteToDetectionPort, this, _1, _2, _3);

		read_handler_for_detection.Install(base_port, read_from_detection_port, io_width_t::byte, 16);
		write_handler_for_detection.Install(base_port + 4,
		                                    write_to_detection_port,
		                                    io_width_t::byte,
		                                    12);
	}

	// Setup the soft limiter
	soft_limiter = std::make_unique<SoftLimiter>(CardName());

	// Setup the mixer and level controls
	const auto audio_callback = std::bind(&GameBlaster::AudioCallback, this, _1);
	const auto level_callback = std::bind(&GameBlaster::LevelCallback, this, _1);
	channel = MIXER_AddChannel(audio_callback,
	                           0,
	                           CardName(),
	                           {ChannelFeature::Stereo,
	                            ChannelFeature::ReverbSend,
	                            ChannelFeature::ChorusSend});

	// The filter parameters have been tweaked by analysing real hardware
	// recordings. The results are virtually indistinguishable from the
	// real thing by ear only.
	if (filter_choice == "on") {
		constexpr auto order       = 1;
		constexpr auto cutoff_freq = 6000;
		channel->ConfigureLowPassFilter(order, cutoff_freq);
		channel->SetLowPassFilter(FilterState::On);
	} else {
		if (filter_choice != "off")
			LOG_WARNING("%s: Invalid filter setting '%s', using off",
			            CardName(),
			            filter_choice.c_str());

		channel->SetLowPassFilter(FilterState::Off);
	}

	channel->RegisterLevelCallBack(level_callback);

	// Calculate rates and ratio based on the mixer's rate
	const auto frame_rate_hz = channel->GetSampleRate();

	// Setup the resampler to convert from the render rate to the mixer's frame rate
	const auto max_freq = std::max(frame_rate_hz * 0.9 / 2, 8000.0);
	for (auto &r : resamplers)
		r.reset(reSIDfp::TwoPassSincResampler::create(render_rate_hz, frame_rate_hz, max_freq));

	LOG_MSG("%s: Running on port %xh with two %0.3f MHz Phillips SAA-1099 chips",
	        CardName(),
	        base_port,
	        chip_clock / 1e6);

	assert(channel);
	assert(devices[0]);
	assert(devices[1]);
	assert(soft_limiter);
	assert(resamplers[0]);
	assert(resamplers[1]);

	is_open = true;
}

bool GameBlaster::MaybeRenderFrame(const FrameSink sink)
{
	// Static containers setup once and reused
	constexpr auto num_channels = 2; // left and right
	static std::vector<int16_t> frame(num_channels);
	static int16_t *buffer[] = {&frame[0], &frame[1]};
	static std::vector<float> accumulator(num_channels);
	static device_sound_interface::sound_stream stream;

	// Accumulate the samples from both SAA-1099 devices
	devices[0]->sound_stream_update(stream, 0, buffer, 1);
	accumulator[0] = frame[0];
	accumulator[1] = frame[1];
	devices[1]->sound_stream_update(stream, 0, buffer, 1);
	accumulator[0] += frame[0];
	accumulator[1] += frame[1];

	// Increment our time datum up to which the device has rendered
	last_rendered_ms += ms_per_render;

	// Limit the accumulated frame to avoid hard-clipping
	soft_limiter->Process(accumulator, 1, frame);

	// Resample the limited frame
	const auto l_sample_ready = resamplers[0]->input(frame[0]);
	const auto r_sample_ready = resamplers[1]->input(frame[1]);
	// The resamplers should always have samples ready at the same time
	assert(l_sample_ready == r_sample_ready);

	// Inform the caller if we don't have a frame to fill the sink
	if (!l_sample_ready || !r_sample_ready)
		return false;

	// Get the frame from the resampler
	accumulator[0] = static_cast<float>(resamplers[0]->output());
	accumulator[1] = static_cast<float>(resamplers[1]->output());

	// Deposit the frame in the requested sink
	if (sink == FrameSink::Channel) {
		assert(channel);
		channel->AddSamples_sfloat(1, accumulator.data());
	} else {
		assert(sink == FrameSink::Queue);
		fifo.emplace(accumulator);
	}
	return true;
}

void GameBlaster::RenderUpToNow()
{
	const auto now = PIC_FullIndex();

	if (channel->is_enabled) {
		while (last_rendered_ms < now)
			MaybeRenderFrame(FrameSink::Queue);
		return;
	}
	// Otherwise wake up the channel and mark the new last-update time.
	// Subsequent renderings will get the new stream of frames.
	channel->Enable(true);
	last_rendered_ms = now;
}

void GameBlaster::WriteDataToLeftDevice(io_port_t, io_val_t value, io_width_t)
{
	RenderUpToNow();
	unused_for_ms = 0;
	devices[0]->data_w(0, 0, check_cast<uint8_t>(value));
}

void GameBlaster::WriteControlToLeftDevice(io_port_t, io_val_t value, io_width_t)
{
	RenderUpToNow();
	unused_for_ms = 0;
	devices[0]->control_w(0, 0, check_cast<uint8_t>(value));
}

void GameBlaster::WriteDataToRightDevice(io_port_t, io_val_t value, io_width_t)
{
	RenderUpToNow();
	unused_for_ms = 0;
	devices[1]->data_w(0, 0, check_cast<uint8_t>(value));
}

void GameBlaster::WriteControlToRightDevice(io_port_t, io_val_t value, io_width_t)
{
	RenderUpToNow();
	unused_for_ms = 0;
	devices[1]->control_w(0, 0, check_cast<uint8_t>(value));
}

void GameBlaster::AudioCallback(uint16_t requested_frames)
{
	assert(channel);

	//if (fifo.size())
	//	LOG_MSG("%s: Queued %2lu cycle-accurate frames", CardName(), fifo.size());

	// First, add any frames we've queued since the last callback
	while (requested_frames && fifo.size()) {
		channel->AddSamples_sfloat(1, fifo.front().data());
		fifo.pop();
		--requested_frames;
	}
	// When the queue's run dry, get the remainder from the device
	while (requested_frames)
		requested_frames -= MaybeRenderFrame(FrameSink::Channel);

	// At this point, we've given the channel enough frames to catch up
	// with "realtime". We can consider the current PIC index as our new
	// time datum, against which we can queue new frame on IO write events.
	assert(!requested_frames);
	last_rendered_ms = PIC_FullIndex();

	// Maybe idle the channel if the device has been unused for some time
	constexpr auto ten_ms_of_callbacks = 10 * 1000;
	if (unused_for_ms++ > ten_ms_of_callbacks)
		channel->Enable(false);
}

// The "Z:\> mixer CHANNEL VOLUME" normally scales a channels' samples after
// hard-clipping.We can avoid this hard-clipping by letting the soft-limiter
// manage the channel's level using this callback.
void GameBlaster::LevelCallback(const AudioFrame &levels)
{
	soft_limiter->UpdateLevels(levels, 1);
}

void GameBlaster::WriteToDetectionPort(io_port_t port, io_val_t value, io_width_t)
{
	switch (port - base_port) {
	case 0x6:
	case 0x7: cms_detect_register = check_cast<uint8_t>(value); break;
	}
}

uint8_t GameBlaster::ReadFromDetectionPort(io_port_t port, io_width_t) const
{
	uint8_t retval = 0xff;
	switch (port - base_port) {
	case 0x4:
		retval = 0x7f;
		break;
	case 0xa:
	case 0xb:
		retval = cms_detect_register;
		break;
	}
	return retval;
}

const char *GameBlaster::CardName() const
{
	return is_standalone_gameblaster ? "GAMEBLASTER" : "CMS";
}

void GameBlaster::Close()
{
	if (!is_open)
		return;

	LOG_INFO("%s: Shutting down the card on port %xh", CardName(), base_port);

	// Drop access to the IO ports
	for (auto &w : write_handlers)
		w.Uninstall();
	write_handler_for_detection.Uninstall();
	read_handler_for_detection.Uninstall();

	// Stop playback
	if (channel)
		channel->Enable(false);

	// Remove the mixer channel, SAA-1099 devices, soft-limiter, and resamplers
	channel.reset();
	devices[0].reset();
	devices[1].reset();
	soft_limiter.reset();
	resamplers[0].reset();
	resamplers[1].reset();

	is_open = false;
}

GameBlaster gameblaster;
void CMS_Init(Section *configuration)
{
	Section_prop *section = static_cast<Section_prop *>(configuration);
	gameblaster.Open(section->Get_hex("sbbase"),
	                 section->Get_string("sbtype"),
	                 section->Get_string("cms_filter"));
}
void CMS_ShutDown([[maybe_unused]] Section* sec) {
	gameblaster.Close();
}
