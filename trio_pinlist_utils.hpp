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
#ifndef TRIO_PINLIST_UTILS_HPP_
#define TRIO_PINLIST_UTILS_HPP_

#include <stdint.h>
#include "loki/Typelist.h"
#include "boost_static_assert.h"

namespace TRIO
{
    namespace Private
    {
        namespace Utils
        {
            /*//debug staff. To display values in compiler error messages
            template< uint32_t VAL >
            class DisplayInt_i;
			
            template< uint32_t VAL >
            class DisplayInt{ enum{ e = sizeof(DisplayInt_i<VAL>)}; };            
			template
        <
            typename TL1  = Loki::NullType, typename T2  = Loki::NullType, typename T3  = Loki::NullType,
            typename T4  = Loki::NullType, typename T5  = Loki::NullType, typename T6  = Loki::NullType,
            typename T7  = Loki::NullType, typename T8  = Loki::NullType, typename T9  = Loki::NullType,
            typename T10 = Loki::NullType, typename T11 = Loki::NullType, typename T12 = Loki::NullType,
            typename T13 = Loki::NullType, typename T14 = Loki::NullType, typename T15 = Loki::NullType,
            typename T16 = Loki::NullType, typename T17 = Loki::NullType, typename T18 = Loki::NullType,
            typename T19 = Loki::NullType, typename T20 = Loki::NullType, typename T21 = Loki::NullType,
            typename T22 = Loki::NullType, typename T23 = Loki::NullType, typename T24 = Loki::NullType,
            typename T25 = Loki::NullType, typename T26 = Loki::NullType, typename T27 = Loki::NullType,
            typename T28 = Loki::NullType, typename T29 = Loki::NullType, typename T30 = Loki::NullType,
            typename T31 = Loki::NullType, typename T32 = Loki::NullType, typename T33 = Loki::NullType
        >
            class DisplayTList_i;
            template< class List >
            class DisplayTList{ enum{ e = sizeof(DisplayTList_i<List>)}; };
            //debug staff END////////////// */

            //Compile time count set bits in x
            template<uint32_t x>
            class PopBits
            {
                static const uint32_t x1 = (x & 0x55555555) + ((x >> 1) & 0x55555555);
                static const uint32_t x2 = (x1 & 0x33333333) + ((x1 >> 2) & 0x33333333);
                static const uint32_t x3 = (x2 & 0x0f0f0f0f) + ((x2 >> 4) & 0x0f0f0f0f);
                static const uint32_t x4 = (x3 & 0x00ff00ff) + ((x3 >> 8) & 0x00ff00ff);
            public:
                static const uint32_t value = (x4 & 0x0000ffff) + ((x4 >> 16) & 0x0000ffff);
            };

            //======================Algorithms==========================

            /*Call Fn::Run() for each element in the LIST. Fn is a template class with defined public method Run()
			* First Fn template parameter will be list element. i.e. LIST[i] (pseudo code)
			* FnU, FnN, FnM template parameters are forwarded to Fn template parameters.
			* Run(param1, param2...) method parameters ara also forwarded to Fn::Run(param1, param2...) parameters            
            *
			* No return value, output parameter is used instead. We also don't care about Fn::Run return value(if any).
			* Why? Type of output parameter could be deduced. Type of return value could not (in C++2003).
			*/
			/*
			RunIIO - 2 input params, 1 output
			RunIO - 1 input, 1 output param 
			RunO - 1 output param
			...etc
			*/
            template< class LIST, template<class T,class U,uint32_t N,uint32_t M>class Fn, class FnU=Loki::NullType, uint32_t FnN=0, uint32_t FnM=0 >
            struct runForEach;
            
			template< template<class T,class U,uint32_t N,uint32_t M>class Fn, class FnU, uint32_t FnN, uint32_t FnM >
            class runForEach< Loki::NullType, Fn, FnU, FnN, FnM >
            {
            public:
                PL_TEMPLATE_PRAGMA
                template< class RtT, class RtU, class RtV >
                PL_INLINE_PRAGMA
                static void RunIIO(const RtT InPar1, const RtU InPar2, RtV &outPar){ }

                PL_TEMPLATE_PRAGMA
                template< class RtT, class RtV >
                PL_INLINE_PRAGMA
                static void RunIO(const RtT InPar1, RtV &outPar){ }

                PL_TEMPLATE_PRAGMA
                template< class RtU >
                PL_INLINE_PRAGMA
                static void RunO( RtU &OutPar1 ){} //one out param, no in

                PL_TEMPLATE_PRAGMA
				template< class RtT, class RtU >
                PL_INLINE_PRAGMA
				static void RunII(const RtT InPar1, const RtU InPar2){ } //2 IN params, NO OUT!

                PL_TEMPLATE_PRAGMA
                template< class RtT >
                PL_INLINE_PRAGMA
                static void RunI(const RtT InPar1){ }

                PL_INLINE_PRAGMA
                static void Run(){ }
            };
            
			template< class Head, class Tail, template<class T,class U,uint32_t N, uint32_t M>class Fn, class FnU, uint32_t FnN, uint32_t FnM >
            struct runForEach< Loki::Typelist<Head, Tail>, Fn, FnU, FnN, FnM >
            {
                PL_TEMPLATE_PRAGMA
                template< class RtT, class RtU, class RtV >
                PL_INLINE_PRAGMA
                static void RunIIO(const RtT InPar1, const RtU InPar2, RtV &outPar)
                {
                    Fn<Head, FnU, FnN, FnM>::RunIIO( InPar1, InPar2, outPar );
                    runForEach<Tail, Fn, FnU, FnN, FnM>::RunIIO( InPar1, InPar2, outPar );
                }

                PL_TEMPLATE_PRAGMA
                template< class RtT, class RtV >
                PL_INLINE_PRAGMA
                static void RunIO(const RtT InPar1, RtV &outPar)
                {
                    Fn<Head, FnU, FnN, FnM>::RunIO( InPar1, outPar );
                    runForEach<Tail, Fn, FnU, FnN, FnM>::RunIO( InPar1, outPar );
                }

                PL_TEMPLATE_PRAGMA
                template< class RtU >
                PL_INLINE_PRAGMA
                static void RunO( RtU &OutPar1 ) //one out param, no in
                {
                    Fn<Head, FnU, FnN, FnM>::RunO( OutPar1 );
                    runForEach<Tail, Fn, FnU, FnN, FnM>::RunO( OutPar1 );
                }

                PL_TEMPLATE_PRAGMA
                template< class RtT, class RtU >
                PL_INLINE_PRAGMA
                static void RunII(const RtT InPar1, const RtU InPar2) //two in params, no out
                {
                    Fn<Head, FnU, FnN, FnM>::RunII( InPar1, InPar2 );
                    runForEach<Tail, Fn, FnU, FnN, FnM>::RunII( InPar1, InPar2 );
                }

                PL_TEMPLATE_PRAGMA
                template< class RtT >
                PL_INLINE_PRAGMA
                static void RunI(const RtT InPar1)
                {
                    Fn<Head, FnU, FnN, FnM>::RunI(InPar1);
                    runForEach<Tail, Fn, FnU, FnN, FnM>::RunI(InPar1);
                }

                PL_INLINE_PRAGMA
                static void Run()
                {
                    Fn<Head, FnU, FnN, FnM>::Run();
                    runForEach<Tail, Fn, FnU, FnN, FnM>::Run();
                }
            };

            /*Call meta function(not a method!) MetaFn for each element in the LIST
			* Performs BITWISE OR on meta function return value on each iteration.
			*
            * MetaU and MetaN is optional parameters we can pass to MetaFunc if needed.
			* By a convention
			* Type of returned value is taken from typename MetaFn::RetvalType
			* MetaFn returns value by defining public static const value, accessable as MetaFn::value
			*
            *
            * pseudocode:
            *uint32_t intForEach_bitOr(List, MetaFn, MetaU, MetaN )
            *for(i=0; i<listLen; i++)
            *  result |= MetaFn( list[i], MetaU, MetaN );
            *return result;
            */
            template< class LIST, template<class T,class U,uint32_t N>class MetaFn, class MetaU=Loki::NullType, uint32_t MetaN=0 >
            struct metaForEach_bitOr;
            
			template< template<class T,class U,uint32_t N>class MetaFn, class MetaU, uint32_t MetaN >
            class metaForEach_bitOr< Loki::NullType, MetaFn, MetaU, MetaN >
            {
                typedef uint8_t RetvalType; //we can't get retval type from MetaFn, so use smallest possible
            
			public:
                static const RetvalType value = RetvalType(0);
            };
            
			template< class Head, class Tail, template<class T,class U,uint32_t N>class MetaFn, class MetaU, uint32_t MetaN >
            class metaForEach_bitOr< Loki::Typelist<Head, Tail>, MetaFn, MetaU, MetaN >
            {
                typedef typename MetaFn<Head, MetaU, MetaN>::RetvalType RetvalType;
            
			public:
                static const RetvalType value = MetaFn<Head, MetaU, MetaN>::value | metaForEach_bitOr<Tail, MetaFn, MetaU, MetaN>::value;
            };

            /*Call MetaFn for each element in the LIST
			* TList_ForEach returns list of MetaFn results( returned types )
			*
			* MetaFn should return type, accessable as typename MetaFn::Result			            
            * MetaU and MetaN is optional parameters we can pass to MetaFunc if needed.            
            *
            * pseudocode:
            *Typelist TList_ForEach(list, MetaFn, MetaU, MetaN )
            *for(i=0; i<listLen; i++)
            *  result.Append( MetaFn( list[i], MetaU, MetaN ));
            *return result;
            */
            template< class LIST, template<class T,class U,uint32_t N>class MetaFn, class MetaU=Loki::NullType, uint32_t MetaN=0 >
            struct metaForEach_TList;
           
		    template< template<class T,class U,uint32_t N>class MetaFn, class MetaU, uint32_t MetaN >
            struct metaForEach_TList< Loki::NullType, MetaFn, MetaU, MetaN >
			{ 
				typedef Loki::NullType Result; 
			};
            
			template< class Head, class Tail, template<class T,class U,uint32_t N>class MetaFn, class MetaU, uint32_t MetaN  >
            struct metaForEach_TList< Loki::Typelist<Head, Tail>, MetaFn, MetaU, MetaN >
            {
                //Recursively fill Result Typelist with MetaFn results
				typedef Loki::Typelist< typename MetaFn<Head, MetaU, MetaN>::Result,
                                        typename metaForEach_TList<Tail, MetaFn, MetaU, MetaN>::Result > Result;
            };
			
            /*Call Predicate for each element in the LIST
            * Copy element to Result list only if Predicate returns true
            * PredU and PredN is optional parameters we can pass to Predicate if needed
            *
            * pseudocode:
            *TList CopyIf(List, Predicate, PredU, PredN )
            *for(i=0; i<ListLen; i++)
            *   if( Predicate( List[i], MetaU, MetaN ) == true )
            *       Result.Append( List[i] );
            *return Result;
            */
            template< class LIST, template<class T,class U,uint32_t N>class Predicate, class PredU=Loki::NullType, uint32_t PredN=0 >
            struct CopyIf;
            
			template< template<class T,class U,uint32_t N>class Predicate, class PredU, uint32_t PredN >
            struct CopyIf< Loki::NullType, Predicate, PredU, PredN >
			{ 
				typedef Loki::NullType Result;
			};
            
			template< class Head, class Tail, template<class T,class U,uint32_t N>class Predicate, class PredU, uint32_t PredN >
            struct CopyIf< Loki::Typelist<Head, Tail>, Predicate, PredU, PredN >
            {
                typedef typename Loki::Select< Predicate<Head, PredU, PredN>::value,
                    Loki::Typelist<Head, typename CopyIf<Tail, Predicate, PredU, PredN>::Result>, //copy only if predicate returns true
                    typename CopyIf<Tail, Predicate, PredU, PredN>::Result >::Result Result;
            };

            //Copy until predicate is true.
            //See CopyIf description. This is the same, but terminate on first predicate false return
            template< class LIST, template<class T,class U,uint32_t N>class Predicate, class PredU=Loki::NullType, uint32_t PredN=0 >
            struct CopyUntil;
            
			template< template<class T,class U,uint32_t N>class Predicate, class PredU, uint32_t PredN >
            struct CopyUntil< Loki::NullType, Predicate, PredU, PredN >
			{ 
				typedef Loki::NullType Result;
			};
			
            template< class Head, class Tail, template<class T,class U,uint32_t N>class Predicate, class PredU, uint32_t PredN >
            struct CopyUntil< Loki::Typelist<Head, Tail>, Predicate, PredU, PredN >
            {
                typedef typename Loki::Select< Predicate<Head, PredU, PredN>::value,
                    Loki::Typelist<Head, typename CopyUntil<Tail, Predicate, PredU, PredN>::Result>, //copy only if predicate returns true
                    typename CopyUntil<Loki::NullType, Predicate, PredU, PredN>::Result >::Result Result; //Jump to end of recursion on predicate false
            };

            //Remove from LIST1 all occurrences of types from LIST2
            template< class LIST1, class LIST2 > 
			struct List1MinusList2;
			
            template< class LIST1 > struct List1MinusList2<LIST1, Loki::NullType>
			{
				typedef LIST1 Result;
			};
            
			template< class LIST1, class Head, class Tail>
            struct List1MinusList2<LIST1, Loki::Typelist<Head, Tail> >
            {
                typedef typename List1MinusList2< typename Loki::TL::EraseAll<LIST1, Head>::Result, Tail >::Result Result;
            };
            //======================Algorithms END==========================
        }//namespace Utils
    }//namespace Private
}//namespace TRIO

#endif //TRIO_PINLIST_UTILS_HPP_

