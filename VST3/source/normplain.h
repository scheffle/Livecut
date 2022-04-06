/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 VST3 SDK Adaption by Arne Scheffler
 
 Livecut can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 Livecut is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Livecut; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA
 */

#pragma once

#include <algorithm>
#include <cstdint>

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
template <typename T>
inline constexpr T normalizedToPlain (T min, T max, T normalizedValue) noexcept
{
	return normalizedValue * (max - min) + min;
}

//------------------------------------------------------------------------
template <typename ReturnType = int32_t, typename ValueType = double>
inline constexpr ReturnType normalizedToSteps (int32_t numSteps, int32_t startValue,
                                               ValueType normalizedValue) noexcept
{
	return static_cast<ReturnType> (
	    std::min (numSteps,
	              static_cast<int32_t> (normalizedValue * static_cast<ValueType> (numSteps + 1))) +
	    startValue);
}

//------------------------------------------------------------------------
template <typename ReturnType = double, typename ValueType>
inline constexpr ReturnType plainToNormalized (ReturnType min, ReturnType max,
                                               ValueType plainValue) noexcept
{
	return (plainValue - min) / (max - min);
}

//------------------------------------------------------------------------
template <typename ReturnType = double, typename ValueType = double>
inline constexpr ReturnType stepsToNormalized (int32_t numSteps, int32_t startValue,
                                               ValueType plainValue) noexcept
{
	return static_cast<ReturnType> (plainValue - startValue) / static_cast<ReturnType> (numSteps);
}

//------------------------------------------------------------------------
} // Livecut
