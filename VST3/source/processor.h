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

#include "dspkernel.h"
#include "pids.h"
#include "public.sdk/source/vst/utility/rttransfer.h"
#include "public.sdk/source/vst/vstaudioeffect.h"

#include <array>

namespace Livecut {

//------------------------------------------------------------------------
//  LivecutProcessor
//------------------------------------------------------------------------
class LivecutProcessor : public Steinberg::Vst::AudioEffect
{
public:
	using tresult = Steinberg::tresult;
	using int32 = Steinberg::int32;
	using SpeakerArrangement = Steinberg::Vst::SpeakerArrangement;
	using FUnknown = Steinberg::FUnknown;

	LivecutProcessor ();
	~LivecutProcessor () override;

	// Create function
	static FUnknown* createInstance (void* /*context*/)
	{
		return (Steinberg::Vst::IAudioProcessor*)new LivecutProcessor;
	}

	tresult PLUGIN_API initialize (FUnknown* context) override;
	tresult PLUGIN_API terminate () override;
	tresult PLUGIN_API setActive (Steinberg::TBool state) override;
	tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) override;
	tresult PLUGIN_API setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
	                                       SpeakerArrangement* outputs, int32 numOuts) override;
	tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) override;
	tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) override;
	tresult PLUGIN_API setState (Steinberg::IBStream* state) override;
	tresult PLUGIN_API getState (Steinberg::IBStream* state) override;

//------------------------------------------------------------------------
protected:
	using ParamValue = Steinberg::Vst::ParamValue;
	using ParameterArray = std::array<ParamValue, paramID (ParameterID::ParameterCount)>;
	using RTTransfer = Steinberg::Vst::RTTransferT<ParameterArray>;

	void updateKernelParameter (ParamID pid, Steinberg::Vst::ParamValue value) noexcept;

	ParameterArray parameters;
	Kernel kernel;
	bool doBypass {false};
	
	RTTransfer stateTransfer;

	struct ParameterUpdater;
	std::unique_ptr<ParameterUpdater> cutCountUpdater;
	std::unique_ptr<ParameterUpdater> blockCountUpdater;
};

//------------------------------------------------------------------------
} // namespace Livecut
