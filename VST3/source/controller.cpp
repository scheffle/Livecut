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
#include "controller.h"
#include "paramdesc.h"
#include "parameter.h"
#include "pids.h"
#include "editordelegate.h"
#include "public.sdk/source/vst/vsthelpers.h"
#include "base/source/fstreamer.h"

#include <string>

using namespace Steinberg;

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
// LivecutController Implementation
//------------------------------------------------------------------------
LivecutController::LivecutController () = default;
LivecutController::~LivecutController () = default;

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instanciated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	for (auto pid = 0; pid < paramID (ParameterID::ParameterCount); ++pid)
	{
		auto param = new Parameter (pid, parameterDescriptions[pid]);
		param->setPrecision (0);
		parameters.addParameter (param);
	}

	parameters.getParameter (paramID (ParameterID::Bypass))->getInfo ().flags |=
	    Vst::ParameterInfo::kIsBypass;

	parameters.getParameter (paramID (ParameterID::CutCount))->getInfo ().flags =
	    Vst::ParameterInfo::kIsReadOnly;
	parameters.getParameter (paramID (ParameterID::BlockCount))->getInfo ().flags =
	    Vst::ParameterInfo::kIsReadOnly;

	editorDelegate = std::make_unique<EditorDelegate> (parameters);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutController::terminate ()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!
	editorDelegate.reset ();

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	uint32_t stateId = {};
	if (!streamer.readInt32u (stateId))
		return kResultFalse;
	if (stateId != StateIdentifier)
		return kResultFalse;
	uint32_t numParametersInState = {};
	if (!streamer.readInt32u (numParametersInState))
		return kResultFalse;
	if (numParametersInState > paramID (ParameterID::ParameterCount))
		return kResultFalse;

	double value;
	for (auto index = 0u; index < numParametersInState; ++index)
	{
		streamer.readDouble (value);
		if (auto parameter = parameters.getParameterByIndex ((index)))
			parameter->setNormalized (value);
	}
	for (auto index = numParametersInState; index < paramID (ParameterID::ParameterCount); ++index)
	{
		if (auto parameter = parameters.getParameterByIndex (index))
			parameter->setNormalized (parameterDescriptions[index].defaultNormalized);
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutController::setState (IBStream* state)
{
	// Here you get the state of the controller

	if (Vst::Helpers::isProjectState (state) == kResultTrue)
	{
		IBStreamer streamer (state, kLittleEndian);
		double value {};
		if (streamer.readDouble (value))
			editorDelegate->editorZoom = value;
	}

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API LivecutController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	if (Vst::Helpers::isProjectState (state))
	{
		IBStreamer streamer (state, kLittleEndian);
		streamer.writeDouble (editorDelegate->editorZoom);
	}

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API LivecutController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
#if defined(LIVECUT_VSTGUI_SUPPORT) && VSTGUI_NEWER_THAN_4_10
		auto editor = new VSTGUI::VST3Editor (this, "editor", "editor.uidesc");
		editor->setDelegate (editorDelegate.get ());
		return editor;
#else
		return nullptr;
#endif
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace Livecut
