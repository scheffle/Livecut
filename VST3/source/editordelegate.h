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

#include "controller.h"

#ifdef LIVECUT_VSTGUI_SUPPORT
#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#include "parameter.h"
#include <random>
#include <array>
#endif

//------------------------------------------------------------------------
namespace Livecut {

//------------------------------------------------------------------------
struct LivecutController::EditorDelegate
#if defined(LIVECUT_VSTGUI_SUPPORT) && VSTGUI_NEWER_THAN_4_10
: public VSTGUI::VST3EditorDelegate,
  VSTGUI::ViewListenerAdapter
#endif
{
#if defined(LIVECUT_VSTGUI_SUPPORT) && VSTGUI_NEWER_THAN_4_10
	using VST3Editor = VSTGUI::VST3Editor;
	using CView = VSTGUI::CView;
	using UIAttributes = VSTGUI::UIAttributes;
	using IUIDescription = VSTGUI::IUIDescription;
	using UTF8StringPtr = VSTGUI::UTF8StringPtr;

	EditorDelegate (Steinberg::Vst::ParameterContainer& p) : parameters (p) { init (); }

	void init ();
	Parameter* getParameterByIndex (uint32_t index) const;

	void didOpen (VST3Editor* editor) override;
	void onZoomChanged (VST3Editor* editor, double newZoom) override;
	CView* createCustomView (UTF8StringPtr name, const UIAttributes& attributes,
	                         const IUIDescription* description, VST3Editor* editor) override;
	CView* verifyView (CView* view, const UIAttributes& attributes,
	                   const IUIDescription* description, VST3Editor* editor) override;
	void viewWillDelete (CView* view) override;

	void startNewBoxAnimations (uint32_t numAnim);
	void updateBoxIterator ();
	
	void onCutProcChanged (const Parameter& param, double newValue);

	using ControlVector = std::vector<VSTGUI::CView*>;
	ControlVector cutVisualBoxes;
	ControlVector::iterator cutVisBoxIt {cutVisualBoxes.end ()};

	using CutProcContainerVector = std::array<VSTGUI::CView*, 3>;
	CutProcContainerVector cutProcContainers;

	std::random_device rd;
	std::mt19937 randGen {rd ()};

	Steinberg::Vst::ParameterContainer& parameters;

	static const std::vector<double> zoomFactors;
#else
	EditorDelegate (Steinberg::Vst::ParameterContainer& parameters) {}
#endif

	double editorZoom {1.};
};

//------------------------------------------------------------------------
} // Livecut
