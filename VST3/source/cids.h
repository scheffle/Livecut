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

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Livecut {
//------------------------------------------------------------------------
static DECLARE_UID (kLivecutProcessorUID, 0xA5CBCDB2, 0xB9745B88, 0xA69760B1, 0x64E92582);
static DECLARE_UID (kLivecutControllerUID, 0xFF0CD0C5, 0xB652589C, 0x9DE5538A, 0x50388A44);

#define LivecutVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace Livecut
