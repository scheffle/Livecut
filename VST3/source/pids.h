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
#include "pluginterfaces/vst/vsttypes.h"

#include <array>

//------------------------------------------------------------------------
namespace Livecut {

using Steinberg::Vst::ParamID;

//------------------------------------------------------------------------
enum class ParameterID
{
	CutProc,
	SubDiv,
	Seed,
	Fade,
	MinAmp,
	MaxAmp,
	MinPan,
	MaxPan,
	MinPitch,
	MaxPitch,
	Duty,
	FillDuty,
	MaxPhrase,
	MinPhrase,
	CutProc11MaxRepeat,
	CutProc11MinRepeat,
	CutProc11Stutter,
	CutProc11Area,
	WarpCutStraight,
	WarpCutRegular,
	WarpCutRitard,
	WarpCutSpeed,
	SQPusherActivity,
	Crusher,
	CrusherMinBits,
	CrusherMaxBits,
	CrusherMinFreq,
	CrusherMaxFreq,
	Comb,
	CombType,
	CombFeedback,
	CombMinDelay,
	CombMaxDelay,
	Bypass,
	ParameterCount
};

//------------------------------------------------------------------------
inline constexpr ParamID paramID (ParameterID p)
{
	return static_cast<ParamID> (p);
}

//------------------------------------------------------------------------
static const constexpr std::array<uint32_t, 7> SubDivValues = {6, 8, 12, 16, 18, 24, 32};
static const constexpr std::array<const char16_t*, 7> SubDivValueStrings = {
    u"6", u"8", u"12", u"16", u"18", u"24", u"32"};

static const constexpr std::array<const char16_t*, 3> CutProcStrings = {u"CutProc11", u"WarpCut",
                                                                        u"SQPusher"};

static const constexpr std::array<const char16_t*, 2> CombTypeStrings = {u"FeedBack", u"FeedFwd"};

//------------------------------------------------------------------------
static constexpr std::array<ParamDesc, paramID (ParameterID::ParameterCount)>
    parameterDescriptions = {{
        {u"CutProc",
         0.,
         [] (auto v) { return normalizedToSteps<double> (2, 0, v); },
         {StepCount {2}},
         CutProcStrings.data ()},
        {u"SubDiv",
         0.,
         [] (auto v) {
	         return static_cast<double> (
	             SubDivValues[normalizedToSteps (SubDivValues.size () - 1, 0, v)]);
         },
         {StepCount {SubDivValues.size () - 1, 0}},
         SubDivValueStrings.data ()},
        {u"Seed",
         0.,
         [] (double v) { return normalizedToSteps<double> (15, 1, v); },
         {StepCount {15, 1}}},
        {u"Fade", 0., [] (auto v) { return v * 100.; }, {Range {0., 100.}}},
        {u"Min Amp", 1., [] (double v) { return v; }, {Range {0., 100.}}},
        {u"Max Amp", 1., [] (double v) { return v; }, {Range {0., 100.}}},
        {u"Min Pan", 0.4, [] (auto v) { return (v - 0.5) * 2.; }, {Range {-100., 100.}}},
        {u"Max Pan", 0.6, [] (auto v) { return (v - 0.5) * 2.; }, {Range {-100., 100.}}},
        {u"Min Pitch", 0.5, [] (auto v) { return (v - 0.5) * 4800.; }, {Range {-2400., 2400.}}},
        {u"Max Pitch", 0.5, [] (auto v) { return (v - 0.5) * 4800.; }, {Range {-2400., 2400.}}},
        {u"Duty", 1., [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"Fill Duty", 1., [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"Min Phrase",
         0.5,
         [] (auto v) { return normalizedToSteps<double> (7, 1, v); },
         {StepCount {7, 1}}},
        {u"Max Phrase",
         0.5,
         [] (auto v) { return normalizedToSteps<double> (7, 1, v); },
         {StepCount {7, 1}}},
        {u"CutProc11 Min Repeat",
         0.,
         [] (auto v) { return normalizedToSteps<double> (4, 0, v); },
         {StepCount {4}}},
        {u"CutProc11 Max Repeat",
         0.2,
         [] (auto v) { return normalizedToSteps<double> (4, 0, v); },
         {StepCount {4}}},
        {u"CutProc11 Stutter", 0.8, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"CutProc11 Area", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"WarpCut Straight", 0.3, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"WarpCut Regular", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"WarpCut Ritard", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"WarpCut Speed",
         0.9,
         [] (auto v) { return normalizedToPlain (0.5, 0.999, v); },
         {Range {0., 100.}}},
        {u"SQPusher Activity", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"Crusher", 0., [] (auto v) { return v > 0.5 ? 1. : 0.; }, {StepCount {1}}},
        {u"Crusher Min Bits",
         1.,
         [] (auto v) { return normalizedToSteps<double> (31, 1, v); },
         {StepCount {31, 1}}},
        {u"Crusher Max Bits",
         1.,
         [] (auto v) { return normalizedToSteps<double> (31, 1, v); },
         {StepCount {31, 1}}},
        {u"Crusher Min Freq", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"Crusher Max Freq", 0.5, [] (auto v) { return v; }, {Range {0., 100.}}},
        {u"Comb", 0., [] (auto v) { return v > 0.5 ? 1. : 0.; }, {StepCount {1}}},
        {u"Comb Type",
         0.,
         [] (auto v) { return v > 0.5 ? 1. : 0.; },
         {StepCount {1}},
         CombTypeStrings.data ()},
        {u"Comb Feedback",
         0.5,
         [] (auto v) { return normalizedToPlain (0., 0.9, v); },
         {Range {0., 90.}}},
        {u"Comb Min Delay",
         0.2,
         [] (auto v) { return normalizedToPlain (1., 50., v); },
         {Range {1., 50.}}},
        {u"Comb Max Delay",
         0.2,
         [] (auto v) { return normalizedToPlain (1., 50., v); },
         {Range {1., 50.}}},
        {u"Bypass", 0., [] (auto v) { return v > 0.5 ? 1. : 0.; }, {StepCount {1}}},
    }};

//------------------------------------------------------------------------
} // Livecut
