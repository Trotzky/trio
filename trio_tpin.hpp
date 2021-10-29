//*****************************************************************************
// Author		: Konstantin Chizhov
// Date			: 2010
// All rights reserved.
//
// rewrited > 90% by Trotskiy Vasily
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
#ifndef TRIO_TPIN_HPP_
#define TRIO_TPIN_HPP_

#ifndef TRIO_HPP_
    #error "Do not include this file directly. Use #include\"trio.hpp\" instead!"
#endif

#include <stdint.h>
#include "boost_static_assert.h"
#include "trio_tpinextensions.hpp"

namespace TRIO
{
    namespace Private
    {
    	template<class PORT, uint8_t PIN, class CONFIG_PORT = PORT>
    	class TPinImplementation
    	{
    		BOOST_STATIC_ASSERT(PIN < PORT::Width);
            static const typename PORT::DataT MASK = (typename PORT::DataT)1U << PIN;
        public:
            struct TRIOInternals //We need this to be public for internal library purposes
            {
                typedef PORT Port;
                typedef typename PORT::DataT PortDataT;
    		    typedef CONFIG_PORT ConfigPort;
                static const bool Inverted = false;
    		    static const uint8_t PortPinNumber = PIN;
            };

    		static void Set(bool val)
    		{
    			if( val )
                    PORT::template Set<MASK>();
    			else
    				PORT::template Clear<MASK>();
    		}
    		
			static void Set(){ Set(true); }
    		
			static void Clear(){ Set(false); }
    		
			static void Toggle(){ PORT::template Toggle<MASK>(); }
    		
			static bool IsSet()
			{ 
				return PORT::Read() & MASK;
			}

            struct Configure
            {
                //Generic config API. Should be implemented on every platform.
                static void OutPushPull_Slow(){		CONFIG_PORT::Configure::template OutPushPull_Slow<MASK>(); }
                
				static void OutPushPull_Medium(){	CONFIG_PORT::Configure::template OutPushPull_Medium<MASK>(); }
                
				static void OutPushPull_Fast(){		CONFIG_PORT::Configure::template OutPushPull_Fast<MASK>(); }
                
				static void OutOpenDrain_Slow(){	CONFIG_PORT::Configure::template OutOpenDrain_Slow<MASK>(); }
                
				static void OutOpenDrain_Medium(){	CONFIG_PORT::Configure::template OutOpenDrain_Medium<MASK>(); }
                
				static void OutOpenDrain_Fast(){	CONFIG_PORT::Configure::template OutOpenDrain_Fast<MASK>(); }

                
				static void InFloating(){			CONFIG_PORT::Configure::template InFloating<MASK>(); }
                
				static void InPullUp(){				CONFIG_PORT::Configure::template InPullUp<MASK>(); }
                
				static void InPullDown(){			CONFIG_PORT::Configure::template InPullDown<MASK>(); }
                
				static void InAnalog(){				CONFIG_PORT::Configure::template InAnalog<MASK>(); }

                static void EnableAF(){				CONFIG_PORT::Configure::template EnableAF<MASK>(); }
                
				static void DisableAF(){			CONFIG_PORT::Configure::template DisableAF<MASK>(); }
                
				static void EnableInterrupt(){		CONFIG_PORT::Configure::template EnableInterrupt<MASK>(); }
                
				static void DisableInterrupt(){		CONFIG_PORT::Configure::template DisableInterrupt<MASK>(); }
            };
            
			//Gives access to optional hardware specific port API
            typedef typename PORT::template HWSpecificAPI<MASK> HWSpecificAPI;
			
    	};//class TPinImplementation

        template<class PORT, uint8_t PIN, class CONFIG_PORT = PORT>
    	class InvertedPinImplementation: public TPinImplementation<PORT, PIN, CONFIG_PORT>
    	{
            static const typename PORT::DataT MASK = (typename PORT::DataT)1U << PIN;
    	public:
            struct TRIOInternals
            {
                typedef PORT Port;
                typedef typename PORT::DataT PortDataT;
    		    typedef CONFIG_PORT ConfigPort;
                static const bool Inverted = true;
    		    static const uint8_t PortPinNumber = PIN;
            };

    		static void Set(bool val)
    		{
    			if( val )
    				PORT::template Clear<MASK>();
    			else
    				PORT::template Set<MASK>();
    		}
    		
			static void Set(){	Set(true); }
    		
			static void Clear(){ Set(false); }
            
			static bool IsSet()
			{ 
				return !(PORT::PinRead() & MASK);
			}

    	};//class InvertedPinImplementation

        //Just put together interfaces of TPinImplementation and TPinInterfaceExtension
        template< class PORT, uint8_t PIN, class CONFIG_PORT = PORT >
        class TPin: public TPinImplementation<PORT, PIN, CONFIG_PORT>, public TPinInterfaceExtension< TPinImplementation<PORT, PIN, CONFIG_PORT> >
        { };

        //Just put together interfaces of InvertedPinImplementation and TPinInterfaceExtension
        template< class PORT, uint8_t PIN, class CONFIG_PORT = PORT >
        class InvertedPin: public InvertedPinImplementation<PORT, PIN, CONFIG_PORT>, public TPinInterfaceExtension< InvertedPinImplementation<PORT, PIN, CONFIG_PORT> >
        { };
    }// namespace Private
	
	
	//General define pin macro
	#define TRIO_DEFINE_PIN( PORT_TYPE_NAME, PIN_NAME_PREFIX, PIN_NUMBER )\
		typedef Private::TPin<PORT_TYPE_NAME, PIN_NUMBER> PIN_NAME_PREFIX##PIN_NUMBER;\
		typedef Private::InvertedPin<PORT_TYPE_NAME, PIN_NUMBER> PIN_NAME_PREFIX##PIN_NUMBER##Inv;
	
	/*This is only syntactic sugar macros! NOT Pin definitions!!!
	*  Note: Porta, Portb, Portc...etc port type names are defined in trio_gpio_implementation.hpp
	*  TRIO_DEFINE_P**() macroses are supposed to be used in hardware specific implementations 
	*  See trio_pins_declaration.hpp from your port implementation directory.
	*/	
	//Port A pins 	
	#define TRIO_DEFINE_PA0()	TRIO_DEFINE_PIN( Porta, Pa, 0 )
	#define TRIO_DEFINE_PA1()	TRIO_DEFINE_PIN( Porta, Pa, 1 )
	#define TRIO_DEFINE_PA2()	TRIO_DEFINE_PIN( Porta, Pa, 2 )
	#define TRIO_DEFINE_PA3()	TRIO_DEFINE_PIN( Porta, Pa, 3 )
	#define TRIO_DEFINE_PA4()	TRIO_DEFINE_PIN( Porta, Pa, 4 )
	#define TRIO_DEFINE_PA5()	TRIO_DEFINE_PIN( Porta, Pa, 5 )
	#define TRIO_DEFINE_PA6()	TRIO_DEFINE_PIN( Porta, Pa, 6 )
	#define TRIO_DEFINE_PA7()	TRIO_DEFINE_PIN( Porta, Pa, 7 )
	#define TRIO_DEFINE_PA8()	TRIO_DEFINE_PIN( Porta, Pa, 8 )
	#define TRIO_DEFINE_PA9()	TRIO_DEFINE_PIN( Porta, Pa, 9 )
	#define TRIO_DEFINE_PA10()	TRIO_DEFINE_PIN( Porta, Pa, 10 )
	#define TRIO_DEFINE_PA11()	TRIO_DEFINE_PIN( Porta, Pa, 11 )
	#define TRIO_DEFINE_PA12()	TRIO_DEFINE_PIN( Porta, Pa, 12 )
	#define TRIO_DEFINE_PA13()	TRIO_DEFINE_PIN( Porta, Pa, 13 )
	#define TRIO_DEFINE_PA14()	TRIO_DEFINE_PIN( Porta, Pa, 14 )
	#define TRIO_DEFINE_PA15()	TRIO_DEFINE_PIN( Porta, Pa, 15 )
	#define TRIO_DEFINE_PA16()	TRIO_DEFINE_PIN( Porta, Pa, 16 )
	#define TRIO_DEFINE_PA17()	TRIO_DEFINE_PIN( Porta, Pa, 17 )
	#define TRIO_DEFINE_PA18()	TRIO_DEFINE_PIN( Porta, Pa, 18 )
	#define TRIO_DEFINE_PA19()	TRIO_DEFINE_PIN( Porta, Pa, 19 )
	#define TRIO_DEFINE_PA20()	TRIO_DEFINE_PIN( Porta, Pa, 20 )
	#define TRIO_DEFINE_PA21()	TRIO_DEFINE_PIN( Porta, Pa, 21 )
	#define TRIO_DEFINE_PA22()	TRIO_DEFINE_PIN( Porta, Pa, 22 )
	#define TRIO_DEFINE_PA23()	TRIO_DEFINE_PIN( Porta, Pa, 23 )
	#define TRIO_DEFINE_PA24()	TRIO_DEFINE_PIN( Porta, Pa, 24 )
	#define TRIO_DEFINE_PA25()	TRIO_DEFINE_PIN( Porta, Pa, 25 )
	#define TRIO_DEFINE_PA26()	TRIO_DEFINE_PIN( Porta, Pa, 26 )
	#define TRIO_DEFINE_PA27()	TRIO_DEFINE_PIN( Porta, Pa, 27 )
	#define TRIO_DEFINE_PA28()	TRIO_DEFINE_PIN( Porta, Pa, 28 )
	#define TRIO_DEFINE_PA29()	TRIO_DEFINE_PIN( Porta, Pa, 29 )
	#define TRIO_DEFINE_PA30()	TRIO_DEFINE_PIN( Porta, Pa, 30 )
	#define TRIO_DEFINE_PA31()	TRIO_DEFINE_PIN( Porta, Pa, 31 )
	
	//Port B pins 		
	#define TRIO_DEFINE_PB0()	TRIO_DEFINE_PIN( Portb, Pb, 0  )
	#define TRIO_DEFINE_PB1() 	TRIO_DEFINE_PIN( Portb, Pb, 1  )
	#define TRIO_DEFINE_PB2() 	TRIO_DEFINE_PIN( Portb, Pb, 2  )
	#define TRIO_DEFINE_PB3() 	TRIO_DEFINE_PIN( Portb, Pb, 3  )
	#define TRIO_DEFINE_PB4() 	TRIO_DEFINE_PIN( Portb, Pb, 4  )
	#define TRIO_DEFINE_PB5() 	TRIO_DEFINE_PIN( Portb, Pb, 5  )
	#define TRIO_DEFINE_PB6() 	TRIO_DEFINE_PIN( Portb, Pb, 6  )
	#define TRIO_DEFINE_PB7() 	TRIO_DEFINE_PIN( Portb, Pb, 7  )
	#define TRIO_DEFINE_PB8() 	TRIO_DEFINE_PIN( Portb, Pb, 8  )
	#define TRIO_DEFINE_PB9() 	TRIO_DEFINE_PIN( Portb, Pb, 9  )
	#define TRIO_DEFINE_PB10()	TRIO_DEFINE_PIN( Portb, Pb, 10 )
	#define TRIO_DEFINE_PB11()	TRIO_DEFINE_PIN( Portb, Pb, 11 )
	#define TRIO_DEFINE_PB12()	TRIO_DEFINE_PIN( Portb, Pb, 12 )
	#define TRIO_DEFINE_PB13()	TRIO_DEFINE_PIN( Portb, Pb, 13 )
	#define TRIO_DEFINE_PB14()	TRIO_DEFINE_PIN( Portb, Pb, 14 )
	#define TRIO_DEFINE_PB15()	TRIO_DEFINE_PIN( Portb, Pb, 15 )
	#define TRIO_DEFINE_PB16()	TRIO_DEFINE_PIN( Portb, Pb, 16 )
	#define TRIO_DEFINE_PB17()	TRIO_DEFINE_PIN( Portb, Pb, 17 )
	#define TRIO_DEFINE_PB18()	TRIO_DEFINE_PIN( Portb, Pb, 18 )
	#define TRIO_DEFINE_PB19()	TRIO_DEFINE_PIN( Portb, Pb, 19 )
	#define TRIO_DEFINE_PB20()	TRIO_DEFINE_PIN( Portb, Pb, 20 )
	#define TRIO_DEFINE_PB21()	TRIO_DEFINE_PIN( Portb, Pb, 21 )
	#define TRIO_DEFINE_PB22()	TRIO_DEFINE_PIN( Portb, Pb, 22 )
	#define TRIO_DEFINE_PB23()	TRIO_DEFINE_PIN( Portb, Pb, 23 )
	#define TRIO_DEFINE_PB24()	TRIO_DEFINE_PIN( Portb, Pb, 24 )
	#define TRIO_DEFINE_PB25()	TRIO_DEFINE_PIN( Portb, Pb, 25 )
	#define TRIO_DEFINE_PB26()	TRIO_DEFINE_PIN( Portb, Pb, 26 )
	#define TRIO_DEFINE_PB27()	TRIO_DEFINE_PIN( Portb, Pb, 27 )
	#define TRIO_DEFINE_PB28()	TRIO_DEFINE_PIN( Portb, Pb, 28 )
	#define TRIO_DEFINE_PB29()	TRIO_DEFINE_PIN( Portb, Pb, 29 )
	#define TRIO_DEFINE_PB30()	TRIO_DEFINE_PIN( Portb, Pb, 30 )
	#define TRIO_DEFINE_PB31()	TRIO_DEFINE_PIN( Portb, Pb, 31 )
	
	//Port C pins
	#define TRIO_DEFINE_PC0()	TRIO_DEFINE_PIN( Portc, Pc, 0 )
	#define TRIO_DEFINE_PC1()	TRIO_DEFINE_PIN( Portc, Pc, 1  )
	#define TRIO_DEFINE_PC2()	TRIO_DEFINE_PIN( Portc, Pc, 2  )
	#define TRIO_DEFINE_PC3()	TRIO_DEFINE_PIN( Portc, Pc, 3  )
	#define TRIO_DEFINE_PC4()	TRIO_DEFINE_PIN( Portc, Pc, 4  )
	#define TRIO_DEFINE_PC5() 	TRIO_DEFINE_PIN( Portc, Pc, 5  )
	#define TRIO_DEFINE_PC6() 	TRIO_DEFINE_PIN( Portc, Pc, 6  )
	#define TRIO_DEFINE_PC7() 	TRIO_DEFINE_PIN( Portc, Pc, 7  )
	#define TRIO_DEFINE_PC8() 	TRIO_DEFINE_PIN( Portc, Pc, 8  )
	#define TRIO_DEFINE_PC9() 	TRIO_DEFINE_PIN( Portc, Pc, 9  )
	#define TRIO_DEFINE_PC10()	TRIO_DEFINE_PIN( Portc, Pc, 10 )
	#define TRIO_DEFINE_PC11()	TRIO_DEFINE_PIN( Portc, Pc, 11 )
	#define TRIO_DEFINE_PC12()	TRIO_DEFINE_PIN( Portc, Pc, 12 )
	#define TRIO_DEFINE_PC13()	TRIO_DEFINE_PIN( Portc, Pc, 13 )
	#define TRIO_DEFINE_PC14()	TRIO_DEFINE_PIN( Portc, Pc, 14 )
	#define TRIO_DEFINE_PC15()	TRIO_DEFINE_PIN( Portc, Pc, 15 )
	#define TRIO_DEFINE_PC16()	TRIO_DEFINE_PIN( Portc, Pc, 16 )
	#define TRIO_DEFINE_PC17()	TRIO_DEFINE_PIN( Portc, Pc, 17 )
	#define TRIO_DEFINE_PC18()	TRIO_DEFINE_PIN( Portc, Pc, 18 )
	#define TRIO_DEFINE_PC19()	TRIO_DEFINE_PIN( Portc, Pc, 19 )
	#define TRIO_DEFINE_PC20()	TRIO_DEFINE_PIN( Portc, Pc, 20 )
	#define TRIO_DEFINE_PC21()	TRIO_DEFINE_PIN( Portc, Pc, 21 )
	#define TRIO_DEFINE_PC22()	TRIO_DEFINE_PIN( Portc, Pc, 22 )
	#define TRIO_DEFINE_PC23()	TRIO_DEFINE_PIN( Portc, Pc, 23 )
	#define TRIO_DEFINE_PC24()	TRIO_DEFINE_PIN( Portc, Pc, 24 )
	#define TRIO_DEFINE_PC25()	TRIO_DEFINE_PIN( Portc, Pc, 25 )
	#define TRIO_DEFINE_PC26()	TRIO_DEFINE_PIN( Portc, Pc, 26 )
	#define TRIO_DEFINE_PC27()	TRIO_DEFINE_PIN( Portc, Pc, 27 )
	#define TRIO_DEFINE_PC28()	TRIO_DEFINE_PIN( Portc, Pc, 28 )
	#define TRIO_DEFINE_PC29()	TRIO_DEFINE_PIN( Portc, Pc, 29 )
	#define TRIO_DEFINE_PC30()	TRIO_DEFINE_PIN( Portc, Pc, 30 )
	#define TRIO_DEFINE_PC31()	TRIO_DEFINE_PIN( Portc, Pc, 31 )
	
	//Port D pins
	#define TRIO_DEFINE_PD0()	TRIO_DEFINE_PIN( Portd, Pd, 0 )
	#define TRIO_DEFINE_PD1() 	TRIO_DEFINE_PIN( Portd, Pd, 1  )
	#define TRIO_DEFINE_PD2() 	TRIO_DEFINE_PIN( Portd, Pd, 2  )
	#define TRIO_DEFINE_PD3() 	TRIO_DEFINE_PIN( Portd, Pd, 3  )
	#define TRIO_DEFINE_PD4() 	TRIO_DEFINE_PIN( Portd, Pd, 4  )
	#define TRIO_DEFINE_PD5() 	TRIO_DEFINE_PIN( Portd, Pd, 5  )
	#define TRIO_DEFINE_PD6() 	TRIO_DEFINE_PIN( Portd, Pd, 6  )
	#define TRIO_DEFINE_PD7() 	TRIO_DEFINE_PIN( Portd, Pd, 7  )
	#define TRIO_DEFINE_PD8() 	TRIO_DEFINE_PIN( Portd, Pd, 8  )
	#define TRIO_DEFINE_PD9() 	TRIO_DEFINE_PIN( Portd, Pd, 9  )
	#define TRIO_DEFINE_PD10()	TRIO_DEFINE_PIN( Portd, Pd, 10 )
	#define TRIO_DEFINE_PD11()	TRIO_DEFINE_PIN( Portd, Pd, 11 )
	#define TRIO_DEFINE_PD12()	TRIO_DEFINE_PIN( Portd, Pd, 12 )
	#define TRIO_DEFINE_PD13()	TRIO_DEFINE_PIN( Portd, Pd, 13 )
	#define TRIO_DEFINE_PD14()	TRIO_DEFINE_PIN( Portd, Pd, 14 )
	#define TRIO_DEFINE_PD15()	TRIO_DEFINE_PIN( Portd, Pd, 15 )
	#define TRIO_DEFINE_PD16()	TRIO_DEFINE_PIN( Portd, Pd, 16 )
	#define TRIO_DEFINE_PD17()	TRIO_DEFINE_PIN( Portd, Pd, 17 )
	#define TRIO_DEFINE_PD18()	TRIO_DEFINE_PIN( Portd, Pd, 18 )
	#define TRIO_DEFINE_PD19()	TRIO_DEFINE_PIN( Portd, Pd, 19 )
	#define TRIO_DEFINE_PD20()	TRIO_DEFINE_PIN( Portd, Pd, 20 )
	#define TRIO_DEFINE_PD21()	TRIO_DEFINE_PIN( Portd, Pd, 21 )
	#define TRIO_DEFINE_PD22()	TRIO_DEFINE_PIN( Portd, Pd, 22 )
	#define TRIO_DEFINE_PD23()	TRIO_DEFINE_PIN( Portd, Pd, 23 )
	#define TRIO_DEFINE_PD24()	TRIO_DEFINE_PIN( Portd, Pd, 24 )
	#define TRIO_DEFINE_PD25()	TRIO_DEFINE_PIN( Portd, Pd, 25 )
	#define TRIO_DEFINE_PD26()	TRIO_DEFINE_PIN( Portd, Pd, 26 )
	#define TRIO_DEFINE_PD27()	TRIO_DEFINE_PIN( Portd, Pd, 27 )
	#define TRIO_DEFINE_PD28()	TRIO_DEFINE_PIN( Portd, Pd, 28 )
	#define TRIO_DEFINE_PD29()	TRIO_DEFINE_PIN( Portd, Pd, 29 )
	#define TRIO_DEFINE_PD30()	TRIO_DEFINE_PIN( Portd, Pd, 30 )
	#define TRIO_DEFINE_PD31()	TRIO_DEFINE_PIN( Portd, Pd, 31 )
	
	//Port E pins
	#define TRIO_DEFINE_PE0()	TRIO_DEFINE_PIN( Porte, Pe, 0 )
	#define TRIO_DEFINE_PE1()	TRIO_DEFINE_PIN( Porte, Pe, 1  )
	#define TRIO_DEFINE_PE2() 	TRIO_DEFINE_PIN( Porte, Pe, 2  )
	#define TRIO_DEFINE_PE3() 	TRIO_DEFINE_PIN( Porte, Pe, 3  )
	#define TRIO_DEFINE_PE4() 	TRIO_DEFINE_PIN( Porte, Pe, 4  )
	#define TRIO_DEFINE_PE5() 	TRIO_DEFINE_PIN( Porte, Pe, 5  )
	#define TRIO_DEFINE_PE6() 	TRIO_DEFINE_PIN( Porte, Pe, 6  )
	#define TRIO_DEFINE_PE7() 	TRIO_DEFINE_PIN( Porte, Pe, 7  )
	#define TRIO_DEFINE_PE8() 	TRIO_DEFINE_PIN( Porte, Pe, 8  )
	#define TRIO_DEFINE_PE9() 	TRIO_DEFINE_PIN( Porte, Pe, 9  )
	#define TRIO_DEFINE_PE10()	TRIO_DEFINE_PIN( Porte, Pe, 10 )
	#define TRIO_DEFINE_PE11()	TRIO_DEFINE_PIN( Porte, Pe, 11 )
	#define TRIO_DEFINE_PE12()	TRIO_DEFINE_PIN( Porte, Pe, 12 )
	#define TRIO_DEFINE_PE13()	TRIO_DEFINE_PIN( Porte, Pe, 13 )
	#define TRIO_DEFINE_PE14()	TRIO_DEFINE_PIN( Porte, Pe, 14 )
	#define TRIO_DEFINE_PE15()	TRIO_DEFINE_PIN( Porte, Pe, 15 )
	#define TRIO_DEFINE_PE16()	TRIO_DEFINE_PIN( Porte, Pe, 16 )
	#define TRIO_DEFINE_PE17()	TRIO_DEFINE_PIN( Porte, Pe, 17 )
	#define TRIO_DEFINE_PE18()	TRIO_DEFINE_PIN( Porte, Pe, 18 )
	#define TRIO_DEFINE_PE19()	TRIO_DEFINE_PIN( Porte, Pe, 19 )
	#define TRIO_DEFINE_PE20()	TRIO_DEFINE_PIN( Porte, Pe, 20 )
	#define TRIO_DEFINE_PE21()	TRIO_DEFINE_PIN( Porte, Pe, 21 )
	#define TRIO_DEFINE_PE22()	TRIO_DEFINE_PIN( Porte, Pe, 22 )
	#define TRIO_DEFINE_PE23()	TRIO_DEFINE_PIN( Porte, Pe, 23 )
	#define TRIO_DEFINE_PE24()	TRIO_DEFINE_PIN( Porte, Pe, 24 )
	#define TRIO_DEFINE_PE25()	TRIO_DEFINE_PIN( Porte, Pe, 25 )
	#define TRIO_DEFINE_PE26()	TRIO_DEFINE_PIN( Porte, Pe, 26 )
	#define TRIO_DEFINE_PE27()	TRIO_DEFINE_PIN( Porte, Pe, 27 )
	#define TRIO_DEFINE_PE28()	TRIO_DEFINE_PIN( Porte, Pe, 28 )
	#define TRIO_DEFINE_PE29()	TRIO_DEFINE_PIN( Porte, Pe, 29 )
	#define TRIO_DEFINE_PE30()	TRIO_DEFINE_PIN( Porte, Pe, 30 )
	#define TRIO_DEFINE_PE31()	TRIO_DEFINE_PIN( Porte, Pe, 31 )
	
	//Port F pins
	#define TRIO_DEFINE_PF0()	TRIO_DEFINE_PIN( Portf, Pf, 0 )
	#define TRIO_DEFINE_PF1() 	TRIO_DEFINE_PIN( Portf, Pf, 1  )
	#define TRIO_DEFINE_PF2() 	TRIO_DEFINE_PIN( Portf, Pf, 2  )
	#define TRIO_DEFINE_PF3() 	TRIO_DEFINE_PIN( Portf, Pf, 3  )
	#define TRIO_DEFINE_PF4() 	TRIO_DEFINE_PIN( Portf, Pf, 4  )
	#define TRIO_DEFINE_PF5() 	TRIO_DEFINE_PIN( Portf, Pf, 5  )
	#define TRIO_DEFINE_PF6() 	TRIO_DEFINE_PIN( Portf, Pf, 6  )
	#define TRIO_DEFINE_PF7() 	TRIO_DEFINE_PIN( Portf, Pf, 7  )
	#define TRIO_DEFINE_PF8() 	TRIO_DEFINE_PIN( Portf, Pf, 8  )
	#define TRIO_DEFINE_PF9() 	TRIO_DEFINE_PIN( Portf, Pf, 9  )
	#define TRIO_DEFINE_PF10()	TRIO_DEFINE_PIN( Portf, Pf, 10 )
	#define TRIO_DEFINE_PF11()	TRIO_DEFINE_PIN( Portf, Pf, 11 )
	#define TRIO_DEFINE_PF12()	TRIO_DEFINE_PIN( Portf, Pf, 12 )
	#define TRIO_DEFINE_PF13()	TRIO_DEFINE_PIN( Portf, Pf, 13 )
	#define TRIO_DEFINE_PF14()	TRIO_DEFINE_PIN( Portf, Pf, 14 )
	#define TRIO_DEFINE_PF15()	TRIO_DEFINE_PIN( Portf, Pf, 15 )
	#define TRIO_DEFINE_PF16()	TRIO_DEFINE_PIN( Portf, Pf, 16 )
	#define TRIO_DEFINE_PF17()	TRIO_DEFINE_PIN( Portf, Pf, 17 )
	#define TRIO_DEFINE_PF18()	TRIO_DEFINE_PIN( Portf, Pf, 18 )
	#define TRIO_DEFINE_PF19()	TRIO_DEFINE_PIN( Portf, Pf, 19 )
	#define TRIO_DEFINE_PF20()	TRIO_DEFINE_PIN( Portf, Pf, 20 )
	#define TRIO_DEFINE_PF21()	TRIO_DEFINE_PIN( Portf, Pf, 21 )
	#define TRIO_DEFINE_PF22()	TRIO_DEFINE_PIN( Portf, Pf, 22 )
	#define TRIO_DEFINE_PF23()	TRIO_DEFINE_PIN( Portf, Pf, 23 )
	#define TRIO_DEFINE_PF24()	TRIO_DEFINE_PIN( Portf, Pf, 24 )
	#define TRIO_DEFINE_PF25()	TRIO_DEFINE_PIN( Portf, Pf, 25 )
	#define TRIO_DEFINE_PF26()	TRIO_DEFINE_PIN( Portf, Pf, 26 )
	#define TRIO_DEFINE_PF27()	TRIO_DEFINE_PIN( Portf, Pf, 27 )
	#define TRIO_DEFINE_PF28()	TRIO_DEFINE_PIN( Portf, Pf, 28 )
	#define TRIO_DEFINE_PF29()	TRIO_DEFINE_PIN( Portf, Pf, 29 )
	#define TRIO_DEFINE_PF30()	TRIO_DEFINE_PIN( Portf, Pf, 30 )
	#define TRIO_DEFINE_PF31()	TRIO_DEFINE_PIN( Portf, Pf, 31 )
	
	//Port G pins
	#define TRIO_DEFINE_PG0()	TRIO_DEFINE_PIN( Portg, Pg, 0 )
	#define TRIO_DEFINE_PG1() 	TRIO_DEFINE_PIN( Portg, Pg, 1  )
	#define TRIO_DEFINE_PG2() 	TRIO_DEFINE_PIN( Portg, Pg, 2  )
	#define TRIO_DEFINE_PG3() 	TRIO_DEFINE_PIN( Portg, Pg, 3  )
	#define TRIO_DEFINE_PG4() 	TRIO_DEFINE_PIN( Portg, Pg, 4  )
	#define TRIO_DEFINE_PG5() 	TRIO_DEFINE_PIN( Portg, Pg, 5  )
	#define TRIO_DEFINE_PG6() 	TRIO_DEFINE_PIN( Portg, Pg, 6  )
	#define TRIO_DEFINE_PG7() 	TRIO_DEFINE_PIN( Portg, Pg, 7  )
	#define TRIO_DEFINE_PG8() 	TRIO_DEFINE_PIN( Portg, Pg, 8  )
	#define TRIO_DEFINE_PG9() 	TRIO_DEFINE_PIN( Portg, Pg, 9  )
	#define TRIO_DEFINE_PG10()	TRIO_DEFINE_PIN( Portg, Pg, 10 )
	#define TRIO_DEFINE_PG11()	TRIO_DEFINE_PIN( Portg, Pg, 11 )
	#define TRIO_DEFINE_PG12()	TRIO_DEFINE_PIN( Portg, Pg, 12 )
	#define TRIO_DEFINE_PG13()	TRIO_DEFINE_PIN( Portg, Pg, 13 )
	#define TRIO_DEFINE_PG14()	TRIO_DEFINE_PIN( Portg, Pg, 14 )
	#define TRIO_DEFINE_PG15()	TRIO_DEFINE_PIN( Portg, Pg, 15 )
	#define TRIO_DEFINE_PG16()	TRIO_DEFINE_PIN( Portg, Pg, 16 )
	#define TRIO_DEFINE_PG17()	TRIO_DEFINE_PIN( Portg, Pg, 17 )
	#define TRIO_DEFINE_PG18()	TRIO_DEFINE_PIN( Portg, Pg, 18 )
	#define TRIO_DEFINE_PG19()	TRIO_DEFINE_PIN( Portg, Pg, 19 )
	#define TRIO_DEFINE_PG20()	TRIO_DEFINE_PIN( Portg, Pg, 20 )
	#define TRIO_DEFINE_PG21()	TRIO_DEFINE_PIN( Portg, Pg, 21 )
	#define TRIO_DEFINE_PG22()	TRIO_DEFINE_PIN( Portg, Pg, 22 )
	#define TRIO_DEFINE_PG23()	TRIO_DEFINE_PIN( Portg, Pg, 23 )
	#define TRIO_DEFINE_PG24()	TRIO_DEFINE_PIN( Portg, Pg, 24 )
	#define TRIO_DEFINE_PG25()	TRIO_DEFINE_PIN( Portg, Pg, 25 )
	#define TRIO_DEFINE_PG26()	TRIO_DEFINE_PIN( Portg, Pg, 26 )
	#define TRIO_DEFINE_PG27()	TRIO_DEFINE_PIN( Portg, Pg, 27 )
	#define TRIO_DEFINE_PG28()	TRIO_DEFINE_PIN( Portg, Pg, 28 )
	#define TRIO_DEFINE_PG29()	TRIO_DEFINE_PIN( Portg, Pg, 29 )
	#define TRIO_DEFINE_PG30()	TRIO_DEFINE_PIN( Portg, Pg, 30 )
	#define TRIO_DEFINE_PG31()	TRIO_DEFINE_PIN( Portg, Pg, 31 )
	
	//Port H pins
	#define TRIO_DEFINE_PH0()	TRIO_DEFINE_PIN( Porth, Ph, 0 )
	#define TRIO_DEFINE_PH1() 	TRIO_DEFINE_PIN( Porth, Ph, 1  )
	#define TRIO_DEFINE_PH2() 	TRIO_DEFINE_PIN( Porth, Ph, 2  )
	#define TRIO_DEFINE_PH3() 	TRIO_DEFINE_PIN( Porth, Ph, 3  )
	#define TRIO_DEFINE_PH4() 	TRIO_DEFINE_PIN( Porth, Ph, 4  )
	#define TRIO_DEFINE_PH5() 	TRIO_DEFINE_PIN( Porth, Ph, 5  )
	#define TRIO_DEFINE_PH6() 	TRIO_DEFINE_PIN( Porth, Ph, 6  )
	#define TRIO_DEFINE_PH7() 	TRIO_DEFINE_PIN( Porth, Ph, 7  )
	#define TRIO_DEFINE_PH8() 	TRIO_DEFINE_PIN( Porth, Ph, 8  )
	#define TRIO_DEFINE_PH9() 	TRIO_DEFINE_PIN( Porth, Ph, 9  )
	#define TRIO_DEFINE_PH10()	TRIO_DEFINE_PIN( Porth, Ph, 10 )
	#define TRIO_DEFINE_PH11()	TRIO_DEFINE_PIN( Porth, Ph, 11 )
	#define TRIO_DEFINE_PH12()	TRIO_DEFINE_PIN( Porth, Ph, 12 )
	#define TRIO_DEFINE_PH13()	TRIO_DEFINE_PIN( Porth, Ph, 13 )
	#define TRIO_DEFINE_PH14()	TRIO_DEFINE_PIN( Porth, Ph, 14 )
	#define TRIO_DEFINE_PH15()	TRIO_DEFINE_PIN( Porth, Ph, 15 )
	#define TRIO_DEFINE_PH16()	TRIO_DEFINE_PIN( Porth, Ph, 16 )
	#define TRIO_DEFINE_PH17()	TRIO_DEFINE_PIN( Porth, Ph, 17 )
	#define TRIO_DEFINE_PH18()	TRIO_DEFINE_PIN( Porth, Ph, 18 )
	#define TRIO_DEFINE_PH19()	TRIO_DEFINE_PIN( Porth, Ph, 19 )
	#define TRIO_DEFINE_PH20()	TRIO_DEFINE_PIN( Porth, Ph, 20 )
	#define TRIO_DEFINE_PH21()	TRIO_DEFINE_PIN( Porth, Ph, 21 )
	#define TRIO_DEFINE_PH22()	TRIO_DEFINE_PIN( Porth, Ph, 22 )
	#define TRIO_DEFINE_PH23()	TRIO_DEFINE_PIN( Porth, Ph, 23 )
	#define TRIO_DEFINE_PH24()	TRIO_DEFINE_PIN( Porth, Ph, 24 )
	#define TRIO_DEFINE_PH25()	TRIO_DEFINE_PIN( Porth, Ph, 25 )
	#define TRIO_DEFINE_PH26()	TRIO_DEFINE_PIN( Porth, Ph, 26 )
	#define TRIO_DEFINE_PH27()	TRIO_DEFINE_PIN( Porth, Ph, 27 )
	#define TRIO_DEFINE_PH28()	TRIO_DEFINE_PIN( Porth, Ph, 28 )
	#define TRIO_DEFINE_PH29()	TRIO_DEFINE_PIN( Porth, Ph, 29 )
	#define TRIO_DEFINE_PH30()	TRIO_DEFINE_PIN( Porth, Ph, 30 )
	#define TRIO_DEFINE_PH31()	TRIO_DEFINE_PIN( Porth, Ph, 31 )
	
	//Port I pins
	#define TRIO_DEFINE_PI0()	TRIO_DEFINE_PIN( Porti, Pi, 0  )
	#define TRIO_DEFINE_PI1()	TRIO_DEFINE_PIN( Porti, Pi, 1  )
	#define TRIO_DEFINE_PI2() 	TRIO_DEFINE_PIN( Porti, Pi, 2  )
	#define TRIO_DEFINE_PI3() 	TRIO_DEFINE_PIN( Porti, Pi, 3  )
	#define TRIO_DEFINE_PI4() 	TRIO_DEFINE_PIN( Porti, Pi, 4  )
	#define TRIO_DEFINE_PI5() 	TRIO_DEFINE_PIN( Porti, Pi, 5  )
	#define TRIO_DEFINE_PI6() 	TRIO_DEFINE_PIN( Porti, Pi, 6  )
	#define TRIO_DEFINE_PI7() 	TRIO_DEFINE_PIN( Porti, Pi, 7  )
	#define TRIO_DEFINE_PI8() 	TRIO_DEFINE_PIN( Porti, Pi, 8  )
	#define TRIO_DEFINE_PI9() 	TRIO_DEFINE_PIN( Porti, Pi, 9  )
	#define TRIO_DEFINE_PI10()	TRIO_DEFINE_PIN( Porti, Pi, 10 )
	#define TRIO_DEFINE_PI11()	TRIO_DEFINE_PIN( Porti, Pi, 11 )
	#define TRIO_DEFINE_PI12()	TRIO_DEFINE_PIN( Porti, Pi, 12 )
	#define TRIO_DEFINE_PI13()	TRIO_DEFINE_PIN( Porti, Pi, 13 )
	#define TRIO_DEFINE_PI14()	TRIO_DEFINE_PIN( Porti, Pi, 14 )
	#define TRIO_DEFINE_PI15()	TRIO_DEFINE_PIN( Porti, Pi, 15 )
	#define TRIO_DEFINE_PI16()	TRIO_DEFINE_PIN( Porti, Pi, 16 )
	#define TRIO_DEFINE_PI17()	TRIO_DEFINE_PIN( Porti, Pi, 17 )
	#define TRIO_DEFINE_PI18()	TRIO_DEFINE_PIN( Porti, Pi, 18 )
	#define TRIO_DEFINE_PI19()	TRIO_DEFINE_PIN( Porti, Pi, 19 )
	#define TRIO_DEFINE_PI20()	TRIO_DEFINE_PIN( Porti, Pi, 20 )
	#define TRIO_DEFINE_PI21()	TRIO_DEFINE_PIN( Porti, Pi, 21 )
	#define TRIO_DEFINE_PI22()	TRIO_DEFINE_PIN( Porti, Pi, 22 )
	#define TRIO_DEFINE_PI23()	TRIO_DEFINE_PIN( Porti, Pi, 23 )
	#define TRIO_DEFINE_PI24()	TRIO_DEFINE_PIN( Porti, Pi, 24 )
	#define TRIO_DEFINE_PI25()	TRIO_DEFINE_PIN( Porti, Pi, 25 )
	#define TRIO_DEFINE_PI26()	TRIO_DEFINE_PIN( Porti, Pi, 26 )
	#define TRIO_DEFINE_PI27()	TRIO_DEFINE_PIN( Porti, Pi, 27 )
	#define TRIO_DEFINE_PI28()	TRIO_DEFINE_PIN( Porti, Pi, 28 )
	#define TRIO_DEFINE_PI29()	TRIO_DEFINE_PIN( Porti, Pi, 29 )
	#define TRIO_DEFINE_PI30()	TRIO_DEFINE_PIN( Porti, Pi, 30 )
	#define TRIO_DEFINE_PI31()	TRIO_DEFINE_PIN( Porti, Pi, 31 )
	
	//Port J pins
	#define TRIO_DEFINE_PJ0()	TRIO_DEFINE_PIN( Portj, Pj, 0  )
	#define TRIO_DEFINE_PJ1()	TRIO_DEFINE_PIN( Portj, Pj, 1  )
	#define TRIO_DEFINE_PJ2()	TRIO_DEFINE_PIN( Portj, Pj, 2  )
	#define TRIO_DEFINE_PJ3() 	TRIO_DEFINE_PIN( Portj, Pj, 3  )
	#define TRIO_DEFINE_PJ4() 	TRIO_DEFINE_PIN( Portj, Pj, 4  )
	#define TRIO_DEFINE_PJ5() 	TRIO_DEFINE_PIN( Portj, Pj, 5  )
	#define TRIO_DEFINE_PJ6() 	TRIO_DEFINE_PIN( Portj, Pj, 6  )
	#define TRIO_DEFINE_PJ7() 	TRIO_DEFINE_PIN( Portj, Pj, 7  )
	#define TRIO_DEFINE_PJ8() 	TRIO_DEFINE_PIN( Portj, Pj, 8  )
	#define TRIO_DEFINE_PJ9() 	TRIO_DEFINE_PIN( Portj, Pj, 9  )
	#define TRIO_DEFINE_PJ10()	TRIO_DEFINE_PIN( Portj, Pj, 10 )
	#define TRIO_DEFINE_PJ11()	TRIO_DEFINE_PIN( Portj, Pj, 11 )
	#define TRIO_DEFINE_PJ12()	TRIO_DEFINE_PIN( Portj, Pj, 12 )
	#define TRIO_DEFINE_PJ13()	TRIO_DEFINE_PIN( Portj, Pj, 13 )
	#define TRIO_DEFINE_PJ14()	TRIO_DEFINE_PIN( Portj, Pj, 14 )
	#define TRIO_DEFINE_PJ15()	TRIO_DEFINE_PIN( Portj, Pj, 15 )
	#define TRIO_DEFINE_PJ16()	TRIO_DEFINE_PIN( Portj, Pj, 16 )
	#define TRIO_DEFINE_PJ17()	TRIO_DEFINE_PIN( Portj, Pj, 17 )
	#define TRIO_DEFINE_PJ18()	TRIO_DEFINE_PIN( Portj, Pj, 18 )
	#define TRIO_DEFINE_PJ19()	TRIO_DEFINE_PIN( Portj, Pj, 19 )
	#define TRIO_DEFINE_PJ20()	TRIO_DEFINE_PIN( Portj, Pj, 20 )
	#define TRIO_DEFINE_PJ21()	TRIO_DEFINE_PIN( Portj, Pj, 21 )
	#define TRIO_DEFINE_PJ22()	TRIO_DEFINE_PIN( Portj, Pj, 22 )
	#define TRIO_DEFINE_PJ23()	TRIO_DEFINE_PIN( Portj, Pj, 23 )
	#define TRIO_DEFINE_PJ24()	TRIO_DEFINE_PIN( Portj, Pj, 24 )
	#define TRIO_DEFINE_PJ25()	TRIO_DEFINE_PIN( Portj, Pj, 25 )
	#define TRIO_DEFINE_PJ26()	TRIO_DEFINE_PIN( Portj, Pj, 26 )
	#define TRIO_DEFINE_PJ27()	TRIO_DEFINE_PIN( Portj, Pj, 27 )
	#define TRIO_DEFINE_PJ28()	TRIO_DEFINE_PIN( Portj, Pj, 28 )
	#define TRIO_DEFINE_PJ29()	TRIO_DEFINE_PIN( Portj, Pj, 29 )
	#define TRIO_DEFINE_PJ30()	TRIO_DEFINE_PIN( Portj, Pj, 30 )
	#define TRIO_DEFINE_PJ31()	TRIO_DEFINE_PIN( Portj, Pj, 31 )
	
	//Port K pins
	#define TRIO_DEFINE_PK0()	TRIO_DEFINE_PIN( Portk, Pk, 0  )
	#define TRIO_DEFINE_PK1() 	TRIO_DEFINE_PIN( Portk, Pk, 1  )
	#define TRIO_DEFINE_PK2() 	TRIO_DEFINE_PIN( Portk, Pk, 2  )
	#define TRIO_DEFINE_PK3() 	TRIO_DEFINE_PIN( Portk, Pk, 3  )
	#define TRIO_DEFINE_PK4() 	TRIO_DEFINE_PIN( Portk, Pk, 4  )
	#define TRIO_DEFINE_PK5() 	TRIO_DEFINE_PIN( Portk, Pk, 5  )
	#define TRIO_DEFINE_PK6() 	TRIO_DEFINE_PIN( Portk, Pk, 6  )
	#define TRIO_DEFINE_PK7() 	TRIO_DEFINE_PIN( Portk, Pk, 7  )
	#define TRIO_DEFINE_PK8() 	TRIO_DEFINE_PIN( Portk, Pk, 8  )
	#define TRIO_DEFINE_PK9() 	TRIO_DEFINE_PIN( Portk, Pk, 9  )
	#define TRIO_DEFINE_PK10()	TRIO_DEFINE_PIN( Portk, Pk, 10 )
	#define TRIO_DEFINE_PK11()	TRIO_DEFINE_PIN( Portk, Pk, 11 )
	#define TRIO_DEFINE_PK12()	TRIO_DEFINE_PIN( Portk, Pk, 12 )
	#define TRIO_DEFINE_PK13()	TRIO_DEFINE_PIN( Portk, Pk, 13 )
	#define TRIO_DEFINE_PK14()	TRIO_DEFINE_PIN( Portk, Pk, 14 )
	#define TRIO_DEFINE_PK15()	TRIO_DEFINE_PIN( Portk, Pk, 15 )
	#define TRIO_DEFINE_PK16()	TRIO_DEFINE_PIN( Portk, Pk, 16 )
	#define TRIO_DEFINE_PK17()	TRIO_DEFINE_PIN( Portk, Pk, 17 )
	#define TRIO_DEFINE_PK18()	TRIO_DEFINE_PIN( Portk, Pk, 18 )
	#define TRIO_DEFINE_PK19()	TRIO_DEFINE_PIN( Portk, Pk, 19 )
	#define TRIO_DEFINE_PK20()	TRIO_DEFINE_PIN( Portk, Pk, 20 )
	#define TRIO_DEFINE_PK21()	TRIO_DEFINE_PIN( Portk, Pk, 21 )
	#define TRIO_DEFINE_PK22()	TRIO_DEFINE_PIN( Portk, Pk, 22 )
	#define TRIO_DEFINE_PK23()	TRIO_DEFINE_PIN( Portk, Pk, 23 )
	#define TRIO_DEFINE_PK24()	TRIO_DEFINE_PIN( Portk, Pk, 24 )
	#define TRIO_DEFINE_PK25()	TRIO_DEFINE_PIN( Portk, Pk, 25 )
	#define TRIO_DEFINE_PK26()	TRIO_DEFINE_PIN( Portk, Pk, 26 )
	#define TRIO_DEFINE_PK27()	TRIO_DEFINE_PIN( Portk, Pk, 27 )
	#define TRIO_DEFINE_PK28()	TRIO_DEFINE_PIN( Portk, Pk, 28 )
	#define TRIO_DEFINE_PK29()	TRIO_DEFINE_PIN( Portk, Pk, 29 )
	#define TRIO_DEFINE_PK30()	TRIO_DEFINE_PIN( Portk, Pk, 30 )
	#define TRIO_DEFINE_PK31()	TRIO_DEFINE_PIN( Portk, Pk, 31 )
	
	//Port Q pins
	#define TRIO_DEFINE_PQ0()		TRIO_DEFINE_PIN( Portq, Pq, 0  )
	#define TRIO_DEFINE_PQ1() 	TRIO_DEFINE_PIN( Portq, Pq, 1  )
	#define TRIO_DEFINE_PQ2() 	TRIO_DEFINE_PIN( Portq, Pq, 2  )
	#define TRIO_DEFINE_PQ3() 	TRIO_DEFINE_PIN( Portq, Pq, 3  )
	#define TRIO_DEFINE_PQ4() 	TRIO_DEFINE_PIN( Portq, Pq, 4  )
	#define TRIO_DEFINE_PQ5() 	TRIO_DEFINE_PIN( Portq, Pq, 5  )
	#define TRIO_DEFINE_PQ6() 	TRIO_DEFINE_PIN( Portq, Pq, 6  )
	#define TRIO_DEFINE_PQ7() 	TRIO_DEFINE_PIN( Portq, Pq, 7  )
	#define TRIO_DEFINE_PQ8() 	TRIO_DEFINE_PIN( Portq, Pq, 8  )
	#define TRIO_DEFINE_PQ9() 	TRIO_DEFINE_PIN( Portq, Pq, 9  )
	#define TRIO_DEFINE_PQ10()	TRIO_DEFINE_PIN( Portq, Pq, 10 )
	#define TRIO_DEFINE_PQ11()	TRIO_DEFINE_PIN( Portq, Pq, 11 )
	#define TRIO_DEFINE_PQ12()	TRIO_DEFINE_PIN( Portq, Pq, 12 )
	#define TRIO_DEFINE_PQ13()	TRIO_DEFINE_PIN( Portq, Pq, 13 )
	#define TRIO_DEFINE_PQ14()	TRIO_DEFINE_PIN( Portq, Pq, 14 )
	#define TRIO_DEFINE_PQ15()	TRIO_DEFINE_PIN( Portq, Pq, 15 )
	#define TRIO_DEFINE_PQ16()	TRIO_DEFINE_PIN( Portq, Pq, 16 )
	#define TRIO_DEFINE_PQ17()	TRIO_DEFINE_PIN( Portq, Pq, 17 )
	#define TRIO_DEFINE_PQ18()	TRIO_DEFINE_PIN( Portq, Pq, 18 )
	#define TRIO_DEFINE_PQ19()	TRIO_DEFINE_PIN( Portq, Pq, 19 )
	#define TRIO_DEFINE_PQ20()	TRIO_DEFINE_PIN( Portq, Pq, 20 )
	#define TRIO_DEFINE_PQ21()	TRIO_DEFINE_PIN( Portq, Pq, 21 )
	#define TRIO_DEFINE_PQ22()	TRIO_DEFINE_PIN( Portq, Pq, 22 )
	#define TRIO_DEFINE_PQ23()	TRIO_DEFINE_PIN( Portq, Pq, 23 )
	#define TRIO_DEFINE_PQ24()	TRIO_DEFINE_PIN( Portq, Pq, 24 )
	#define TRIO_DEFINE_PQ25()	TRIO_DEFINE_PIN( Portq, Pq, 25 )
	#define TRIO_DEFINE_PQ26()	TRIO_DEFINE_PIN( Portq, Pq, 26 )
	#define TRIO_DEFINE_PQ27()	TRIO_DEFINE_PIN( Portq, Pq, 27 )
	#define TRIO_DEFINE_PQ28()	TRIO_DEFINE_PIN( Portq, Pq, 28 )
	#define TRIO_DEFINE_PQ29()	TRIO_DEFINE_PIN( Portq, Pq, 29 )
	#define TRIO_DEFINE_PQ30()	TRIO_DEFINE_PIN( Portq, Pq, 30 )
	#define TRIO_DEFINE_PQ31()	TRIO_DEFINE_PIN( Portq, Pq, 31 )
	
	//Port R pins
	#define TRIO_DEFINE_PR0()		TRIO_DEFINE_PIN( Portr, Pr, 0  )
	#define TRIO_DEFINE_PR1() 	TRIO_DEFINE_PIN( Portr, Pr, 1  )
	#define TRIO_DEFINE_PR2() 	TRIO_DEFINE_PIN( Portr, Pr, 2  )
	#define TRIO_DEFINE_PR3() 	TRIO_DEFINE_PIN( Portr, Pr, 3  )
	#define TRIO_DEFINE_PR4() 	TRIO_DEFINE_PIN( Portr, Pr, 4  )
	#define TRIO_DEFINE_PR5() 	TRIO_DEFINE_PIN( Portr, Pr, 5  )
	#define TRIO_DEFINE_PR6() 	TRIO_DEFINE_PIN( Portr, Pr, 6  )
	#define TRIO_DEFINE_PR7() 	TRIO_DEFINE_PIN( Portr, Pr, 7  )
	#define TRIO_DEFINE_PR8() 	TRIO_DEFINE_PIN( Portr, Pr, 8  )
	#define TRIO_DEFINE_PR9() 	TRIO_DEFINE_PIN( Portr, Pr, 9  )
	#define TRIO_DEFINE_PR10()	TRIO_DEFINE_PIN( Portr, Pr, 10 )
	#define TRIO_DEFINE_PR11()	TRIO_DEFINE_PIN( Portr, Pr, 11 )
	#define TRIO_DEFINE_PR12()	TRIO_DEFINE_PIN( Portr, Pr, 12 )
	#define TRIO_DEFINE_PR13()	TRIO_DEFINE_PIN( Portr, Pr, 13 )
	#define TRIO_DEFINE_PR14()	TRIO_DEFINE_PIN( Portr, Pr, 14 )
	#define TRIO_DEFINE_PR15()	TRIO_DEFINE_PIN( Portr, Pr, 15 )
	#define TRIO_DEFINE_PR16()	TRIO_DEFINE_PIN( Portr, Pr, 16 )
	#define TRIO_DEFINE_PR17()	TRIO_DEFINE_PIN( Portr, Pr, 17 )
	#define TRIO_DEFINE_PR18()	TRIO_DEFINE_PIN( Portr, Pr, 18 )
	#define TRIO_DEFINE_PR19()	TRIO_DEFINE_PIN( Portr, Pr, 19 )
	#define TRIO_DEFINE_PR20()	TRIO_DEFINE_PIN( Portr, Pr, 20 )
	#define TRIO_DEFINE_PR21()	TRIO_DEFINE_PIN( Portr, Pr, 21 )
	#define TRIO_DEFINE_PR22()	TRIO_DEFINE_PIN( Portr, Pr, 22 )
	#define TRIO_DEFINE_PR23()	TRIO_DEFINE_PIN( Portr, Pr, 23 )
	#define TRIO_DEFINE_PR24()	TRIO_DEFINE_PIN( Portr, Pr, 24 )
	#define TRIO_DEFINE_PR25()	TRIO_DEFINE_PIN( Portr, Pr, 25 )
	#define TRIO_DEFINE_PR26()	TRIO_DEFINE_PIN( Portr, Pr, 26 )
	#define TRIO_DEFINE_PR27()	TRIO_DEFINE_PIN( Portr, Pr, 27 )
	#define TRIO_DEFINE_PR28()	TRIO_DEFINE_PIN( Portr, Pr, 28 )
	#define TRIO_DEFINE_PR29()	TRIO_DEFINE_PIN( Portr, Pr, 29 )
	#define TRIO_DEFINE_PR30()	TRIO_DEFINE_PIN( Portr, Pr, 30 )
	#define TRIO_DEFINE_PR31()	TRIO_DEFINE_PIN( Portr, Pr, 31 )
}// namespace TRIO

#endif //TRIO_TPIN_HPP_
