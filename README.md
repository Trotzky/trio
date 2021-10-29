# trio
C++ GPIO library with lots of compile time template magic

Add trio to your compiler include path. Example:
$PROJ_DIR$\lib\trio
also make sure to add second incluse path appropriate with your MCU architecture
stm8: 
$PROJ_DIR$\lib\trio\STM8
stm32: 
$PROJ_DIR$\lib\trio\STM32
AVR:
$PROJ_DIR$\lib\trio\AVR
XMega:
$PROJ_DIR$\lib\trio\XMega


Then you can specify exact part number of your MCU using (optional)
#define TRIO_MCUSPECIFIC_PINDECL_FILE "pins_decl/stm8s103k.hpp"
then
#include "trio.hpp"

Ofcourse you should define TRIO_MCUSPECIFIC_PINDECL_FILE before #include "trio.hpp"!