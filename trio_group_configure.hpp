//*****************************************************************************
//
// Author		: Trotzky Vasily
// Date			: may 2017
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.

// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

// Configure group of pins to same configuration.
// Basically we have this functionality in PinList.hpp but PinList follows GPIO Port interface and forces user
// to think about PinList as a port.
//
// Here we try simplify things to something like "here is list of pins, just configure them all"
//
// This will produce more optimal code then calling each pin configure methods individually
// because of grouping port register operations into one operation with mask.
// Grouping is smart and tuned fine for each supported platform.
// See ConfGrSizeMetric in PinList.hpp
//
// Usage example:
// IO::PinGroupConfigure< Pd1, Pd0, Pa1, Pb7, Pb5, Pb3 >::OutPushPull_Slow();
// Will configure all pins to OutPushPull_Slow();


#ifndef TRIO_GROUP_CONFIGURE_HPP_
#define TRIO_GROUP_CONFIGURE_HPP_

#ifndef TRIO_HPP_
    #error "Do not include this file directly. Use #include\"trio.hpp\" instead!"
#endif

#include <stdint.h>
#include "loki/Typelist.h"
#include "boost_static_assert.h"
#include "trio_pinlist.hpp"

namespace TRIO
{
    namespace Private
    {
        template < class PINLIST >
        class PinGroupConfigureImplementation
        {
            //Safe non-warning way to set all bits of arbitrary type DataT to 1. Give 0xFF for uint8_t and 0xFFFF for uint16_t
            static const typename PINLIST::DataT MASK = ~(typename PINLIST::DataT(0));
        public:
            static void OutPushPull_Slow(){ PINLIST::Configure::template OutPushPull_Slow<MASK>(); }
            static void OutPushPull_Medium(){ PINLIST::Configure::template OutPushPull_Medium<MASK>(); }
            static void OutPushPull_Fast(){ PINLIST::Configure::template OutPushPull_Fast<MASK>(); }
            static void OutOpenDrain_Slow(){ PINLIST::Configure::template OutOpenDrain_Slow<MASK>(); }
            static void OutOpenDrain_Medium(){ PINLIST::Configure::template OutOpenDrain_Medium<MASK>(); }
            static void OutOpenDrain_Fast(){ PINLIST::Configure::template OutOpenDrain_Fast<MASK>(); }

            static void InFloating(){ PINLIST::Configure::template InFloating<MASK>(); }
            static void InPullUp(){ PINLIST::Configure::template InPullUp<MASK>(); }
            static void InPullDown(){ PINLIST::Configure::template InPullDown<MASK>(); }
            static void InAnalog(){ PINLIST::Configure::template InAnalog<MASK>(); }

            static void EnableAF(){ PINLIST::Configure::template EnableAF<MASK>(); }
            static void DisableAF(){ PINLIST::Configure::template DisableAF<MASK>(); }
            static void EnableInterrupt(){ PINLIST::Configure::template EnableInterrupt<MASK>(); }
            static void DisableInterrupt(){ PINLIST::Configure::template DisableInterrupt<MASK>(); }
        };
    }//namespace Private

    template<
        typename T1  = Loki::NullType, typename T2  = Loki::NullType, typename T3  = Loki::NullType,
        typename T4  = Loki::NullType, typename T5  = Loki::NullType, typename T6  = Loki::NullType,
        typename T7  = Loki::NullType, typename T8  = Loki::NullType, typename T9  = Loki::NullType,
        typename T10 = Loki::NullType, typename T11 = Loki::NullType, typename T12 = Loki::NullType,
        typename T13 = Loki::NullType, typename T14 = Loki::NullType, typename T15 = Loki::NullType,
        typename T16 = Loki::NullType
    >
    class PinGroupConfigure: public Private::PinGroupConfigureImplementation< PinList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16> >
    {
    };
}//namespace TRIO

#endif

