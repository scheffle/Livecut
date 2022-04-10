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

#include "public.sdk/source/vst/vsteditcontroller.h"

#ifdef LIVECUT_VSTGUI_SUPPORT
#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#endif

namespace Livecut {

//------------------------------------------------------------------------
//  LivecutController
//------------------------------------------------------------------------
class LivecutController : public Steinberg::Vst::EditControllerEx1
#ifdef LIVECUT_VSTGUI_SUPPORT
, public VSTGUI::VST3EditorDelegate
#endif
{
public:
//------------------------------------------------------------------------
	LivecutController () = default;
	~LivecutController () SMTG_OVERRIDE = default;

	// Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/)
	{
		return (Steinberg::Vst::IEditController*)new LivecutController;
	}

	// IPluginBase
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	// EditController
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

	//---Interface---------
	DEFINE_INTERFACES
	// Here you can add more supported VST3 interfaces
	// DEF_INTERFACE (Vst::IXXX)
	END_DEFINE_INTERFACES (EditController)
	DELEGATE_REFCOUNT (EditController)

//------------------------------------------------------------------------
protected:
#ifdef LIVECUT_VSTGUI_SUPPORT
	using VST3Editor = VSTGUI::VST3Editor;

	void didOpen (VST3Editor* editor) override;
#if VSTGUI_NEWER_THAN_4_10
	void onZoomChanged (VST3Editor* editor, double newZoom) override;
#endif // VSTGUI_NEWER_THAN_4_10
#endif // LIVECUT_VSTGUI_SUPPORT
	double editorZoom {1.};
};

//------------------------------------------------------------------------
} // namespace Livecut
