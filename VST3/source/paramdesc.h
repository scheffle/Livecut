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

#include "normplain.h"
#include <variant>

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
struct Range
{
	double min {0.};
	double max {1.};
};

//------------------------------------------------------------------------
struct StepCount
{
	uint32_t value {0};
	uint32_t startValue {0};
};

//------------------------------------------------------------------------
struct ParamDesc
{
	using Norm2ProcNativeFunc = double (*) (double normValue);

	const char16_t* name {nullptr};
	const double defaultNormalized {0.};
	const Norm2ProcNativeFunc toNative = [] (double v) { return v; };
	const std::variant<StepCount, Range> rangeOrStepCount {Range {}};
	const char16_t* const* stringList {nullptr};
};

//------------------------------------------------------------------------
} // Livecut
