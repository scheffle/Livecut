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

#include "cids.h"
#include "processor.h"
#include "paramdesc.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"

using namespace Steinberg;

namespace Livecut {

//------------------------------------------------------------------------
// LivecutProcessor
//------------------------------------------------------------------------
LivecutProcessor::LivecutProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kLivecutControllerUID);
	for (auto index = 0u; index < parameters.size(); ++index)
		parameters[index] = parameterDescriptions[index].defaultNormalized;

	processContextRequirements.needTempo ()
	    .needProjectTimeMusic ()
	    .needTimeSignature ()
	    .needTransportState ();
}

//------------------------------------------------------------------------
LivecutProcessor::~LivecutProcessor ()
{
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::initialize (FUnknown* context)
{
	tresult result = AudioEffect::initialize (context);
	if (result != kResultOk)
		return result;

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Vst::SpeakerArr::kStereo);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::terminate ()
{
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
                                                         SpeakerArrangement* outputs, int32 numOuts)
{
	// Livecut only supports Stereo
	if (numIns != numOuts || numIns != 1)
		return kResultFalse;
	if (inputs[0] != outputs[0] || inputs[0] != Vst::SpeakerArr::kStereo)
		return kResultFalse;
	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::process (Vst::ProcessData& data)
{
	stateTransfer.accessTransferObject_rt ([&] (auto state) {
		for (auto index = 0u; index < paramID (ParameterID::ParameterCount); ++index)
		{
			updateKernelParameter (index, state[index]);
		}
	});
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				paramQueue->getPoint (numPoints - 1, sampleOffset, value);
				updateKernelParameter (paramQueue->getParameterId (), value);
			}
		}
	}

	if (data.numSamples <= 0)
		return kResultTrue;

	Kernel::StereoBuffer inputs;
	Kernel::StereoBuffer outputs;

	auto ins = data.inputs[0];
	inputs[0] = ins.channelBuffers32[0];
	inputs[1] = ins.channelBuffers32[1];
	auto outs = data.outputs[0];
	outputs[0] = outs.channelBuffers32[0];
	outputs[1] = outs.channelBuffers32[1];

	if (doBypass)
	{
		for (auto index = 0; index < 2; ++index)
		{
			if (inputs[index] != outputs[index])
				memcpy(outputs[index], inputs[index], data.numSamples * sizeof (float));
		}
		outs.silenceFlags = ins.silenceFlags;
		return kResultTrue;
	}

	Kernel::TimeInfo timeInfo {};
	if (auto processContext = data.processContext)
	{
		if (processContext->state & Vst::ProcessContext::kTempoValid)
			timeInfo.tempo = processContext->tempo;
		if (processContext->state & Vst::ProcessContext::kTimeSigValid)
		{
			timeInfo.denominator = processContext->timeSigDenominator;
			timeInfo.numerator = processContext->timeSigNumerator;
		}
		if (processContext->state & Vst::ProcessContext::kProjectTimeMusicValid)
			timeInfo.ppqPos = processContext->projectTimeMusic;
		timeInfo.playing = processContext->state & Vst::ProcessContext::kPlaying;
		timeInfo.transportChanged = false; // TODO: need transport state observer
	}

	kernel.process (inputs, outputs, data.numSamples, timeInfo);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	kernel.setSampleRate (newSetup.sampleRate);
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);

	ParameterArray parameterState;
	for (auto index = 0u; index < paramID (ParameterID::ParameterCount); ++index)
		streamer.readDouble (parameterState[index]);

	stateTransfer.transferObject_ui (std::make_unique<ParameterArray> (std::move (parameterState)));

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	for (auto index = 0u; index < paramID (ParameterID::ParameterCount); ++index)
		streamer.writeDouble (parameters[index]);

	return kResultOk;
}

//------------------------------------------------------------------------
void LivecutProcessor::updateKernelParameter (ParamID pid, ParamValue value) noexcept
{
	parameters[pid] = value;
	value = parameterDescriptions[pid].toNative (value);
	switch (static_cast<ParameterID> (pid))
	{
		case ParameterID::CutProc:
		{
			kernel.setCutProc (value);
			break;
		}
		case ParameterID::SubDiv:
		{
			kernel.setSubDiv (value);
			break;
		}
		case ParameterID::Seed:
		{
			kernel.setSeed (value);
			break;
		}
		case ParameterID::Fade:
		{
			kernel.setFade (value);
			break;
		}
		case ParameterID::MinAmp:
		{
			kernel.setMinAmp (value);
			break;
		}
		case ParameterID::MaxAmp:
		{
			kernel.setMaxAmp (value);
			break;
		}
		case ParameterID::MinPan:
		{
			kernel.setMinPan (value);
			break;
		}
		case ParameterID::MaxPan:
		{
			kernel.setMaxPan (value);
			break;
		}
		case ParameterID::MinPitch:
		{
			kernel.setMinPitch (value);
			break;
		}
		case ParameterID::MaxPitch:
		{
			kernel.setMaxPitch (value);
			break;
		}
		case ParameterID::Duty:
		{
			kernel.setDuty (value);
			break;
		}
		case ParameterID::FillDuty:
		{
			kernel.setFillDuty (value);
			break;
		}
		case ParameterID::MaxPhrase:
		{
			kernel.setMaxPhrase (value);
			break;
		}
		case ParameterID::MinPhrase:
		{
			kernel.setMinPhrase (value);
			break;
		}
		case ParameterID::CutProc11MaxRepeat:
		{
			kernel.setMaxRepeat (value);
			break;
		}
		case ParameterID::CutProc11MinRepeat:
		{
			kernel.setMinRepeat (value);
			break;
		}
		case ParameterID::CutProc11Stutter:
		{
			kernel.setStutter (value);
			break;
		}
		case ParameterID::CutProc11Area:
		{
			kernel.setArea (value);
			break;
		}
		case ParameterID::WarpCutStraight:
		{
			kernel.setStraight (value);
			break;
		}
		case ParameterID::WarpCutRegular:
		{
			kernel.setRegular (value);
			break;
		}
		case ParameterID::WarpCutRitard:
		{
			kernel.setRitard (value);
			break;
		}
		case ParameterID::WarpCutSpeed:
		{
			kernel.setSpeed (value);
			break;
		}
		case ParameterID::SQPusherActivity:
		{
			kernel.setActivity (value);
			break;
		}
		case ParameterID::Crusher:
		{
			kernel.setBitcrusher (value);
			break;
		}
		case ParameterID::CrusherMinBits:
		{
			kernel.setMinBits (value);
			break;
		}
		case ParameterID::CrusherMaxBits:
		{
			kernel.setMaxBits (value);
			break;
		}
		case ParameterID::CrusherMinFreq:
		{
			kernel.setMinFreq (value);
			break;
		}
		case ParameterID::CrusherMaxFreq:
		{
			kernel.setMaxFreq (value);
			break;
		}
		case ParameterID::Comb:
		{
			kernel.setComb (value);
			break;
		}
		case ParameterID::CombType:
		{
			kernel.setCombType (value);
			break;
		}
		case ParameterID::CombFeedback:
		{
			kernel.setCombFeedback (value);
			break;
		}
		case ParameterID::CombMinDelay:
		{
			kernel.setCombMinDelay (value);
			break;
		}
		case ParameterID::CombMaxDelay:
		{
			kernel.setCombMaxDelay (value);
			break;
		}
		case ParameterID::Bypass:
		{
			doBypass = value;
			break;
		}
		case ParameterID::ParameterCount:
		{
			assert (false);
			break;
		}
	}
}

//------------------------------------------------------------------------
} // namespace Livecut
