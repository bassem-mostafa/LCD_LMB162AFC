// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifndef LCD_LMB162AFC_H_
    #define LCD_LMB162AFC_H_

    #ifdef __cplusplus
extern "C"
{
    #endif /* __cplusplus */

    // #############################################################################
    // #### Include(s) #############################################################
    // #############################################################################

    #include <stdint.h>

    // #############################################################################
    // #### Public Macro(s) ########################################################
    // #############################################################################

    #define LCD_LMB162AFC_NUMBER_OF_CHARACTERS 16
    #define LCD_LMB162AFC_NUMBER_OF_LINES      2

    #define LCD_LMB162AFC_CHARACTER_WIDTH      5
    #define LCD_LMB162AFC_CHARACTER_HEIGHT     8

    #define LCD_LMB162AFC_WIDTH                ( LCD_LMB162AFC_NUMBER_OF_CHARACTERS * LCD_LMB162AFC_CHARACTER_WIDTH ) // in pixels
    #define LCD_LMB162AFC_HEIGHT               ( LCD_LMB162AFC_NUMBER_OF_LINES * LCD_LMB162AFC_CHARACTER_HEIGHT )     // in pixels

    // #############################################################################
    // #### Public Type(s) #########################################################
    // #############################################################################

    typedef enum LCD_LMB162AFC_Status
    {
        LCD_LMB162AFC_Status_Success = 0,
        LCD_LMB162AFC_Status_ArgumentInvalid,
        LCD_LMB162AFC_Status_NotSupported,
        LCD_LMB162AFC_Status_Error,
        LCD_LMB162AFC_Status_Busy,
        LCD_LMB162AFC_Status_Timeout,
    } LCD_LMB162AFC_Status_t;

    typedef enum LCD_LMB162AFC_DisplayShift
    {
        LCD_LMB162AFC_DisplayShift_Off = 0,
        LCD_LMB162AFC_DisplayShift_On,
    } LCD_LMB162AFC_DisplayShift_t;

    typedef enum LCD_LMB162AFC_Counter
    {
        LCD_LMB162AFC_Counter_Decrement = 0,
        LCD_LMB162AFC_Counter_Increment,
    } LCD_LMB162AFC_Counter_t;

    typedef enum LCD_LMB162AFC_CursorBlink
    {
        LCD_LMB162AFC_CursorBlink_Off = 0,
        LCD_LMB162AFC_CursorBlink_On,
    } LCD_LMB162AFC_CursorBlink_t;

    typedef enum LCD_LMB162AFC_Cursor
    {
        LCD_LMB162AFC_Cursor_Off = 0,
        LCD_LMB162AFC_Cursor_On,
    } LCD_LMB162AFC_Cursor_t;

    typedef enum LCD_LMB162AFC_Display
    {
        LCD_LMB162AFC_Display_Off = 0,
        LCD_LMB162AFC_Display_On,
    } LCD_LMB162AFC_Display_t;

    typedef enum LCD_LMB162AFC_Shift
    {
        LCD_LMB162AFC_Shift_Cursor = 0,
        LCD_LMB162AFC_Shift_Display,
    } LCD_LMB162AFC_Shift_t;

    typedef enum LCD_LMB162AFC_ShiftDirection
    {
        LCD_LMB162AFC_ShiftDirection_Left = 0,
        LCD_LMB162AFC_ShiftDirection_Right,
    } LCD_LMB162AFC_ShiftDirection_t;

    typedef enum LCD_LMB162AFC_Font
    {
        LCD_LMB162AFC_Font_5x8 = 0,
        LCD_LMB162AFC_Font_5x11,
    } LCD_LMB162AFC_Font_t;

    typedef enum LCD_LMB162AFC_Mode
    {
        LCD_LMB162AFC_Mode_1_Line = 0,
        LCD_LMB162AFC_Mode_2_Line,
    } LCD_LMB162AFC_Mode_t;

    typedef enum LCD_LMB162AFC_Interface
    {
        LCD_LMB162AFC_Interface_4_bit = 0,
        LCD_LMB162AFC_Interface_8_bit,
    } LCD_LMB162AFC_Interface_t;

    typedef enum LCD_LMB162AFC_CGRAM
    {
        LCD_LMB162AFC_CGRAM_0 = 0,
        LCD_LMB162AFC_CGRAM_1,
        LCD_LMB162AFC_CGRAM_2,
        LCD_LMB162AFC_CGRAM_3,
        LCD_LMB162AFC_CGRAM_4,
        LCD_LMB162AFC_CGRAM_5,
        LCD_LMB162AFC_CGRAM_6,
        LCD_LMB162AFC_CGRAM_7,
        LCD_LMB162AFC_CGRAM_8,
        LCD_LMB162AFC_CGRAM_9,
        LCD_LMB162AFC_CGRAM_10,
        LCD_LMB162AFC_CGRAM_11,
        LCD_LMB162AFC_CGRAM_12,
        LCD_LMB162AFC_CGRAM_13,
        LCD_LMB162AFC_CGRAM_14,
        LCD_LMB162AFC_CGRAM_15,
        LCD_LMB162AFC_CGRAM_16,
        LCD_LMB162AFC_CGRAM_17,
        LCD_LMB162AFC_CGRAM_18,
        LCD_LMB162AFC_CGRAM_19,
        LCD_LMB162AFC_CGRAM_20,
        LCD_LMB162AFC_CGRAM_21,
        LCD_LMB162AFC_CGRAM_22,
        LCD_LMB162AFC_CGRAM_23,
        LCD_LMB162AFC_CGRAM_24,
        LCD_LMB162AFC_CGRAM_25,
        LCD_LMB162AFC_CGRAM_26,
        LCD_LMB162AFC_CGRAM_27,
        LCD_LMB162AFC_CGRAM_28,
        LCD_LMB162AFC_CGRAM_29,
        LCD_LMB162AFC_CGRAM_30,
        LCD_LMB162AFC_CGRAM_31,
        LCD_LMB162AFC_CGRAM_32,
        LCD_LMB162AFC_CGRAM_33,
        LCD_LMB162AFC_CGRAM_34,
        LCD_LMB162AFC_CGRAM_35,
        LCD_LMB162AFC_CGRAM_36,
        LCD_LMB162AFC_CGRAM_37,
        LCD_LMB162AFC_CGRAM_38,
        LCD_LMB162AFC_CGRAM_39,
        LCD_LMB162AFC_CGRAM_40,
        LCD_LMB162AFC_CGRAM_41,
        LCD_LMB162AFC_CGRAM_42,
        LCD_LMB162AFC_CGRAM_43,
        LCD_LMB162AFC_CGRAM_44,
        LCD_LMB162AFC_CGRAM_45,
        LCD_LMB162AFC_CGRAM_46,
        LCD_LMB162AFC_CGRAM_47,
        LCD_LMB162AFC_CGRAM_48,
        LCD_LMB162AFC_CGRAM_49,
        LCD_LMB162AFC_CGRAM_50,
        LCD_LMB162AFC_CGRAM_51,
        LCD_LMB162AFC_CGRAM_52,
        LCD_LMB162AFC_CGRAM_53,
        LCD_LMB162AFC_CGRAM_54,
        LCD_LMB162AFC_CGRAM_55,
        LCD_LMB162AFC_CGRAM_56,
        LCD_LMB162AFC_CGRAM_57,
        LCD_LMB162AFC_CGRAM_58,
        LCD_LMB162AFC_CGRAM_59,
        LCD_LMB162AFC_CGRAM_60,
        LCD_LMB162AFC_CGRAM_61,
        LCD_LMB162AFC_CGRAM_62,
        LCD_LMB162AFC_CGRAM_63,
    } LCD_LMB162AFC_CGRAM_t;

    typedef enum LCD_LMB162AFC_DDRAM
    {
        LCD_LMB162AFC_DDRAM_0 = 0,
        LCD_LMB162AFC_DDRAM_1,
        LCD_LMB162AFC_DDRAM_2,
        LCD_LMB162AFC_DDRAM_3,
        LCD_LMB162AFC_DDRAM_4,
        LCD_LMB162AFC_DDRAM_5,
        LCD_LMB162AFC_DDRAM_6,
        LCD_LMB162AFC_DDRAM_7,
        LCD_LMB162AFC_DDRAM_8,
        LCD_LMB162AFC_DDRAM_9,
        LCD_LMB162AFC_DDRAM_10,
        LCD_LMB162AFC_DDRAM_11,
        LCD_LMB162AFC_DDRAM_12,
        LCD_LMB162AFC_DDRAM_13,
        LCD_LMB162AFC_DDRAM_14,
        LCD_LMB162AFC_DDRAM_15,
        LCD_LMB162AFC_DDRAM_16,
        LCD_LMB162AFC_DDRAM_17,
        LCD_LMB162AFC_DDRAM_18,
        LCD_LMB162AFC_DDRAM_19,
        LCD_LMB162AFC_DDRAM_20,
        LCD_LMB162AFC_DDRAM_21,
        LCD_LMB162AFC_DDRAM_22,
        LCD_LMB162AFC_DDRAM_23,
        LCD_LMB162AFC_DDRAM_24,
        LCD_LMB162AFC_DDRAM_25,
        LCD_LMB162AFC_DDRAM_26,
        LCD_LMB162AFC_DDRAM_27,
        LCD_LMB162AFC_DDRAM_28,
        LCD_LMB162AFC_DDRAM_29,
        LCD_LMB162AFC_DDRAM_30,
        LCD_LMB162AFC_DDRAM_31,
        LCD_LMB162AFC_DDRAM_32,
        LCD_LMB162AFC_DDRAM_33,
        LCD_LMB162AFC_DDRAM_34,
        LCD_LMB162AFC_DDRAM_35,
        LCD_LMB162AFC_DDRAM_36,
        LCD_LMB162AFC_DDRAM_37,
        LCD_LMB162AFC_DDRAM_38,
        LCD_LMB162AFC_DDRAM_39,
        LCD_LMB162AFC_DDRAM_40,
        LCD_LMB162AFC_DDRAM_41,
        LCD_LMB162AFC_DDRAM_42,
        LCD_LMB162AFC_DDRAM_43,
        LCD_LMB162AFC_DDRAM_44,
        LCD_LMB162AFC_DDRAM_45,
        LCD_LMB162AFC_DDRAM_46,
        LCD_LMB162AFC_DDRAM_47,
        LCD_LMB162AFC_DDRAM_48,
        LCD_LMB162AFC_DDRAM_49,
        LCD_LMB162AFC_DDRAM_50,
        LCD_LMB162AFC_DDRAM_51,
        LCD_LMB162AFC_DDRAM_52,
        LCD_LMB162AFC_DDRAM_53,
        LCD_LMB162AFC_DDRAM_54,
        LCD_LMB162AFC_DDRAM_55,
        LCD_LMB162AFC_DDRAM_56,
        LCD_LMB162AFC_DDRAM_57,
        LCD_LMB162AFC_DDRAM_58,
        LCD_LMB162AFC_DDRAM_59,
        LCD_LMB162AFC_DDRAM_60,
        LCD_LMB162AFC_DDRAM_61,
        LCD_LMB162AFC_DDRAM_62,
        LCD_LMB162AFC_DDRAM_63,
        LCD_LMB162AFC_DDRAM_64,
        LCD_LMB162AFC_DDRAM_65,
        LCD_LMB162AFC_DDRAM_66,
        LCD_LMB162AFC_DDRAM_67,
        LCD_LMB162AFC_DDRAM_68,
        LCD_LMB162AFC_DDRAM_69,
        LCD_LMB162AFC_DDRAM_70,
        LCD_LMB162AFC_DDRAM_71,
        LCD_LMB162AFC_DDRAM_72,
        LCD_LMB162AFC_DDRAM_73,
        LCD_LMB162AFC_DDRAM_74,
        LCD_LMB162AFC_DDRAM_75,
        LCD_LMB162AFC_DDRAM_76,
        LCD_LMB162AFC_DDRAM_77,
        LCD_LMB162AFC_DDRAM_78,
        LCD_LMB162AFC_DDRAM_79,
        LCD_LMB162AFC_DDRAM_80,
        LCD_LMB162AFC_DDRAM_81,
        LCD_LMB162AFC_DDRAM_82,
        LCD_LMB162AFC_DDRAM_83,
        LCD_LMB162AFC_DDRAM_84,
        LCD_LMB162AFC_DDRAM_85,
        LCD_LMB162AFC_DDRAM_86,
        LCD_LMB162AFC_DDRAM_87,
        LCD_LMB162AFC_DDRAM_88,
        LCD_LMB162AFC_DDRAM_89,
        LCD_LMB162AFC_DDRAM_90,
        LCD_LMB162AFC_DDRAM_91,
        LCD_LMB162AFC_DDRAM_92,
        LCD_LMB162AFC_DDRAM_93,
        LCD_LMB162AFC_DDRAM_94,
        LCD_LMB162AFC_DDRAM_95,
        LCD_LMB162AFC_DDRAM_96,
        LCD_LMB162AFC_DDRAM_97,
        LCD_LMB162AFC_DDRAM_98,
        LCD_LMB162AFC_DDRAM_99,
        LCD_LMB162AFC_DDRAM_100,
        LCD_LMB162AFC_DDRAM_101,
        LCD_LMB162AFC_DDRAM_102,
        LCD_LMB162AFC_DDRAM_103,
        LCD_LMB162AFC_DDRAM_104,
        LCD_LMB162AFC_DDRAM_105,
        LCD_LMB162AFC_DDRAM_106,
        LCD_LMB162AFC_DDRAM_107,
        LCD_LMB162AFC_DDRAM_108,
        LCD_LMB162AFC_DDRAM_109,
        LCD_LMB162AFC_DDRAM_110,
        LCD_LMB162AFC_DDRAM_111,
        LCD_LMB162AFC_DDRAM_112,
        LCD_LMB162AFC_DDRAM_113,
        LCD_LMB162AFC_DDRAM_114,
        LCD_LMB162AFC_DDRAM_115,
        LCD_LMB162AFC_DDRAM_116,
        LCD_LMB162AFC_DDRAM_117,
        LCD_LMB162AFC_DDRAM_118,
        LCD_LMB162AFC_DDRAM_119,
        LCD_LMB162AFC_DDRAM_120,
        LCD_LMB162AFC_DDRAM_121,
        LCD_LMB162AFC_DDRAM_122,
        LCD_LMB162AFC_DDRAM_123,
        LCD_LMB162AFC_DDRAM_124,
        LCD_LMB162AFC_DDRAM_125,
        LCD_LMB162AFC_DDRAM_126,
        LCD_LMB162AFC_DDRAM_127,
    } LCD_LMB162AFC_DDRAM_t;

    typedef enum LCD_LMB162AFC_Row
    {
        LCD_LMB162AFC_Row_0 = 0,
        LCD_LMB162AFC_Row_1,
    } LCD_LMB162AFC_Row_t;

    typedef enum LCD_LMB162AFC_Column
    {
        LCD_LMB162AFC_Column_0 = 0,
        LCD_LMB162AFC_Column_1,
        LCD_LMB162AFC_Column_2,
        LCD_LMB162AFC_Column_3,
        LCD_LMB162AFC_Column_4,
        LCD_LMB162AFC_Column_5,
        LCD_LMB162AFC_Column_6,
        LCD_LMB162AFC_Column_7,
        LCD_LMB162AFC_Column_8,
        LCD_LMB162AFC_Column_9,
        LCD_LMB162AFC_Column_10,
        LCD_LMB162AFC_Column_11,
        LCD_LMB162AFC_Column_12,
        LCD_LMB162AFC_Column_13,
        LCD_LMB162AFC_Column_14,
        LCD_LMB162AFC_Column_15,
    } LCD_LMB162AFC_Column_t;

    typedef struct LCD_LMB162AFC_Coordinate
    {
        LCD_LMB162AFC_Row_t Row;
        LCD_LMB162AFC_Column_t Column;
    } LCD_LMB162AFC_Coordinate_t;

    typedef uint32_t LCD_LMB162AFC_Width_t;

    typedef uint32_t LCD_LMB162AFC_Height_t;

    typedef struct LCD_LMB162AFC_Size
    {
        LCD_LMB162AFC_Width_t Width;
        LCD_LMB162AFC_Height_t Height;
    } LCD_LMB162AFC_Size_t;

    typedef uint8_t LCD_LMB162AFC_Pixel_t;

    typedef LCD_LMB162AFC_Pixel_t LCD_LMB162AFC_Screen_t[ LCD_LMB162AFC_WIDTH ][ LCD_LMB162AFC_HEIGHT ];

    typedef uint8_t LCD_LMB162AFC_Character_t;

    typedef struct LCD_LMB162AFC_InstanceContext LCD_LMB162AFC_InstanceContext_t;

    typedef struct LCD_LMB162AFC_Instance
    {
        SPI_t SPI;
        GPIO_t RegisterSelect;
        GPIO_t ChipSelect;
        GPIO_t ReadWrite;

        // Managed internally !
        LCD_LMB162AFC_InstanceContext_t * Context;
    } LCD_LMB162AFC_Instance_t;

    // #############################################################################
    // #### Public Method(s) #######################################################
    // #############################################################################

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Cycle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_DeInitialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetSize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size_t * LCD_LMB162AFC_Size );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCursor( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Write( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character_t LCD_LMB162AFC_Character );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetPixel( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate, LCD_LMB162AFC_Pixel_t LCD_LMB162AFC_Pixel );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetScreen( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen_t ** LCD_LMB162AFC_Screen );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift_t LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter_t LCD_LMB162AFC_Counter );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink_t LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor_t LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display_t LCD_LMB162AFC_Display );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection_t LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift_t LCD_LMB162AFC_Shift );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font_t LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode_t LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface_t LCD_LMB162AFC_Interface );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM_t LCD_LMB162AFC_CGRAM );
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM_t LCD_LMB162AFC_DDRAM );

    // TODO Add APIs

    // #############################################################################
    // #### Public Variable(s) #####################################################
    // #############################################################################

    extern const char LCD_LMB162AFC_VERSION[];

    // #############################################################################
    // #### File Guard #############################################################
    // #############################################################################

    #ifdef __cplusplus
} /* extern "C" */
    #endif /* __cplusplus */

#endif /* LCD_LMB162AFC_H_ */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
