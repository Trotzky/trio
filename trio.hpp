//*****************************************************************************
// Author		: Trotskiy Vasily
// Date			: apr 2017
// All rights reserved.

// TrIO library was inspired by Konstantin Chizhov's paper http://easyelectronics.ru/rabota-s-portami-vvoda-vyvoda-mikrokontrollerov-na-si.html

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
#ifndef TRIO_HPP_
#define TRIO_HPP_

namespace TRIO{
    namespace Private{
        enum{ CHAR_BITS=8 };
        //enum{ CHAR_BITS=16 };//There is architectures in the world with 16bit char, my son ))
    }
}

#include <stdint.h>
#include "trio_regwrapper.hpp"

// Implementation of a Pin that belongs to some port
// Pin holds its position in the port and calls port API with mask to set, clear, read or toggle the pin.
// Configuration and some HWSpecific API is also supported by TPin
#include "trio_tpin.hpp"

// Platform specific IO ports implementation
// Add appropriate platform specific folder to your include paths
// This should declare ports like Porta, Portb, Portc ...etc
#include "trio_gpio_implementation.hpp"

//Declare port pins with good names like Pa0, Pa2, Pb1 ...etc
//File located in same platform specific directory as GPIOimplementation.hpp is
#include "trio_pins_declaration.hpp"

//Support of multi pin ports
#include "trio_pinlist.hpp"

//Easy apply same configuration to comma delimited group of pins
#include "trio_group_configure.hpp"

//NullPort and NullPin can be useful as a dummy
namespace TRIO
{
    struct NullPort//: public Private::GpioBase
	{
        typedef uint8_t DataT;
        enum{ Id = '-' };
		enum{ Width = sizeof(DataT) * 8 };

        /*struct TRIOInternals
        {
            typedef Private::GpioBase Base;
        };*/

		//Main runtime API======================
		static void Write(DataT value){ }		//Write to GPIO output register
        static DataT Read(){ return 0; }		//Read GPIO input register
		static DataT ReadOutReg(){ return 0; }  //Read GPIO output register    
        static void Set(DataT value){ }
        static void Clear(DataT value){ }
        static void ClearAndSet(DataT clearMask, DataT value){ }
        static void Toggle(DataT value){ }

        //Main compile time static API==================
        template<DataT value>
        static void Set(){ }
        template<DataT value>
        static void Clear(){ }
        template<DataT clearMask, DataT value>
        static void ClearAndSet(){ }
        template<DataT value>
        static void Toggle(){ }

        struct Configure
        {
            template<DataT MASK>
            static void OutPushPull_Slow(){ }
            template<DataT MASK>
            static void OutPushPull_Medium(){ }
            template<DataT MASK>
            static void OutPushPull_Fast(){ }
            template<DataT MASK>
            static void OutOpenDrain_Slow(){ }
            template<DataT MASK>
            static void OutOpenDrain_Medium(){ }
            template<DataT MASK>
            static void OutOpenDrain_Fast(){ }

            template<DataT MASK>
            static void InFloating(){  }
            template<DataT MASK>
            static void InPullUp(){  }
            template<DataT MASK>
            static void InPullDown(){  }
            template<DataT MASK>
            static void InAnalog(){  }

            template<DataT MASK>
            static void EnableAF(){ }
            template<DataT MASK>
            static void DisableAF(){ }
            template<DataT MASK>
            static void EnableInterrupt(){ }
            template<DataT MASK>
            static void DisableInterrupt(){ }
        };
        //=============Hardware specific API for maximum flexibility======================
        //TPin public interface will be extended by this hardware specific API
        //Some operations may need MASK to work, TPin will provide it for us
        template<DataT MASK=0>
        struct HWSpecificAPI{  };
	};//struct NullPort

	typedef Private::TPin< NullPort, 0 > NullPin;
}

#endif//TRIO_HPP_
