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

#include "../../lib/BBCutter.h"
#include "../../lib/BitCrusher.h"
#include "../../lib/Comb.h"
#include "normplain.h"
#include "pids.h"

#include <algorithm>
#include <array>

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
struct Kernel
{
	Kernel () : bbcutter (player)
	{
		bbcutter.RegisterListener (&crusher);
		bbcutter.RegisterListener (&comb);
		bbcutter.SetSubdiv (subDiv);
	}

	void setCutProc (int32_t index) { bbcutter.SetCutProc (index); }
	void setSubDiv (int32_t value) { bbcutter.SetSubdiv (value); }
	void setSeed (int32_t value) { Math::randomseed (value); }
	void setFade (double ms) { bbcutter.SetFade (ms); }
	void setMinAmp (double norm) { bbcutter.SetMinAmp (norm); }
	void setMaxAmp (double norm) { bbcutter.SetMaxAmp (norm); }
	void setMinPan (double value) { bbcutter.SetMinPan (value); }
	void setMaxPan (double value) { bbcutter.SetMaxPan (value); }
	void setMinPitch (double value) { bbcutter.SetMinDetune (value); }
	void setMaxPitch (double value) { bbcutter.SetMaxDetune (value); }
	void setDuty (double value) { bbcutter.SetDutyCycle (value); }
	void setFillDuty (double value) { bbcutter.SetFillDutyCycle (value); }
	void setMaxPhrase (int32_t value) { bbcutter.SetMaxPhraseLength (value); }
	void setMinPhrase (int32_t value) { bbcutter.SetMinPhraseLength (value); }
	void setMaxRepeat (int32_t value) { bbcutter.SetMaxRepeats (value); }
	void setMinRepeat (int32_t value) { bbcutter.SetMinRepeats (value); }
	void setStutter (double value) { bbcutter.SetStutterChance (value); }
	void setArea (double value) { bbcutter.SetStutterArea (value); }
	void setStraight (double value) { bbcutter.SetStraightChance (value); }
	void setRegular (double value) { bbcutter.SetRegularChance (value); }
	void setRitard (double value) { bbcutter.SetRitardChance (value); }
	void setSpeed (double value) { bbcutter.SetAccel (value); }
	void setActivity (double value) { bbcutter.SetActivity (value); }
	void setBitcrusher (bool state) { crusher.SetOn (state); }
	void setMinBits (int32_t bits) { crusher.SetMinBits (bits); }
	void setMaxBits (int32_t bits) { crusher.SetMaxBits (bits); }
	void setMinFreq (double norm)
	{
		auto freq = normalizedToPlain (sampleRate / 100., sampleRate, norm);
		crusher.SetMinFreq (freq);
	}
	void setMaxFreq (double norm)
	{
		auto freq = normalizedToPlain (sampleRate / 100., sampleRate, norm);
		crusher.SetMaxFreq (freq);
	}
	void setComb (bool state) { comb.SetOn (state); }
	void setCombType (bool type) { comb.SetType (type); }
	void setCombFeedback (double feedback) { comb.SetFeedBack (feedback); }
	void setCombMinDelay (double ms) { comb.SetMinDelay (ms); }
	void setCombMaxDelay (double ms) { comb.SetMaxDelay (ms); }

	void setSampleRate (double rate)
	{
		sampleRate = rate;
		crusher.SetSampleRate (rate);
		comb.SetSampleRate (rate);
	}

	using StereoBuffer = std::array<float*, 2>;
	struct TimeInfo
	{
		double tempo {120};
		double numerator {4};
		double denominator {4};
		double ppqPos {0};
		bool playing {false};
		bool transportChanged {false};
	};

	void process (StereoBuffer inputs, StereoBuffer outputs, uint32_t numSamples,
	              const TimeInfo& timeInfo) noexcept
	{
		auto numSamplesD = static_cast<double> (numSamples);
		auto subDivNumerator = static_cast<double> (subDiv) / timeInfo.numerator;
		auto ppqBlockDuration = (numSamplesD / sampleRate) * (timeInfo.tempo / 60.0);
		auto divPerSample =
		    subDivNumerator * ppqBlockDuration * (timeInfo.denominator / 4.0) / numSamplesD;
		auto position = subDivNumerator * timeInfo.ppqPos * (timeInfo.denominator / 4.0);
		auto ref = 0.0;
		auto oldPositionInMeasure =
		    static_cast<int32_t> (std::floor (std::fmod (position - ref, subDiv)));
		auto oldMeasure = static_cast<int32_t> (std::floor ((position - ref) / subDiv));

		bbcutter.SetTimeInfos (timeInfo.tempo, timeInfo.numerator, timeInfo.denominator,
		                       sampleRate);

		if (timeInfo.transportChanged && timeInfo.playing)
			bbcutter.SetPosition (oldMeasure, oldPositionInMeasure);

		for (int i = 0; i < numSamples; i++)
		{
			int32_t positionInMeasure = int32_t (std::floor (std::fmod (position - ref, subDiv)));
			int32_t measure = int32_t (std::floor ((position - ref) / subDiv));

			if (positionInMeasure != oldPositionInMeasure)
			{
				bbcutter.SetPosition (measure, positionInMeasure);
				oldPositionInMeasure = positionInMeasure;
			}

			float l = 0.f;
			float r = 0.f;
			player.tick (l, r, inputs[0][i], inputs[1][i]);
			crusher.tick (l, r, l, r);
			comb.tick (l, r, l, r);
			outputs[0][i] = l;
			outputs[1][i] = r;
			oldMeasure = measure;
			position += divPerSample;
		}
	}

private:
	LivePlayer player;
	BitCrusher crusher;
	Comb comb;
	BBCutter bbcutter;

	double sampleRate {44100.};
	uint32_t subDiv {6};
};

//------------------------------------------------------------------------
} // Livecut
