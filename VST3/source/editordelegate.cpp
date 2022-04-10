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

#include "editordelegate.h"
#include "parameter.h"
#include "pids.h"

#ifdef LIVECUT_VSTGUI_SUPPORT
#include "vstgui4/vstgui/lib/animation/animations.h"
#include "vstgui4/vstgui/lib/animation/timingfunctions.h"
#include "vstgui4/vstgui/lib/controls/ccontrol.h"
#include "vstgui4/vstgui/lib/iviewlistener.h"
#include "vstgui4/vstgui/uidescription/uiattributes.h"
#endif

//------------------------------------------------------------------------
namespace Livecut {

#if defined(LIVECUT_VSTGUI_SUPPORT) && VSTGUI_NEWER_THAN_4_10
const std::vector<double> LivecutController::EditorDelegate::zoomFactors = {0.75, 1.0,  1.25,
                                                                            1.50, 1.75, 2.0};
using namespace VSTGUI;
//------------------------------------------------------------------------
class CutVisBox : public CControl
{
public:
	CutVisBox () : CControl ({0, 0, 10, 10}) {}
	void draw (CDrawContext* pContext) override
	{
		auto normValue = getValueNormalized ();
		if (normValue == 0.f)
			return;
		pContext->saveGlobalState ();
		pContext->setGlobalAlpha (normValue);
		pContext->setFillColor (backcolor);
		pContext->drawRect (getViewSize (), kDrawFilled);
		pContext->restoreGlobalState ();
		setDirty (false);
	}

	void setColor (CColor c)
	{
		backcolor = c;
		invalid ();
	}

	CLASS_METHODS_NOCOPY (CutVisBox, CControl);

private:
	CColor backcolor {kWhiteCColor};
};

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::init ()
{
	if (auto param = parameters.getParameterByIndex (paramID (ParameterID::CutCount)))
	{
		static_cast<Parameter*> (param)->addListener (
		    [this] (auto, auto value) { startNewBoxAnimations (value * 1000); });
	}
	if (auto param = parameters.getParameterByIndex (paramID (ParameterID::BlockCount)))
	{
		static_cast<Parameter*> (param)->addListener (
		    [this] (auto, auto value) { startNewBoxAnimations (value * 1000); });
	}
}

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::didOpen (VST3Editor* editor)
{
	editor->setAllowedZoomFactors (zoomFactors);
	editor->setZoomFactor (editorZoom);
}

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::onZoomChanged (VST3Editor* editor, double newZoom)
{
	editorZoom = newZoom;
}

//------------------------------------------------------------------------
auto LivecutController::EditorDelegate::createCustomView (UTF8StringPtr name,
                                                          const UIAttributes& attributes,
                                                          const IUIDescription* description,
                                                          VST3Editor* editor) -> CView*
{
	if (name == std::string_view ("CutVisBox"))
	{
		return new CutVisBox;
	}
	return nullptr;
}

//------------------------------------------------------------------------
auto LivecutController::EditorDelegate::verifyView (CView* view, const UIAttributes& attributes,
                                                    const IUIDescription* description,
                                                    VST3Editor* editor) -> CView*
{
	if (auto customViewName = attributes.getAttributeValue (IUIDescription::kCustomViewName))
	{
		if (*customViewName == "SetupMenu")
		{
			if (auto menu = dynamic_cast<VSTGUI::COptionMenu*> (view))
			{
				menu->setStyle (menu->getStyle () | VSTGUI::COptionMenu::kMultipleCheckStyle);
				auto labelItem = new VSTGUI::CMenuItem ("UI Zoom");
				labelItem->setEnabled (false);
				menu->addEntry (labelItem);
				for (auto zf : zoomFactors)
				{
					auto tag = static_cast<int32_t> (zf * 100);
					auto str = VSTGUI::toString (tag);
					str += " %";
					auto item = new VSTGUI::CCommandMenuItem (str);
					item->setTag (tag);
					item->setActions ([editor, zf] (auto) { editor->setZoomFactor (zf); },
					                  [this] (auto item) {
						                  bool checked = item->getTag () ==
						                                 static_cast<int32_t> (editorZoom * 100);
						                  item->setChecked (checked);
					                  });
					item->setChecked (zf == editorZoom);
					menu->addEntry (item);
				}
			}
		}
		else if (*customViewName == "CutVisBox")
		{
			if (auto box = dynamic_cast<CutVisBox*> (view))
			{
				cutVisualBoxes.push_back (box);
				cutVisBoxIt = cutVisualBoxes.begin ();
				box->registerViewListener (this);
				box->setTag (-1);

				std::uniform_int_distribution<> randomDist (0u, 255u);

				CColor backcolor {};
				backcolor.red = randomDist (randGen);
				backcolor.green = randomDist (randGen);
				backcolor.blue = randomDist (randGen);
				box->setColor (backcolor);
			}
		}
	}
	return view;
}

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::viewWillDelete (CView* view)
{
	view->unregisterViewListener (this);
	auto it = std::find (cutVisualBoxes.begin (), cutVisualBoxes.end (), view);
	if (it != cutVisualBoxes.end ())
		cutVisualBoxes.erase (it);
}

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::startNewBoxAnimations (uint32_t numAnim)
{
	using namespace VSTGUI;
	using namespace VSTGUI::Animation;

	if (cutVisualBoxes.empty () || cutVisBoxIt == cutVisualBoxes.end ())
		return;

	for (auto i = 0u; i < numAnim; ++i)
	{
		updateBoxIterator ();
		auto control = static_cast<CControl*> (*cutVisBoxIt);
		if (control->isAttached () == false)
			continue;
		control->setValue (1.f);
		control->valueChanged ();
		control->addAnimation ("ControlAnim", new ControlValueAnimation (0.f, false),
		                       new LinearTimingFunction (250), [control] (auto, auto, auto) {
			                       control->setValue (0.f);
			                       control->valueChanged ();
		                       });
	}
}

//------------------------------------------------------------------------
void LivecutController::EditorDelegate::updateBoxIterator ()
{
	using namespace VSTGUI;
	using namespace VSTGUI::Animation;

	std::uniform_int_distribution<> randomDist (0u, cutVisualBoxes.size () - 1);

	for (auto i = 0u; i < 5u; ++i)
	{
		auto index = randomDist (randGen);
		auto newIt = cutVisualBoxes.begin ();
		std::advance (newIt, index);
		if (newIt != cutVisBoxIt)
		{
			cutVisBoxIt = newIt;
			break;
		}
	}
}

#endif

//------------------------------------------------------------------------
} // Livecut
