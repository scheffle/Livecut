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

#include "paramdesc.h"
#include "public.sdk/source/vst/vstparameters.h"
#include <functional>

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
class Parameter : public Steinberg::Vst::Parameter
{
public:
	using Flags = Steinberg::Vst::ParameterInfo::ParameterFlags;
	using ParamID = Steinberg::Vst::ParamID;
	using ParamValue = Steinberg::Vst::ParamValue;
	using String128 = Steinberg::Vst::String128;
	using TChar = Steinberg::Vst::TChar;

	Parameter (ParamID pid, const ParamDesc& desc, int32_t flags = Flags::kCanAutomate);

	bool setNormalized (ParamValue v) override;
	void toString (ParamValue valueNormalized, String128 string) const override;
	bool fromString (const TChar* string, ParamValue& valueNormalized) const override;
	ParamValue toPlain (ParamValue valueNormalized) const override;
	ParamValue toNormalized (ParamValue plainValue) const override;

	using ValueChangedFunc = std::function<void (Parameter&,ParamValue)>;
	using Token = uint64_t;
	static constexpr const Token InvalidToken = 0u;

	Token addListener (const ValueChangedFunc& func);
	void removeListener (Token token);

private:
	const ParamDesc& desc;
	std::vector<std::pair<ValueChangedFunc, Token>> listeners;
	Token tokenCounter {0};
};

//------------------------------------------------------------------------
} // Livecut
