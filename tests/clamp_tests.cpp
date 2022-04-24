/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2021-2022  The DOSBox Staging Team
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

#include "support.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>

//  Usage: clamp<to_type>(x, min, max)
//  If x is less than min, return min.
//  If x is greater than max, return max.
//  If min is less than the to_type's minimum, then min is the to_type's
//  minimum. If max is greater than the to_type's maximum, then max is the
//  to_type's maximum.

// use a typedef to get the larger type between the two
template<class T, class U>
struct larger_type {
    using type = typename std::conditional<sizeof(T) >= sizeof(U), T, U>::type;
};

template <typename target_t, typename source_t, typename lower_bound_t, typename upper_bound_t>
target_t clamp_(const source_t val, const lower_bound_t lower,
               const upper_bound_t upper)
{
	assert(lower <= upper);

	using common_t = typename larger_type<target_t, source_t>::type;

	// can the lower bound be cast to the larger type?
	assert(static_cast<next_int_t<common_t>>(lower) == lower);

	// bound the lower bound to the target_t's minimum
	constexpr auto target_min = std::numeric_limits<target_t>::min();
	const auto lower_bound = std::max(
	        static_cast<next_int_t<common_t>>(lower), static_cast<next_int_t<common_t>>(target_min));

	// can the upper bound be cast to the larger type?
	assert(static_cast<next_int_t<common_t>>(upper) == upper);

	// bound the upper bound to the target_t's maximum
	constexpr auto target_max = std::numeric_limits<target_t>::max();
	const auto upper_bound = std::min(
	        static_cast<next_int_t<common_t>>(upper), static_cast<next_int_t<common_t> >(target_max));

	// can the value be cast to the larger type?
	assert(static_cast<next_int_t<common_t>>(val) == val);

	// clamp the value
	const auto clamped = std::clamp(static_cast<next_int_t<common_t>>(val), lower_bound, upper_bound);

	// can the clamped value be cast to the target_t?
	assert(static_cast<target_t>(clamped) == clamped);

	return static_cast<target_t>(clamped);
}

template <typename target_t, typename source_t>
constexpr target_t clamp_(const source_t val)
{
	constexpr auto target_min = std::numeric_limits<target_t>::min();
	constexpr auto target_max = std::numeric_limits<target_t>::max();
	return clamp_<target_t>(val, target_min, target_max);
}

namespace {

TEST(clamp, uint8_t_clamping)
{
	const auto mid_value = (UINT8_MAX - 0) / 2;
	const auto max_plus_1 = UINT8_MAX + 1;
	const auto min_minus_1 = 0 - 1;

	// Value in-range of limits [0, 255]

	auto val = clamp_<uint8_t>(0, 0, UINT8_MAX);

 	EXPECT_EQ(0, val);
	EXPECT_EQ(UINT8_MAX, clamp_<uint8_t>(UINT8_MAX, 0, UINT8_MAX));
	EXPECT_EQ(0x7F, clamp_<uint8_t>(mid_value, 0, UINT8_MAX));

	// Value out-of-range of limits [0, 255]
	EXPECT_EQ(0, clamp_<uint8_t>(min_minus_1, 0, UINT8_MAX));
	EXPECT_EQ(UINT8_MAX, clamp_<uint8_t>(max_plus_1, 0, UINT8_MAX));
	
	// Value and limits below range
	EXPECT_EQ(0, clamp_<uint8_t>(min_minus_1, min_minus_1, UINT8_MAX));

	// Value and limits above range
	EXPECT_EQ(UINT8_MAX, clamp_<uint8_t>(max_plus_1, 0, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(0, clamp_<uint8_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(UINT8_MAX, clamp_<uint8_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<uint8_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, uint16_t_clamping)
{
	const auto mid_value = (UINT16_MAX - 0) / 2;
	const auto max_plus_1 = UINT16_MAX + 1;
	const auto min_minus_1 = 0 - 1;

	// Value in-range of limits [0, 65535]
	EXPECT_EQ(0, clamp_<uint16_t>(0, 0, UINT16_MAX));
	EXPECT_EQ(UINT16_MAX, clamp_<uint16_t>(UINT16_MAX, 0, UINT16_MAX));
	EXPECT_EQ(mid_value, clamp_<uint16_t>(mid_value, 0, UINT16_MAX));

	// Value out-of-range of limits [0, 65535]
	EXPECT_EQ(0, clamp_<uint16_t>(min_minus_1, 0, UINT16_MAX));
	EXPECT_EQ(UINT16_MAX, clamp_<uint16_t>(max_plus_1, 0, UINT16_MAX));

	// Value and limits below range
	EXPECT_EQ(0, clamp_<uint16_t>(min_minus_1, min_minus_1, UINT16_MAX));

	// Value and limits above range
	EXPECT_EQ(UINT16_MAX, clamp_<uint16_t>(max_plus_1, 0, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(0, clamp_<uint16_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(UINT16_MAX, clamp_<uint16_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<uint16_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, uint32_t_clamping)
{
	const auto mid_value = (UINT32_MAX - 0) / 2;
	const auto max_plus_1 = UINT32_MAX + 1;
	const auto min_minus_1 = 0 - 1;

	// Value in-range of limits [0, 4294967295]
	EXPECT_EQ(0, clamp_<uint32_t>(0, 0, UINT32_MAX));
	EXPECT_EQ(UINT32_MAX, clamp_<uint32_t>(UINT32_MAX, 0, UINT32_MAX));
	EXPECT_EQ(mid_value, clamp_<uint32_t>(mid_value, 0, UINT32_MAX));

	// Value out-of-range of limits [0, 4294967295]
	EXPECT_EQ(0, clamp_<uint32_t>(min_minus_1, 0, UINT32_MAX));
	EXPECT_EQ(UINT32_MAX, clamp_<uint32_t>(max_plus_1, 0, UINT32_MAX));

	// Value and limits below range
	EXPECT_EQ(0, clamp_<uint32_t>(min_minus_1, min_minus_1, UINT32_MAX));

	// Value and limits above range
	EXPECT_EQ(UINT32_MAX, clamp_<uint32_t>(max_plus_1, 0, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(0, clamp_<uint32_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(UINT32_MAX, clamp_<uint32_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<uint32_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, uint64_t_clamping)
{
	const auto mid_value = (UINT64_MAX - 0) / 2;
	const auto max_plus_1 = UINT64_MAX + 1;
	const auto min_minus_1 = 0 - 1;

	// Value in-range of limits [0, 18446744073709551615]
	EXPECT_EQ(0, clamp_<uint64_t>(0, 0, UINT64_MAX));
	EXPECT_EQ(UINT64_MAX, clamp_<uint64_t>(UINT64_MAX, 0, UINT64_MAX));
	EXPECT_EQ(mid_value, clamp_<uint64_t>(mid_value, 0, UINT64_MAX));

	// Value out-of-range of limits [0, 18446744073709551615]
	EXPECT_EQ(0, clamp_<uint64_t>(min_minus_1, 0, UINT64_MAX));
	EXPECT_EQ(UINT64_MAX, clamp_<uint64_t>(max_plus_1, 0, UINT64_MAX));

	// Value and limits below range
	EXPECT_EQ(0, clamp_<uint64_t>(min_minus_1, min_minus_1, UINT64_MAX));

	// Value and limits above range
	EXPECT_EQ(UINT64_MAX, clamp_<uint64_t>(max_plus_1, 0, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(0, clamp_<uint64_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(UINT64_MAX, clamp_<uint64_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<uint64_t>(mid_value, mid_value, mid_value));
}
/*
TEST(clamp, int8_t) {

	const auto mid_value = (INT8_MAX - INT8_MIN) / 2;
	const auto max_plus_1 = INT8_MAX + 1;
	const auto min_minus_1 = INT8_MIN - 1;

	// Value in-range of limits [-128, 127]
	EXPECT_EQ(INT8_MIN, clamp_<int8_t>(INT8_MIN, INT8_MIN, INT8_MAX));
	EXPECT_EQ(INT8_MAX, clamp_<int8_t>(INT8_MAX, INT8_MIN, INT8_MAX));
	EXPECT_EQ(mid_value, clamp_<int8_t>(mid_value, INT8_MIN, INT8_MAX));

	// Value out-of-range of limits [-128, 127]
	EXPECT_EQ(INT8_MIN, clamp_<int8_t>(min_minus_1, INT8_MIN, INT8_MAX));
	EXPECT_EQ(INT8_MAX, clamp_<int8_t>(max_plus_1, INT8_MIN, INT8_MAX));

	// Value and limits below range
	EXPECT_EQ(INT8_MIN, clamp_<int8_t>(min_minus_1, min_minus_1, INT8_MAX));

	// Value and limits above range
	EXPECT_EQ(INT8_MAX, clamp_<int8_t>(max_plus_1, INT8_MIN, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(INT8_MIN, clamp_<int8_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(INT8_MAX, clamp_<int8_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<int8_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, int16_t) {

	const auto mid_value = (INT16_MAX - INT16_MIN) / 2;
	const auto max_plus_1 = INT16_MAX + 1;
	const auto min_minus_1 = INT16_MIN - 1;

	// Value in-range of limits [-32768, 32767]
	EXPECT_EQ(INT16_MIN, clamp_<int16_t>(INT16_MIN, INT16_MIN, INT16_MAX));
	EXPECT_EQ(INT16_MAX, clamp_<int16_t>(INT16_MAX, INT16_MIN, INT16_MAX));
	EXPECT_EQ(mid_value, clamp_<int16_t>(mid_value, INT16_MIN, INT16_MAX));

	// Value out-of-range of limits [-32768, 32767]
	EXPECT_EQ(INT16_MIN, clamp_<int16_t>(min_minus_1, INT16_MIN, INT16_MAX));
	EXPECT_EQ(INT16_MAX, clamp_<int16_t>(max_plus_1, INT16_MIN, INT16_MAX));

	// Value and limits below range
	EXPECT_EQ(INT16_MIN, clamp_<int16_t>(min_minus_1, min_minus_1, INT16_MAX));
	
	// Value and limits above range
	EXPECT_EQ(INT16_MAX, clamp_<int16_t>(max_plus_1, INT16_MIN, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(INT16_MIN, clamp_<int16_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(INT16_MAX, clamp_<int16_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<int16_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, int32_t) {

	const auto mid_value = (INT32_MAX - 0) / 2;
	const auto max_plus_1 = {INT32_MAX + 1};
	const auto min_minus_1 = 0 - 1;

	// Value in-range of limits [-2147483648, 2147483647]
	EXPECT_EQ(0, clamp_<int32_t>(0, 0, INT32_MAX));
	EXPECT_EQ(INT32_MAX, clamp_<int32_t>(INT32_MAX, 0, INT32_MAX));
	EXPECT_EQ(mid_value, clamp_<int32_t>(mid_value, 0, INT32_MAX));

	// Value out-of-range of limits [-2147483648, 2147483647]
	EXPECT_EQ(0, clamp_<int32_t>(min_minus_1, 0, INT32_MAX));
	EXPECT_EQ(INT32_MAX, clamp_<int32_t>(max_plus_1, 0, INT32_MAX));

	// Value and limits below range
	EXPECT_EQ(0, clamp_<int32_t>(min_minus_1, min_minus_1, INT32_MAX));

	// Value and limits above range
	EXPECT_EQ(INT32_MAX, clamp_<int32_t>(max_plus_1, 0, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(0, clamp_<int32_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(INT32_MAX, clamp_<int32_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<int32_t>(mid_value, mid_value, mid_value));
}

TEST(clamp, int64_t) {

	const auto mid_value = (INT64_MAX - INT64_MIN) / 2;
	const auto max_plus_1 = INT64_MAX + 1;
	const auto min_minus_1 = INT64_MIN - 1;

	// Value in-range of limits [-9223372036854775808, 9223372036854775807]
	EXPECT_EQ(INT64_MIN, clamp_<int64_t>(INT64_MIN, INT64_MIN, INT64_MAX));
	EXPECT_EQ(INT64_MAX, clamp_<int64_t>(INT64_MAX, INT64_MIN, INT64_MAX));
	EXPECT_EQ(mid_value, clamp_<int64_t>(mid_value, INT64_MIN, INT64_MAX));

	// Value out-of-range of limits [-9223372036854775808, 9223372036854775807]
	EXPECT_EQ(INT64_MIN, clamp_<int64_t>(min_minus_1, INT64_MIN, INT64_MAX));
	EXPECT_EQ(INT64_MAX, clamp_<int64_t>(max_plus_1, INT64_MIN, INT64_MAX));

	// Value and limits below range
	EXPECT_EQ(INT64_MIN, clamp_<int64_t>(min_minus_1, min_minus_1, INT64_MAX));

	// Value and limits above range
	EXPECT_EQ(INT64_MAX, clamp_<int64_t>(max_plus_1, INT64_MIN, max_plus_1));

	// Value below-range and both limits out-of-range
	EXPECT_EQ(INT64_MIN, clamp_<int64_t>(min_minus_1, min_minus_1, min_minus_1));

	// Value above-range and both limits out-of-range
	EXPECT_EQ(INT64_MAX, clamp_<int64_t>(max_plus_1, max_plus_1, max_plus_1));

	// Value and limits equal
	EXPECT_EQ(mid_value, clamp_<int64_t>(mid_value, mid_value, mid_value));

}

TEST(clamp, to_int_from_floating_point) {

	// Value in-range
	EXPECT_EQ(1, clamp_<int>(-1, -2.0f, 2.0f));
	EXPECT_EQ(1, clamp_<int>(-1, -2.0f, 2.0));

	EXPECT_EQ(0, clamp_<int>(0, -1.0f, 1.0f));
	EXPECT_EQ(0, clamp_<int>(0, -1.0, 1.0));

	EXPECT_EQ(1, clamp_<int>(1, 0.0f, 2.0f));
	EXPECT_EQ(1, clamp_<int>(1, 0.0, 2.0));

	// Value out-of-range
	EXPECT_EQ(0, clamp_<int>(1, -2.0f, 0.0f));
	EXPECT_EQ(0, clamp_<int>(1, -2.0, 0.0));

	EXPECT_EQ(1, clamp_<int>(2, 0.0f, 1.0f));
	EXPECT_EQ(1, clamp_<int>(2, 0.0, 1.0));

	EXPECT_EQ(0, clamp_<int>(-1, 0.0f, 2.0f));
	EXPECT_EQ(0, clamp_<int>(-1, 0.0, 2.0));

	// Value and limits are below range
	EXPECT_EQ(-128, clamp_<int8_t>(-500, -500.0f, 0.0f));

	// Value and limits are above range
	EXPECT_EQ(127, clamp_<int8_t>(500, 0.0f, 500.0f));

	// Value and limits are equal
	EXPECT_EQ(0, clamp_<int8_t>(0, 0.0f, 0.0f));
}

TEST(clamp, to_float_from_floating_point) {

	// lamda template function to compare float with FLT_EPSILON
	auto floats_are_equal = [](float a, float b) {
		return std::abs(a - b) < std::numeric_limits<float>::epsilon();
	};

	// lamda template function to compare double with DBL_EPSILON
	auto doubles_are_equal = [](double a, double b) {
		return std::abs(a - b) < std::numeric_limits<double>::epsilon();
	};

	// Value in-range
	EXPECT_TRUE(floats_are_equal(clamp_<float>(-1.0f, -2.0f, 2.0f), -1.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(-1.0, -2.0, 2.0), -1.0));

	EXPECT_TRUE(floats_are_equal(clamp_<float>(0.0f, -1.0f, 1.0f), 0.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(0.0, -1.0, 1.0), 0.0));

	EXPECT_TRUE(floats_are_equal(clamp_<float>(1.0f, 0.0f, 2.0f), 1.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(1.0, 0.0, 2.0), 1.0));

	// Value out-of-range
	EXPECT_TRUE(floats_are_equal(clamp_<float>(1.0f, -2.0f, 0.0f), 0.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(1.0, -2.0, 0.0), 0.0));

	EXPECT_TRUE(floats_are_equal(clamp_<float>(2.0f, 0.0f, 1.0f), 1.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(2.0, 0.0, 1.0), 1.0));

	EXPECT_TRUE(floats_are_equal(clamp_<float>(-1.0f, 0.0f, 2.0f), 0.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(-1.0, 0.0, 2.0), 0.0));

	// Value and limits are below range
	EXPECT_TRUE(floats_are_equal(clamp_<float>(-500.0f, -500.0f, 0.0f), -500.0f));

	// Value and limits are above range
	EXPECT_TRUE(floats_are_equal(clamp_<float>(500.0f, 0.0f, 500.0f), 500.0f));

	// Value and limits are equal
	EXPECT_TRUE(floats_are_equal(clamp_<float>(0.0f, 0.0f, 0.0f), 0.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(0.0, 0.0, 0.0), 0.0));

	// Value and limits are equal
	EXPECT_TRUE(floats_are_equal(clamp_<float>(0.0f, 0.0f, 0.0f), 0.0f));
	EXPECT_TRUE(doubles_are_equal(clamp_<double>(0.0, 0.0, 0.0), 0.0));
}
 */
} // namespace
