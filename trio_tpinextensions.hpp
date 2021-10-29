//*****************************************************************************
//
// Author		: Trotzky Vasily
// Date			: apr 2017
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
#ifndef TRIO_TPINEXTENSIONS_HPP_
#define TRIO_TPINEXTENSIONS_HPP_

#ifndef TRIO_HPP_
    #error "Do not include this file directly. Use #include\"trio.hpp\" instead!"
#endif

#include <stdint.h>
#include "boost_static_assert.h"

//We know on which MCU we are running, so use it for HW depandend ops(i.e. dirty hacks :)))
#ifdef TRIO_STM8_PORT_

#endif
#ifdef TRIO_STM32_PORT_

#endif
#ifdef TRIO_AVR_PORT_

#endif
#ifdef TRIO_XMEGA_PORT_

#endif

namespace TRIO
{
    namespace Private
    {
        template<class TPIN>
        class TPinInterfaceExtension
        {
            typedef typename TPIN::TRIOInternals::Port PORT;
            typedef typename PORT::DataT PortDataT;
            typedef typename TPIN::TRIOInternals::ConfigPort CONFIG_PORT;
            static const bool Inverted = TPIN::TRIOInternals::Inverted;
            static const uint8_t PIN = TPIN::TRIOInternals::PortPinNumber;
            static const PortDataT MASK = (PortDataT)1U << PIN;			//port mask for this pin
        public:
            /*
            *~~Put your TPin public interface extensions here~~
            *
            * Use TPIN to access already implemented TPin publics:
            * PORT, PIN, CONFIG_PORT are = TPin template parameters
            * MASK is the same as MASK in TPin implementation
            *
            * I want you to feel comfortable, almost like you are inside TPin class :)
            */

            //Example of using already implemented TPin method
            /*static void ExtensionMethod1()
            {
                TPIN::Configure::OutPushPull_Fast();
            }*/

            //Example of accessing port associated with TPin
            /*static void ExtensionMethod2()
            {
                CONFIG_PORT::Configure::template OutPushPull_Slow<MASK>();
            }*/

            //Example of accessing port low level API
            //HIGHLY NOT RECOMENDED!
            //Remember, you are extending TPin class interface
            //keep TPin as generic and portable as possible
            /*static void ExtensionMethod3()
            {
                PORT::template HWSpecificAPI<MASK>::GetGPIORegsReference().DDR |= MASK;
            }*/
        };//class TPinInterfaceExtension
    }//namespace Private
}//namespace TRIO

#endif /*TRIO_TPINEXTENSIONS_HPP_*/

