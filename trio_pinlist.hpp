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

/*
* Usage: 
* using namespace TRIO;
* typedef PinList<Pd0, Pd1, Pd2, Pa2> MultipinPort;
* 
* Now you can use MultipinPort as usual port because it fully implements port API
* i.e. MultipinPort::Write(0x01); //will set Pd0 to 1, Pd1 = 0 Pd2 = 0 Pa2 = 0
* uint8_t portdata = MultipinPort::Read();
* 
* You can even pull pins from PinList by index and call TPin HWSpecific and extended API
* Example:
*  MultipinPort::GetPinByIndex<1>::Result::HWSpecificAPI::HWSpecificMethod(); */

#ifndef TRIO_PINLIST_HPP_
#define TRIO_PINLIST_HPP_

#ifndef TRIO_HPP_
    #error "Do not include this file directly. Use #include\"trio.hpp\" instead!"
#endif

#ifdef __GNUC__
	#define PL_INLINE_PRAGMA __attribute__((always_inline))
    #define PL_TEMPLATE_PRAGMA
#elif defined(__ICCSTM8__)  //IAR for STM8S    
    #define PL_INLINE_PRAGMA _Pragma("inline=forced")
    #define PL_TEMPLATE_PRAGMA _Pragma("basic_template_matching")
#else
	#define PL_INLINE_PRAGMA
    #define PL_TEMPLATE_PRAGMA
#endif

#include <stdint.h>
#include "loki/Typelist.h"
#include "boost_static_assert.h"
#include "trio_pinlist_utils.hpp"

namespace TRIO
{
    namespace Private
    {
        /*STM8S has slow bit by bit mapping because of accumulator architecture and IAR compiler issue.
        * Using GrOpt metrics for aggressive shifting whenever it possible*/
        struct SettingsForSTM8
        {
            //Shift distance. Don't optimize groups requiring shifting by more than this number of bits.
            static const uint8_t GrOptShiftMetric = 24;
            //Don't optimize groups smaller than this number of pins
            static const uint8_t GrOptSizeMetric = 2;
            //always shift group of 8/16/24 pins by 8/16/24 bits
            //This will kick in very powerful compiler optimizations!
            static const bool GrOptOmitMetricsForWholeByte = true;

            //Configuretor optimizations parameters//
            //Don't configure port by mask if group of pins is smaller then this value
            //Will call Pin::Configure:: methods instead.
            //This will produce BSET/BRES instructions for individual bit manipulation which is better for small groups or individual pins.
            static const uint8_t ConfGrSizeMetric = 3;//Fine tuned to give the best results
        };
        /*Not tested yet*/
        struct SettingsForSTM32
        {
            static const uint8_t GrOptShiftMetric = 5;
            static const uint8_t GrOptSizeMetric = 3;
            static const bool GrOptOmitMetricsForWholeByte = true;

            static const uint8_t ConfGrSizeMetric = 3; //NOT TUNED FINE! Experiments needed!
        };

        /*AVR has VERY good bit by bit mapping but shifting is not so good.
         GrOpt metrics allows only whole byte groups optimization */
        struct SettingsForAVR
        {
            static const uint8_t GrOptShiftMetric = 1;
            static const uint8_t GrOptSizeMetric = 32;
            static const bool GrOptOmitMetricsForWholeByte = true;

            static const uint8_t ConfGrSizeMetric = 3;//NOT TUNED FINE! Experiments needed!
        };
        /*Same as AVR*/
        struct SettingsForXMega
        {
            static const uint8_t GrOptShiftMetric = 1;
            static const uint8_t GrOptSizeMetric = 32;
            static const bool GrOptOmitMetricsForWholeByte = true;

            static const uint8_t ConfGrSizeMetric = 3;//NOT TUNED FINE! Experiments needed!
        };
        /*In general using moderate settings....*/
        struct SettingsForUnknown
        {
            static const uint8_t GrOptShiftMetric = 5;
            static const uint8_t GrOptSizeMetric = 3;
            static const bool GrOptOmitMetricsForWholeByte = true;

            static const uint8_t ConfGrSizeMetric = 3;//NOT TUNED FINE! Experiments needed!
        };

        #if defined(TRIO_STM8_PORT_)
            typedef SettingsForSTM8 Settings;
        #elif defined(TRIO_STM32_PORT_)
            typedef SettingsForSTM32 Settings;
        #elif defined(TRIO_AVR_PORT_)
            typedef SettingsForAVR Settings;
        #elif defined(TRIO_XMEGA_PORT_)
            typedef SettingsForXMega Settings;
        #else
            typedef SettingsForUnknown Settings;
        #endif


        //rfConfigurePort handles all configuration operations
		typedef enum
		{
			configOP_OutPushPull_Slow,
			configOP_OutPushPull_Medium,
			configOP_OutPushPull_Fast,
			configOP_OutOpenDrain_Slow,
			configOP_OutOpenDrain_Medium,
			configOP_OutOpenDrain_Fast,
			configOP_InFloating,
			configOP_InPullUp,
			configOP_InPullDown,
			configOP_InAnalog,
			configOP_EnableAF,
			configOP_DisableAF,
			configOP_EnableInterrupt,
			configOP_DisableInterrupt
		}ConfigOPS_enum;

		//rfWriteValueToPort handles all port write operations
		typedef enum{
			writeOP_Write,
			writeOP_Set,
			writeOP_Clear,
			writeOP_Toggle
		} WriteOPs_enum;
		
		//rfReadValueFromPort handles all port read operations
		typedef enum{
			readOP_Read,		//I think usually you want this. call Port::Read();
			readOP_ReadOutReg   //means call Port::ReadOutReg() instead of Port::Read();
		} ReadOPs_enum;

		/*
		MetaFunctions(mf_) are used in compile-time template magic. They are completely static and can be used in constant expressions.
		They may "return" type or value.
		
		Calling conventions:
		Each meta function has two TYPE input parameters(T and U) and one value input parameter N(uint32_t)
		
		Returning:
		typedef Result is defined to return a type
		static const value is used to return a value
		*/
        
		//=========Meta functions returning a type =====================        
		/*Return pin's port type. T meant to be TPinWrapper */
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfGetPort{ typedef typename T::Pin::TRIOInternals::Port Result; };
       
	    /*Return PinList in which pin is sitting. T meant to be TPinWrapper*/
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfGetPinList{ typedef typename T::PinList Result; };
        //=========Meta functions returning type END=============

        //=========Predicates for conditional operations=============
		/*Check if pin T belongs to port U. T meant to be TPinWrapper*/
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        struct IfBelongsToPort{ enum{ value = Loki::IsSameType<typename mfGetPort<T>::Result, U>::value }; };
        
		/*Return true if pin number in port is equal to pin number in PinList.
		It means we need no shifts and masking to map pin number form list to port. I called this "direct mapping".*/
		template < class T, class U=Loki::NullType, uint32_t N=0 >
        struct IfListToPortDirectMapped{ enum{ value = T::Pin::TRIOInternals::PortPinNumber == T::ListPinNumber }; };

        /*This is used for pin serial groups detection
        * T Pin
        * U ListOfPins on which operation is performed
        * Checks left neighbor
        * Return true if left PinList neighbor pin is also left Port neighbor */
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        class IfPinHasLeftSerialNeighbor
        {
            enum{ 
				myI = Loki::TL::IndexOf<U, T>::value 
				}; 
			enum{ 
				LeftNeighborI = (myI == 0)? myI : myI-1 
				};
            typedef typename Loki::TL::TypeAt< U, LeftNeighborI >::Result LeftNeighbor;
        
		public:
            enum{ 
				value = (T::Pin::TRIOInternals::PortPinNumber == LeftNeighbor::Pin::TRIOInternals::PortPinNumber+1 )
                       && (T::ListPinNumber == LeftNeighbor::ListPinNumber+1 ) 
				};
        };
		
		/*This is used for pin serial groups detection
        * T Pin
        * U ListOfPins on which operation is performed
        * Checks right neighbor
        * Return true if right PinList neighbor pin is also right Port neighbor */
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        class IfPinHasRightSerialNeighbor
        {
            enum{ 
				ListLen = Loki::TL::Length<U>::value 
				}; 
			enum{ 
				myI = Loki::TL::IndexOf<U, T>::value 
				};
            enum{ 
				RightNeighborI = (myI == ListLen-1)? myI : myI+1 
				};
            typedef typename Loki::TL::TypeAt< U, RightNeighborI >::Result	RightNeighbor;
        
		public:
            enum{ 
				value = (T::Pin::TRIOInternals::PortPinNumber == RightNeighbor::Pin::TRIOInternals::PortPinNumber-1 )
                       && (T::ListPinNumber == RightNeighbor::ListPinNumber-1 ) 
				};
        };

        /*This is used for pin serial groups detection
        * T Pin
        * U ListOfPins on which operation is performed		 
        * Return true if left OR right PinList neighbor pins is in serial group
		* i.e. pin is participating in serial group.*/
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        class IfPinParticipateInSomeSerialGroup
        {
            enum{ 
				LeftTest = IfPinHasLeftSerialNeighbor<T, U>::value 
				};
            enum{ 
				RightTest = IfPinHasRightSerialNeighbor<T, U>::value 
				};
        public:
            enum{ 
				value = LeftTest || RightTest 
				};
        };

        /*This is used for pin serial groups boundaries detection
        * T Pin
        * U ListOfPins on which operation is performed
        * Checks left neighbor, return true if left neighbor is serial
        * !!!Return true for zero element */
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        class IfPinHasLeftSerialNeighborExcept0
        {
            enum{ 
				ListLen = Loki::TL::Length<U>::value 
				}; 
			enum{ 
				myI = Loki::TL::IndexOf<U, T>::value 
				};
            enum{ 
				SkipLeftTest = (myI == 0) 
				};
            enum{ 
				LeftTest = IfPinHasLeftSerialNeighbor<T, U>::value || SkipLeftTest 
				};
        
		public:
            enum{ 
				value = LeftTest 
				};
        };

        /*Check GrOp metrics from Settings
        * T - Typelist of pins representing one serial group
        * Return true if group satisfies Settings GrOp metrics*/
        template < class T, class U=Loki::NullType, uint32_t N=0 >
        class IfGroupPassesGrOpMetrics
        {
            typedef typename Loki::TL::TypeAt< T, 0>::Result LeftmostPin; //get left pin from group
            static const uint8_t PortPinNumber = LeftmostPin::Pin::TRIOInternals::PortPinNumber;
			static const uint8_t ListPinNumber = LeftmostPin::ListPinNumber;
            static const uint8_t ShiftMetric = (PortPinNumber < ListPinNumber)  ? ListPinNumber-PortPinNumber
															                    : PortPinNumber-ListPinNumber;
            static const bool WholeByteOptIsPossible = (ShiftMetric % 8 == 0) && (Loki::TL::Length<T>::value % 8 == 0);
            static const bool ShiftMetricOK = ShiftMetric <= Settings::GrOptShiftMetric;
            static const bool SizeMetricOK = Loki::TL::Length<T>::value >= Settings::GrOptSizeMetric;
        
		public:
            static const bool value = (ShiftMetricOK && SizeMetricOK) || (WholeByteOptIsPossible && Settings::GrOptOmitMetricsForWholeByte);
        };
        //=========Predicates for conditional operations END=============

        //=========Meta functions returning value==============================
        //Get port mask for pin, T meant to be TPinWrapper
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfCalcPortMask
        {
            typedef typename mfGetPort<T>::Result::DataT	RetvalType; //we are returning same type as needed for pin's port
            static const RetvalType value = (RetvalType)1 << T::Pin::TRIOInternals::PortPinNumber;
        };
        
		//Get inversion mask. mask will be 0 for not inverted pins
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfCalcPortInversionMask
        {
            typedef typename mfGetPort<T>::Result::DataT	RetvalType; //we are returning same type as needed for pin's port
            static const RetvalType value = T::Pin::TRIOInternals::Inverted ? (RetvalType)1 << T::Pin::TRIOInternals::PortPinNumber 
																			: 0;
        };
		
        //Get PinList mask for pin, T meant to be TPinWrapper
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfCalcPinListMask
        {
            typedef typename mfGetPinList<T>::Result::DataT	RetvalType; //we are returning same type as needed for pin's pinlist
            static const RetvalType value = (RetvalType)1 << T::ListPinNumber;
        };
        
		/*Get inversion mask. T meant to be TPinWrapper
        This mask will be 0 for not inverted pins*/
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfCalcPinListInversionMask
        {
            typedef typename mfGetPort<T>::Result::DataT	RetvalType; //we are returning same type as needed for pin's port
            static const RetvalType value = T::Pin::TRIOInternals::Inverted ? (RetvalType)1 << T::ListPinNumber 
																			: 0;
        };
        
		/*Used to perform Bit by bit PinList to port pins projection. I.e. to map pin position in PinList to pin position in Port.
		T - pin. T meant to be TPinWrapper.
		U - unused. PinList is not passed as U because mfCalcPinListMask will acquire PinList from T using mfGetPinList<>
		N - PinList bit mask.
		If pin T is present in PinList bit mask N(corresponding bit is 1) then return port mask for pin T, otherwise return 0
		*/
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfPinListMaskToPortMask
        {
            typedef typename mfGetPort<T>::Result::DataT	RetvalType; //we are returning same type as needed for pin's port
            static const RetvalType value = (N & mfCalcPinListMask<T>::value)	? mfCalcPortMask<T>::value 
																				: 0;
        };
       
	    /*Used to perform Bit by bit port pins to PinList pint projection. I.e. to map pin position in Port to PinList pin position.
		T - pin. T meant to be TPinWrapper.
		U - unused. PinList is not passed as U because mfCalcPortMask will acquire PinList from T using mfGetPinList<>
		N - Port bit mask.
		If pin T is present in port bit mask N(corresponding bit is 1) then return PinList mask for pin T, otherwise return 0
		*/
        template< class T, class U=Loki::NullType, uint32_t N=0 >
        struct mfPortMaskToPinListMask
        {
            typedef typename mfGetPinList<T>::Result::DataT		RetvalType;//we are returning same type as needed for PinList
            static const RetvalType value = (N & mfCalcPortMask<T>::value)	? mfCalcPinListMask<T>::value 
																			: 0;
        };
		//===================Meta functions returning value END================================//

        /* LIST - list of pins participating in some groups
        *  RETLIST - used to hold return value between recursion iterations. I.e. used for internal purpose.
        *
        * Finds group boundaries, checks if group satisfies Settings::GrOp metrics if so - put this pins to Result
        * We can't use metaForEach_TList<> for this task because we have to remove detected group for second iteration.*/
        template< class LIST, class RETLIST=LIST >
        class FilterPinsUsingGrOpMetrics
        {
            typedef typename Utils::CopyUntil< LIST, IfPinHasLeftSerialNeighborExcept0, LIST >::Result	GroupOfSerialPins;
            static const bool GrOpMetricsIsOk = IfGroupPassesGrOpMetrics< GroupOfSerialPins >::value;
            typedef typename Utils::List1MinusList2< LIST, GroupOfSerialPins >::Result		ListMinusGroup;
            typedef typename Utils::List1MinusList2< RETLIST, GroupOfSerialPins >::Result	RETLISTMinusGroup;
        
		public:
            typedef typename Loki::Select< GrOpMetricsIsOk,
                typename FilterPinsUsingGrOpMetrics< ListMinusGroup, RETLIST >::Result, //if metrics is ok then left group in retlist
                typename FilterPinsUsingGrOpMetrics< ListMinusGroup, RETLISTMinusGroup >::Result >::Result Result; //otherwise delete group from retlist
        };
        
		template< class RETLIST >
        struct FilterPinsUsingGrOpMetrics< Loki::NullType, RETLIST >
		{ 
			typedef RETLIST Result; 
		};

        /*Most of compile time magic happens here!
        * After all this magic we will know pins for DirectMapper, SerialGroupsProcessor and BitByBitMapper
        * PORTMASK - port mask for PinsList list
        * INVMASK will take into account inverted pins if they are present in PinsList*/
        template< class PinsList, typename PortDataT >
        class PinsAndMasksForProcessing
        {
            typedef typename Utils::CopyIf< PinsList, IfListToPortDirectMapped >::Result			DirectlyMappedPins;
            typedef typename Utils::List1MinusList2< PinsList, DirectlyMappedPins >::Result			NonDirectlyMappedPins;
            typedef typename Utils::CopyIf< NonDirectlyMappedPins,
                             IfPinParticipateInSomeSerialGroup, NonDirectlyMappedPins >::Result		PinsParticipatingInGroups;

        public:
            //directly mapped pins
            typedef DirectlyMappedPins DirectMapperPins;
            //Filter serial group pins using Settings::GrOp metrics. Only groups satisfiyng metrics will go to SerialGroupsProcessor
            typedef typename FilterPinsUsingGrOpMetrics< PinsParticipatingInGroups >::Result	SerialGroupsProcessorPins;
            //Rest of pins will need bit by bit mapping
			typedef typename Utils::List1MinusList2<NonDirectlyMappedPins, SerialGroupsProcessorPins>::Result	BitByBitMapperPins;

            static const PortDataT PORTMASK = Utils::metaForEach_bitOr< PinsList, mfCalcPortMask >::value;
            static const PortDataT INVMASK =  Utils::metaForEach_bitOr< PinsList, mfCalcPortInversionMask >::value;
        };

        static const bool GrProc_WeAreWriting = true;
        static const bool GrProc_WeAreReading = false;
        
		/*LIST: list of pins participating in some serial group(we don't know yet one or more groups are there)
        * recursively break this pins into groups and process each group.
        * OP_TYPE is a flag to distinguish reading and writing, this affects shifting direction*/
		template< class LIST, bool OP_TYPE >
		struct SerialGroupsProcessor
		{
			PL_TEMPLATE_PRAGMA
			template< class RtT, class RtU >
			PL_INLINE_PRAGMA
			static void Run(const RtT n, RtU &result)	//output parameter result
			{
				//find group boundaries
				typedef typename Utils::CopyUntil<LIST, IfPinHasLeftSerialNeighborExcept0, LIST>::Result	GroupOfSerialPins;
				BOOST_STATIC_ASSERT( (Loki::TL::Length<GroupOfSerialPins>::value > 1) );

				//group found, so process it
				typedef typename Loki::TL::TypeAt< GroupOfSerialPins, 0>::Result	LeftmostPin;
				const uint8_t PortPinNumber = LeftmostPin::Pin::TRIOInternals::PortPinNumber;
				const uint8_t ListPinNumber = LeftmostPin::ListPinNumber;

                if( OP_TYPE == GrProc_WeAreWriting )
                {
                    //get exact return type from metafunction to prevent compiler warning
                    const typename mfCalcPinListMask<LeftmostPin>::RetvalType MASK = Utils::metaForEach_bitOr<GroupOfSerialPins, mfCalcPinListMask>::value;
                    const RtU MaskedWriteVal = n & MASK;
                    result |= ( PortPinNumber < ListPinNumber )? MaskedWriteVal >> (ListPinNumber-PortPinNumber)
                                                               : MaskedWriteVal << (PortPinNumber-ListPinNumber);
                }
                else
                {
                    //get exact return type from metafunction to prevent compiler warning
                    const typename mfCalcPortMask<LeftmostPin>::RetvalType MASK = Utils::metaForEach_bitOr<GroupOfSerialPins, mfCalcPortMask>::value;
                    const RtU MaskedPortval = n & MASK;
                    result |= ( PortPinNumber < ListPinNumber )? MaskedPortval << (ListPinNumber-PortPinNumber)
                                                               : MaskedPortval >> (PortPinNumber-ListPinNumber);
                }
				
				//throw away processed pins for next recursion
				typedef typename Utils::List1MinusList2<LIST, GroupOfSerialPins>::Result		NextRecursionPinList;
				//Next iteration(recursion)
				SerialGroupsProcessor<NextRecursionPinList, OP_TYPE>::Run( n, result );
			}
		};
		
		template< bool OP_TYPE >
		struct SerialGroupsProcessor< Loki::NullType, OP_TYPE >
        {
            PL_TEMPLATE_PRAGMA
			template< class RtT, class RtU >
			PL_INLINE_PRAGMA
			static void Run(const RtT n, RtU &result){ }
		};


		//===================Runtime functions. Use it with IO::Private::Utils::runForEach<> algorithm=================
		
        //Bit by bit value to port projection
        template< class T, class U=Loki::NullType, uint32_t N=0, uint32_t M=0 >
        struct rfMapPinListValueToPort
        {
            //Out parameter is BAD, but this is the price we have to pay for having generic ForEach algorithm
            PL_TEMPLATE_PRAGMA
            template< class RtT, class RtV >
            PL_INLINE_PRAGMA
            static void RunIO(const RtT valFromPinlistWrite, RtV &outWritingToPort) //IO - 1Input, 1Output params
            {
				if( valFromPinlistWrite & mfCalcPinListMask<T>::value )
					outWritingToPort |= mfCalcPortMask<T>::value;
            }
        };
        //Bit by bit port to PinList value projection
        template< class PIN, class U=Loki::NullType, uint32_t N=0, uint32_t M=0 >
        struct rfMapPortToPinListValue
        {
            //Out parameter is BAD, but this is the price we have to pay for having generic ForEach algorithm
            PL_TEMPLATE_PRAGMA
            template< class RtT, class RtV >
            PL_INLINE_PRAGMA
            static void RunIO(const RtT valFromPort, RtV &outToPinlist)		//IO - 1Input, 1Output params
            {
				if( valFromPort & mfCalcPortMask<PIN>::value )
					outToPinlist |= mfCalcPinListMask<PIN>::value;
            }
        };

        /*T Port
        * U List of pins wrapped in TPinWrapper
        * OP_TYPE write operation type
        * Write value to port*/
        template< class T, class U=Loki::NullType, uint32_t OP_TYPE=writeOP_Write, uint32_t M=0 >
        class rfWriteValueToPort
        {        
            typedef typename T::DataT PortDataT;
            typedef typename Utils::CopyIf<U, IfBelongsToPort, T>::Result		AllPinsOfThisPort;
            typedef PinsAndMasksForProcessing< AllPinsOfThisPort, PortDataT >	PinsForProcessing;

        public:
            PL_TEMPLATE_PRAGMA
            template< class RtT >
			PL_INLINE_PRAGMA
            static void RunI(const RtT ValToWrite) //I - 1Input params
            {
                PortDataT GonnaWriteToPort = 0;

				//Process direct mapped pins
                const RtT DIRECTMAPPED_VALMASK = Utils::metaForEach_bitOr<typename PinsForProcessing::DirectMapperPins, mfCalcPinListMask>::value;
                GonnaWriteToPort |= ValToWrite & DIRECTMAPPED_VALMASK;

                //process serial groups
                SerialGroupsProcessor<typename PinsForProcessing::SerialGroupsProcessorPins, GrProc_WeAreWriting>::Run( ValToWrite, GonnaWriteToPort );

                //Process rest of pins using bit by bit mapping
                Utils::runForEach<typename PinsForProcessing::BitByBitMapperPins, rfMapPinListValueToPort>::RunIO( ValToWrite, GonnaWriteToPort );

                //Take into account inverted pins
                GonnaWriteToPort ^= PinsForProcessing::INVMASK;

                switch( static_cast<WriteOPs_enum>(OP_TYPE) )
                {
                case writeOP_Write:
						//Whole port write optimization
						if( Utils::PopBits<PinsForProcessing::PORTMASK>::value == T::Width )
							T::Write( GonnaWriteToPort );
						else
							T::ClearAndSet( PinsForProcessing::PORTMASK, GonnaWriteToPort );
						break;
                case writeOP_Set:
                        T::Set( GonnaWriteToPort );
						break;
                case writeOP_Clear:
                        T::Clear( GonnaWriteToPort );
						break;
                case writeOP_Toggle:
                        T::Toggle( GonnaWriteToPort );
						break;
                }
            }//static void RunI(const RtT ValToWrite)
        };//struct rfWriteValueToPort

        /*T Port
        * U List of pins(all of PinList pins)
        * OP_TYPE read operation type
        * Read value from port and map it to pinlist. outPar1 used as a return value*/
        template< class T, class U=Loki::NullType, uint32_t OP_TYPE=readOP_Read, uint32_t M=0 >
        class rfReadValueFromPort
        {
            typedef typename T::DataT PortDataT;
            typedef typename Utils::CopyIf<U, IfBelongsToPort, T>::Result		AllPinsOfThisPort;
            typedef PinsAndMasksForProcessing<AllPinsOfThisPort, PortDataT>		PinsForProcessing;

        public:
            PL_TEMPLATE_PRAGMA
            template< class RtU >
			PL_INLINE_PRAGMA
            static void RunO( RtU &outPar1 ) //O - 1output param
            {
                PortDataT ValueReadedFromPort;
                switch( static_cast<ReadOPs_enum>(OP_TYPE) )
                {                    
                    case readOP_Read:		ValueReadedFromPort = T::Read();
											break;
					case readOP_ReadOutReg: ValueReadedFromPort = T::ReadOutReg();
											break;
                }
                //Take into account inverted pins
                ValueReadedFromPort ^= PinsForProcessing::INVMASK;

                //Process direct mapped pins
                const PortDataT DIRECTMAPPED_PORTMASK = Utils::metaForEach_bitOr<typename PinsForProcessing::DirectMapperPins, mfCalcPortMask>::value;
                outPar1 |= ValueReadedFromPort & DIRECTMAPPED_PORTMASK;

                //process serial groups
                SerialGroupsProcessor<typename PinsForProcessing::SerialGroupsProcessorPins, GrProc_WeAreReading>::Run( ValueReadedFromPort, outPar1 );

                //Process rest of pins using bit by bit mapping
                Utils::runForEach<typename PinsForProcessing::BitByBitMapperPins, rfMapPortToPinListValue>::RunIO( ValueReadedFromPort, outPar1 );
            }//static void Run( RtU &outPar1 )
        };//struct rfReadValueFromPort


        /*Configures one pin by calling appropriate configure method(coded by ConfigOPType)
        * T Pin wrapped in TPinWrapper
        * U unused
        * N ConfigOPType - encode type of operation
        * M unused
        */
        template< class T, class U=Loki::NullType, uint32_t N=0, uint32_t M=0 >
        struct rfConfigureOnePin
		{
            PL_INLINE_PRAGMA
            static void Run(){}  
		};
        
		template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutPushPull_Slow, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutPushPull_Slow(); } 
		};
				
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutPushPull_Medium, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutPushPull_Medium(); } 
		};
        
		template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutPushPull_Fast, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutPushPull_Fast(); } 
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutOpenDrain_Slow, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutOpenDrain_Slow(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutOpenDrain_Medium, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutOpenDrain_Medium(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_OutOpenDrain_Fast, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::OutOpenDrain_Fast(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_InFloating, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::InFloating(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_InPullUp, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::InPullUp(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_InPullDown, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::InPullDown(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_InAnalog, M>
		{
            template< class RtT >
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::InAnalog(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_EnableAF, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::EnableAF(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_DisableAF, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::DisableAF(); }
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_EnableInterrupt, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){T::Pin::Configure::EnableInterrupt();}
		};
		
        template< class T, class U, uint32_t M >
        struct rfConfigureOnePin<T, U, configOP_DisableInterrupt, M>
		{
            PL_INLINE_PRAGMA
            static void Run(){ T::Pin::Configure::DisableInterrupt(); }
		};

        /*This will run rfConfigureOnePin runtime function for every pin in PINS list
         PINS - list of pins we need configure
         CONF_OP - configuration operation encoded in enum
        */
        template< class PINS, ConfigOPS_enum CONF_OP >
        struct ConfStrategy_IndividualPin
		{
            static void Configure()
            {
                Utils::runForEach<PINS, rfConfigureOnePin, Loki::NullType, static_cast<uint32_t>(CONF_OP)>::Run();
            }
		};

        /* Configure port by mask
         * CONF_OP - configuration operation encoded in enum
         * Using template specialization to avoid any runtime conditional operations in rfConfigurePort
         */
        template< class PORT, uint32_t PORTMASK, ConfigOPS_enum CONF_OP >
        struct ConfStrategy_ByMask
		{
			static void Configure(){}
		};
        //we should ignore 0 mask
        template< class PORT, ConfigOPS_enum CONF_OP >
        struct ConfStrategy_ByMask<PORT, 0, CONF_OP>
		{
			static void Configure(){}
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutPushPull_Slow>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutPushPull_Slow<PORTMASK>(); } 
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutPushPull_Medium>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutPushPull_Medium<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutPushPull_Fast>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutPushPull_Fast<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutOpenDrain_Slow>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutOpenDrain_Slow<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutOpenDrain_Medium>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutOpenDrain_Medium<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_OutOpenDrain_Fast>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template OutOpenDrain_Fast<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_InFloating>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template InFloating<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_InPullUp>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template InPullUp<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_InPullDown>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template InPullDown<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_InAnalog>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template InAnalog<PORTMASK>(); }
		};

		template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_EnableAF>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template EnableAF<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_DisableAF>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template DisableAF<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_EnableInterrupt>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template EnableInterrupt<PORTMASK>(); }
		};

        template< class PORT, uint32_t PORTMASK >
        struct ConfStrategy_ByMask<PORT, PORTMASK, configOP_DisableInterrupt>
		{
            PL_INLINE_PRAGMA
            static void Configure(){ PORT::Configure::template DisableInterrupt<PORTMASK>(); }
		};

        /* Entry point for port configuration
        *  Selects configuration strategy depending on Settings::ConfGrSizeMetric and calls it's Configure() method
        * T Port
        * U List of pins wrapped in TPinWrapper
        * N Mask for pins to configure(this is PinList mask and of course it will be mapped to port mask at compile time)
        * M Config operation(ConfigOPS_enum) */
        template< class T, class U=Loki::NullType, uint32_t N=0, uint32_t M=0 >
        class rfConfigurePort
        {
            static const ConfigOPS_enum ConfOp = static_cast<ConfigOPS_enum>(M);
            typedef typename Utils::CopyIf<U, IfBelongsToPort, T>::Result		AllPinsOfThisPort;
			
            static const uint8_t COUNT_OF_PINS_TO_CONFIGURE = Loki::TL::Length<AllPinsOfThisPort>::value;
            
			//map PinList mask to port mask
            static const typename T::DataT CONFMASK = Utils::metaForEach_bitOr<AllPinsOfThisPort, mfPinListMaskToPortMask, Loki::NullType, N>::value;

            //Compile time select config strategy depending on settings and count of pins to configure
            typedef typename Loki::Select< (COUNT_OF_PINS_TO_CONFIGURE >= Settings::ConfGrSizeMetric),
                                            ConfStrategy_ByMask<T, CONFMASK, ConfOp>,				//big groups of pins configure by mask
                                            ConfStrategy_IndividualPin<AllPinsOfThisPort, ConfOp>	//small groups by calling individual pin config methods
                                             >::Result		ConfigPortImplementation;
        public:
            PL_INLINE_PRAGMA
            static void Run( )
            {
				ConfigPortImplementation::Configure();	//Note: no need to pass any parameters here in this line
														//because ConfigPortImplementation is already instantiated with template parameters(see above)
            }//static void Run()
        };//struct rfConfigurePort
		//===================Runtime functions. Use it with runForEach<> algorithm  END=================


        //PinListImplementation should respect Port API interface
        template< class TPINLIST >
        class PinListImplementation
        {
            typedef typename Loki::TL::NoDuplicates< typename Utils::metaForEach_TList<TPINLIST, mfGetPort>::Result >::Result	 UniquePortsList;

        public:
            typedef typename Loki::Select<(Loki::TL::Length<TPINLIST>::value > 8),
						typename Loki::Select<(Loki::TL::Length<TPINLIST>::value > 16), uint32_t, uint16_t>::Result, uint8_t>::Result	DataT;

    		static const uint8_t Width = sizeof(DataT) * CHAR_BITS;

    		//Main runtime API======================
            static void Write(DataT value){ Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Write>::RunI( value ); }
            
			static DataT Read()
            {
                DataT RetVal = 0;
                Utils::runForEach<UniquePortsList, rfReadValueFromPort, TPINLIST, readOP_Read>::RunO( RetVal );
                return RetVal;
            }
            
			static DataT ReadOutReg()
            {
                DataT RetVal = 0;
                Utils::runForEach<UniquePortsList, rfReadValueFromPort, TPINLIST, readOP_ReadOutReg>::RunO( RetVal );
                return RetVal;
            }
            
			static void Set(DataT mask){	Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Set>::RunI( mask ); }
            static void Clear(DataT mask){	Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Clear>::RunI( mask ); }
            
			static void ClearAndSet(DataT clearMask, DataT setmask)
            {
				Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Clear>::RunI( clearMask );
                Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Set>::RunI( setmask );
            }
            
			static void Toggle(DataT mask){ Utils::runForEach<UniquePortsList, rfWriteValueToPort, TPINLIST, writeOP_Toggle>::RunI( mask ); }

            //Main compile time static API==================
            template<DataT value>
            static void Write(){ Write(value); }
            
			template<DataT value>
            static void Set(){ Set(value); }
            
			template<DataT value>
            static void Clear(){ Clear(value); }
           
		    template<DataT clearMask, DataT value>
            static void ClearAndSet(){ ClearAndSet(clearMask, value); }
            
			template<DataT value>
            static void Toggle(){ Toggle(value); }

            struct Configure
            {
                template<DataT MASK>
                static void OutPushPull_Slow(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutPushPull_Slow>::Run(); }
                template<DataT MASK>
                static void OutPushPull_Medium(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutPushPull_Medium >::Run(); }
                template<DataT MASK>
                static void OutPushPull_Fast(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutPushPull_Fast>::Run(); }
                template<DataT MASK>
                static void OutOpenDrain_Slow(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutOpenDrain_Slow>::Run(); }
                template<DataT MASK>
                static void OutOpenDrain_Medium(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutOpenDrain_Medium>::Run(); }
                template<DataT MASK>
                static void OutOpenDrain_Fast(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_OutOpenDrain_Fast>::Run(); }

                template<DataT MASK>
                static void InFloating(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_InFloating>::Run(); }
                template<DataT MASK>
                static void InPullUp(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_InPullUp>::Run(); }
                template<DataT MASK>
                static void InPullDown(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_InPullDown>::Run(); }
                template<DataT MASK>
                static void InAnalog(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_InAnalog>::Run(); }

                template<DataT MASK>
                static void EnableAF(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_EnableAF>::Run(); }
                template<DataT MASK>
                static void DisableAF(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_DisableAF>::Run(); }
                template<DataT MASK>
                static void EnableInterrupt(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_EnableInterrupt>::Run(); }
                template<DataT MASK>
                static void DisableInterrupt(){ Utils::runForEach<UniquePortsList, rfConfigurePort, TPINLIST, MASK, configOP_DisableInterrupt>::Run(); }
            };
            //=============Hardware specific API for maximum flexibility======================
            //TPin public interface will be extended by this hardware specific API
            //Some operations may need MASK to work, TPin will provide it for us
            template<DataT MASK=0>
            struct HWSpecificAPI{  };

            //Allows indexed access to individual pin( to access extended TPin API )
            template< int I >
            struct GetPinByIndex
			{ 
				typedef typename Loki::TL::TypeAt<TPINLIST, I>::Result::Pin		Result; 
			};        
		};//class PinListImplementation


        /*Each pin in PinList is stored with some additional information about the pin
		T - TPin class(pin class implementation, see TPin.hpp)
		U - PinList in which pin is sitting. So we can obtain PinList from any Pin. This is very useful!
		POSITION - Pin number in the PinList
		*/
        template<class T, class U, uint8_t POSITION>
        struct TPinWrapper
		{ 
			typedef T Pin; 
			typedef U PinList; 
			static const uint8_t ListPinNumber = POSITION;
		};

        //Generate list of wrappers with positions
        template<
            class PinListT,
            uint8_t Position,
            typename T1  = Loki::NullType, typename T2  = Loki::NullType, typename T3  = Loki::NullType,
            typename T4  = Loki::NullType, typename T5  = Loki::NullType, typename T6  = Loki::NullType,
            typename T7  = Loki::NullType, typename T8  = Loki::NullType, typename T9  = Loki::NullType,
            typename T10 = Loki::NullType, typename T11 = Loki::NullType, typename T12 = Loki::NullType,
            typename T13 = Loki::NullType, typename T14 = Loki::NullType, typename T15 = Loki::NullType,
            typename T16 = Loki::NullType, typename T17 = Loki::NullType, typename T18 = Loki::NullType,
			typename T19 = Loki::NullType, typename T20 = Loki::NullType, typename T21 = Loki::NullType,
			typename T22 = Loki::NullType, typename T23 = Loki::NullType, typename T24 = Loki::NullType,
			typename T25 = Loki::NullType, typename T26 = Loki::NullType, typename T27 = Loki::NullType,
			typename T28 = Loki::NullType, typename T29 = Loki::NullType, typename T30 = Loki::NullType,
			typename T31 = Loki::NullType, typename T32 = Loki::NullType, typename T33 = Loki::NullType >
        class MakeWrappedPinList
        {
            typedef typename MakeWrappedPinList<
				PinListT, Position + 1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16,
                T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33
            >::Result	TailResult;
			
			enum{ 
				PositionInList = Position 
				};        
		public:
            typedef Loki::Typelist< TPinWrapper<T1, PinListT, PositionInList>, TailResult>	Result;
        };//class MakeWrappedPinList
       
	    template< class PinListT, uint8_t Position >
        struct MakeWrappedPinList< PinListT, Position > 
		{ 
			typedef Loki::NullType Result; 
		};
    
	}//namespace Private

    template<
        typename T1  = Loki::NullType, typename T2  = Loki::NullType, typename T3  = Loki::NullType,
        typename T4  = Loki::NullType, typename T5  = Loki::NullType, typename T6  = Loki::NullType,
        typename T7  = Loki::NullType, typename T8  = Loki::NullType, typename T9  = Loki::NullType,
        typename T10 = Loki::NullType, typename T11 = Loki::NullType, typename T12 = Loki::NullType,
        typename T13 = Loki::NullType, typename T14 = Loki::NullType, typename T15 = Loki::NullType,
        typename T16 = Loki::NullType, typename T17 = Loki::NullType, typename T18 = Loki::NullType,
        typename T19 = Loki::NullType, typename T20 = Loki::NullType, typename T21 = Loki::NullType,
        typename T22 = Loki::NullType, typename T23 = Loki::NullType, typename T24 = Loki::NullType,
        typename T25 = Loki::NullType, typename T26 = Loki::NullType, typename T27 = Loki::NullType,
        typename T28 = Loki::NullType, typename T29 = Loki::NullType, typename T30 = Loki::NullType,
        typename T31 = Loki::NullType, typename T32 = Loki::NullType, typename T33 = Loki::NullType >
    struct PinList: public Private::PinListImplementation<
                    typename Private::MakeWrappedPinList<
                    //this is back reference to PinList. Tricky, but valid C++ ;) 
					//This needed because I want to get PinList from pin. See mfGetPinList meta function.
                    PinList<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17,
                            T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33>,

                      0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17,
                    T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33>::Result >
    {		
	};//struct PinList
}


#endif //PINLIST_HPP_

