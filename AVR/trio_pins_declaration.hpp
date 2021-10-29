//*****************************************************************************
// Author		: Trotskiy Vasily
// Date			: jan 2020
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
#ifndef TRIO_PINS_DECLARATION_HPP_
#define TRIO_PINS_DECLARATION_HPP_

#ifndef TRIO_HPP_
    #error "Do not include this file directly. Use #include\"trio.hpp\" instead!"
#endif

#ifndef TRIO_AVR_PORT_
	#error "This file related to AVR port! /AVR/trio_gpio_implementation.hpp should define TRIO_AVR_PORT_ macro"
#endif 

//just in case it was defined somewhere....
#undef TRIO_MCUSPECIFIC_PINDECL_FILE


//GNU C++ compiler
#if defined(__GNUG__) 
	//Select MCU specific pin definition file based on macro, internally defined by the compiler	
	#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
		#define TRIO_MCUSPECIFIC_PINDECL_FILE "pins_decl/attiny13.hpp"
	#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
		#define TRIO_MCUSPECIFIC_PINDECL_FILE "pins_decl/atmega8.hpp"
	#endif  //TODO: Add more AVR MCUs here....
#endif

namespace TRIO
{		
#if defined(TRIO_MCUSPECIFIC_PINDECL_FILE)
	#include TRIO_MCUSPECIFIC_PINDECL_FILE
#else  
/*No MCU specific pin declaration file is present! Just define all pins for all ports....*/
	#ifdef TRIO_HAS_PORTA
		TRIO_DEFINE_PA0()
		TRIO_DEFINE_PA1()
		TRIO_DEFINE_PA2()
		TRIO_DEFINE_PA3()
		TRIO_DEFINE_PA4()
		TRIO_DEFINE_PA5()
		TRIO_DEFINE_PA6()
		TRIO_DEFINE_PA7()
	#endif

	#ifdef TRIO_HAS_PORTB
		TRIO_DEFINE_PB0()
		TRIO_DEFINE_PB1()
		TRIO_DEFINE_PB2()
		TRIO_DEFINE_PB3()
		TRIO_DEFINE_PB4()
		TRIO_DEFINE_PB5()
		TRIO_DEFINE_PB6()
		TRIO_DEFINE_PB7()
	#endif

	#ifdef TRIO_HAS_PORTC
		TRIO_DEFINE_PC0()
		TRIO_DEFINE_PC1()
		TRIO_DEFINE_PC2()
		TRIO_DEFINE_PC3()
		TRIO_DEFINE_PC4()
		TRIO_DEFINE_PC5()
		TRIO_DEFINE_PC6()
		TRIO_DEFINE_PC7()
	#endif

	#ifdef TRIO_HAS_PORTD
		TRIO_DEFINE_PD0()
		TRIO_DEFINE_PD1()
		TRIO_DEFINE_PD2()
		TRIO_DEFINE_PD3()
		TRIO_DEFINE_PD4()
		TRIO_DEFINE_PD5()
		TRIO_DEFINE_PD6()
		TRIO_DEFINE_PD7()
	#endif

	#ifdef TRIO_HAS_PORTE
		TRIO_DEFINE_PE0()
		TRIO_DEFINE_PE1()
		TRIO_DEFINE_PE2()
		TRIO_DEFINE_PE3()
		TRIO_DEFINE_PE4()
		TRIO_DEFINE_PE5()
		TRIO_DEFINE_PE6()
		TRIO_DEFINE_PE7()
	#endif

	#ifdef TRIO_HAS_PORTF
		TRIO_DEFINE_PF0()
		TRIO_DEFINE_PF1()
		TRIO_DEFINE_PF2()
		TRIO_DEFINE_PF3()
		TRIO_DEFINE_PF4()
		TRIO_DEFINE_PF5()
		TRIO_DEFINE_PF6()
		TRIO_DEFINE_PF7()
	#endif

	#ifdef TRIO_HAS_PORTG
		TRIO_DEFINE_PG0()
		TRIO_DEFINE_PG1()
		TRIO_DEFINE_PG2()
		TRIO_DEFINE_PG3()
		TRIO_DEFINE_PG4()
		TRIO_DEFINE_PG5()
		TRIO_DEFINE_PG6()
		TRIO_DEFINE_PG7()
	#endif

	#ifdef TRIO_HAS_PORTH
		TRIO_DEFINE_PH0()
		TRIO_DEFINE_PH1()
		TRIO_DEFINE_PH2()
		TRIO_DEFINE_PH3()
		TRIO_DEFINE_PH4()
		TRIO_DEFINE_PH5()
		TRIO_DEFINE_PH6()
		TRIO_DEFINE_PH7()
	#endif
#endif //#if defined(TRIO_HAS_MCUSPECIFIC_PINDECL)
}//namespace TRIO

#endif /*TRIO_PINS_DECLARATION_HPP_*/
