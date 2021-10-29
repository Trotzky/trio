//*****************************************************************************
//stm8s103k.h
//
// Author		: Trotzky Vasily
// Date			: mar 2021
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
#ifndef IOCONF_STM8S103K_H_
#define IOCONF_STM8S103K_H_

/*	This is MCU specific pins definition file.
	Here you should define ONLY pins present in the MCU.	
*/

//Port A
//Port A pins present in MCU
TRIO_DEFINE_PA1()
TRIO_DEFINE_PA2()
TRIO_DEFINE_PA3()

//Port B
//Port B pins present in MCU
TRIO_DEFINE_PB0()
TRIO_DEFINE_PB1()
TRIO_DEFINE_PB2()
TRIO_DEFINE_PB3()
TRIO_DEFINE_PB4()
TRIO_DEFINE_PB5()
TRIO_DEFINE_PB6()
TRIO_DEFINE_PB7()

//Port C
TRIO_DEFINE_PC1()
TRIO_DEFINE_PC2()
TRIO_DEFINE_PC3()
TRIO_DEFINE_PC4()
TRIO_DEFINE_PC5()
TRIO_DEFINE_PC6()
TRIO_DEFINE_PC7()

//Port D
TRIO_DEFINE_PD0()
TRIO_DEFINE_PD1()
TRIO_DEFINE_PD2()
TRIO_DEFINE_PD3()
TRIO_DEFINE_PD4()
TRIO_DEFINE_PD5()
TRIO_DEFINE_PD6()
TRIO_DEFINE_PD7()

//Port E
TRIO_DEFINE_PE5()

//Port F          
TRIO_DEFINE_PF4()


//Port G                     NO


//Port H                     NO


//Port I                     NO


//Port J                     NO


//Port K                     NO


//Port Q                     NO


//Port R                     NO


#endif //IOCONF_STM8S003F3P6_H_