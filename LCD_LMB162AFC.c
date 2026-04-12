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

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

#include "../../LCD_Internal.h"
#include "LCD_LMB162AFC.h"

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

#define LCD_LMB162AFC_BUFFER_SIZE_TRANSMIT ( 8 )

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef enum LCD_LMB162AFC_Command
{
    LCD_LMB162AFC_Command_Clear = 0b00000001,
    LCD_LMB162AFC_Command_ReturnHome = 0b00000010,
    LCD_LMB162AFC_Command_SetMode = 0b00000100,
    LCD_LMB162AFC_Command_SetDisplay = 0b00001000,
    LCD_LMB162AFC_Command_SetShift = 0b00010000,
    LCD_LMB162AFC_Command_SetFunction = 0b00100000,
    LCD_LMB162AFC_Command_SetCGRAM = 0b01000000,
    LCD_LMB162AFC_Command_SetDDRAM = 0b10000000,
} LCD_LMB162AFC_Command_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_Clear
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];
} LCD_LMB162AFC_Clear_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_ReturnHome
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];
} LCD_LMB162AFC_ReturnHome_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetMode
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t S   : 1;
        uint8_t I_D : 1;
        uint8_t     : 6;
    };
} LCD_LMB162AFC_SetMode_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetDisplay
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t B : 1;
        uint8_t C : 1;
        uint8_t D : 1;
        uint8_t   : 5;
    };
} LCD_LMB162AFC_SetDisplay_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetShift
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t     : 2;
        uint8_t R_L : 1;
        uint8_t S_C : 1;
        uint8_t     : 4;
    };
} LCD_LMB162AFC_SetShift_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetFunction
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t    : 2;
        uint8_t F  : 1;
        uint8_t N  : 1;
        uint8_t DL : 1;
        uint8_t    : 3;
    };
} LCD_LMB162AFC_SetFunction_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetCGRAM
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t AC : 6;
        uint8_t    : 2;
    };
} LCD_LMB162AFC_SetCGRAM_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) LCD_LMB162AFC_SetDDRAM
{
    const LCD_LMB162AFC_Command_t Command[ 1 ];

    struct
    {
        uint8_t AC : 7;
        uint8_t    : 1;
    };
} LCD_LMB162AFC_SetDDRAM_t;

typedef enum LCD_LMB162AFC_OperationType
{
    LCD_LMB162AFC_OperationType_None = 0,

    // Process Pre-Operation
    LCD_LMB162AFC_OperationType_Pending,

    // Basic Operation(s)
    LCD_LMB162AFC_OperationType_Power_On,
    LCD_LMB162AFC_OperationType_Power_Off,

    // General Operation(s)
    LCD_LMB162AFC_OperationType_Clear,
    LCD_LMB162AFC_OperationType_ReturnHome,
    LCD_LMB162AFC_OperationType_SetMode,
    LCD_LMB162AFC_OperationType_SetDisplay,
    LCD_LMB162AFC_OperationType_SetShift,
    LCD_LMB162AFC_OperationType_SetFunction,
    LCD_LMB162AFC_OperationType_SetCGRAM,
    LCD_LMB162AFC_OperationType_SetDDRAM,

    LCD_LMB162AFC_OperationType_Flush,
} LCD_LMB162AFC_OperationType_t;

typedef LCD_LMB162AFC_Status_t ( *LCD_LMB162AFC_Operation_Handler_t )( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

typedef struct LCD_LMB162AFC_Operation
{
    LCD_LMB162AFC_OperationType_t Type;
    LCD_LMB162AFC_Operation_Handler_t Handler;
    LCD_LMB162AFC_Status_t Status;
    TIM_Timestamp_t Timestamp;
} LCD_LMB162AFC_Operation_t;

typedef enum LCD_LMB162AFC_ProcessType
{
    LCD_LMB162AFC_ProcessType_None = 0,
    LCD_LMB162AFC_ProcessType_Command,
    LCD_LMB162AFC_ProcessType_Initialize,
    LCD_LMB162AFC_ProcessType_DeInitialize,
} LCD_LMB162AFC_ProcessType_t;

typedef LCD_LMB162AFC_Status_t ( *LCD_LMB162AFC_Process_Handler_t )( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

typedef struct LCD_LMB162AFC_Process
{
    LCD_LMB162AFC_ProcessType_t Type;
    LCD_LMB162AFC_Process_Handler_t Handler;
    LCD_LMB162AFC_Status_t Status;
} LCD_LMB162AFC_Process_t;

typedef struct LCD_LMB162AFC_Buffer_Transmit
{
    uint32_t Length;
    uint8_t Content[ LCD_LMB162AFC_BUFFER_SIZE_TRANSMIT ];
} LCD_LMB162AFC_Buffer_Transmit_t;

typedef struct LCD_LMB162AFC_InstanceContext
{
    LCD_LMB162AFC_Process_t Process;
    LCD_LMB162AFC_Operation_t Operation;
    LCD_LMB162AFC_Screen_t Screen;
    LCD_LMB162AFC_Buffer_Transmit_t Transmit;
} LCD_LMB162AFC_InstanceContext_t;

typedef struct LCD_LMB162AFC_Context
{
    TIM_Timestamp_t Timestamp;
} LCD_LMB162AFC_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Handler_Command( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Handler_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_t LCD_LMB162AFC_ProcessType );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetSize_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size_t * LCD_LMB162AFC_Size );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCursor_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Write_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character_t LCD_LMB162AFC_Character );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetPixel_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate, LCD_LMB162AFC_Pixel_t LCD_LMB162AFC_Pixel );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetScreen_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen_t ** LCD_LMB162AFC_Screen );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift_t LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter_t LCD_LMB162AFC_Counter );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink_t LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor_t LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display_t LCD_LMB162AFC_Display );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection_t LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift_t LCD_LMB162AFC_Shift );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font_t LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode_t LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface_t LCD_LMB162AFC_Interface );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM_t LCD_LMB162AFC_CGRAM );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM_t LCD_LMB162AFC_DDRAM );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsIdle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint32_t time_us );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint8_t * buffer, uint32_t length );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Transfer( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint8_t * buffer, uint32_t length );

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_DeInitialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_Cycle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance );
//
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_Initialize( void );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_DeInitialize( void );
static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_Cycle( void );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

static LCD_LMB162AFC_Context_t LCD_LMB162AFC_Context;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Handler_Command( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Process.Type != LCD_LMB162AFC_ProcessType_Command )
        {
            LCD_Error( "Invalid Process Handler, Expected %d Found %d", LCD_LMB162AFC_ProcessType_Command, LCD_LMB162AFC_Instance->Context->Process.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;

        if ( LCD_LMB162AFC_IsIdle( LCD_LMB162AFC_Instance ) != LCD_LMB162AFC_Status_Success )
        {
            // Operation In-progress
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Status != LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_None;
        }
        switch ( LCD_LMB162AFC_Instance->Context->Operation.Type )
        {
            default:
                LCD_LMB162AFC_Instance->Context->Process.Status = LCD_LMB162AFC_Instance->Context->Operation.Status;
                // TODO OnComplete Callback
                LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
                break;
        }
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Handler_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Process.Type != LCD_LMB162AFC_ProcessType_Initialize )
        {
            LCD_Error( "Invalid Process Handler, Expected %d Found %d", LCD_LMB162AFC_ProcessType_Initialize, LCD_LMB162AFC_Instance->Context->Process.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;

        if ( LCD_LMB162AFC_IsIdle( LCD_LMB162AFC_Instance ) != LCD_LMB162AFC_Status_Success )
        {
            // Operation In-progress
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Status != LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_None;
        }
        switch ( LCD_LMB162AFC_Instance->Context->Operation.Type )
        {
            case LCD_LMB162AFC_OperationType_Pending:
                // TODO
            case LCD_LMB162AFC_OperationType_None:
            default:
                LCD_LMB162AFC_Instance->Context->Process.Status = LCD_LMB162AFC_Instance->Context->Operation.Status;
                // TODO OnComplete Callback
                LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
                break;
        }
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_t LCD_LMB162AFC_ProcessType )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Process.Type = LCD_LMB162AFC_ProcessType;
        LCD_LMB162AFC_Instance->Context->Process.Status = LCD_LMB162AFC_Status_Success;
        switch ( LCD_LMB162AFC_Instance->Context->Process.Type )
        {
            case LCD_LMB162AFC_ProcessType_Command:
                LCD_LMB162AFC_Instance->Context->Process.Handler = LCD_LMB162AFC_Process_Handler_Command;
                break;
            case LCD_LMB162AFC_ProcessType_Initialize:
                LCD_LMB162AFC_Instance->Context->Process.Handler = LCD_LMB162AFC_Process_Handler_Initialize;
                break;
            case LCD_LMB162AFC_ProcessType_None:
            default:
                LCD_LMB162AFC_Instance->Context->Process.Handler = NULL;
                if ( LCD_LMB162AFC_Instance->Context->Process.Type != LCD_LMB162AFC_ProcessType_None )
                {
                    LCD_LMB162AFC_Instance->Context->Process.Type = LCD_LMB162AFC_ProcessType_None;
                    LCD_Warning( "Un-Handled Process Setting" );
                }
                break;
        }
        switch ( LCD_LMB162AFC_Instance->Context->Process.Type )
        {
            case LCD_LMB162AFC_ProcessType_Command:
                // Externally Handled Operation
                break;
            case LCD_LMB162AFC_ProcessType_None:
                LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_None;
                break;
            default:
                LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_Pending;
                break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetSize_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size_t * LCD_LMB162AFC_Size )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Size=%p )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size );
        if ( LCD_LMB162AFC_Size == NULL )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Size->Height = LCD_LMB162AFC_HEIGHT;
        LCD_LMB162AFC_Size->Width = LCD_LMB162AFC_WIDTH;
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCursor_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Coordinate={Row=%d, Column=%d} )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate.Row, LCD_LMB162AFC_Coordinate.Column );
        //    if ( LCD_LMB162AFC_Coordinate.Row < 0
        //      || LCD_LMB162AFC_Coordinate.Row > LCD_LMB162AFC_HEIGHT
        //      || LCD_LMB162AFC_Coordinate.Column < 0
        //      || LCD_LMB162AFC_Coordinate.Column > LCD_LMB162AFC_WIDTH )
        //    {
        //      LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
        //      break;
        //    }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        //    LCD_LMB162AFC_Instance->Context->Screen[ LCD_LMB162AFC_Coordinate.Row ][ LCD_LMB162AFC_Coordinate.Column ] = LCD_LMB162AFC_Pixel;
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_NotSupported;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Write_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character_t LCD_LMB162AFC_Character )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Character=%02X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character );
        //    if ( LCD_LMB162AFC_Coordinate.Row < 0
        //      || LCD_LMB162AFC_Coordinate.Row > LCD_LMB162AFC_HEIGHT
        //      || LCD_LMB162AFC_Coordinate.Column < 0
        //      || LCD_LMB162AFC_Coordinate.Column > LCD_LMB162AFC_WIDTH )
        //    {
        //      LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
        //      break;
        //    }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        //    LCD_LMB162AFC_Instance->Context->Screen[ LCD_LMB162AFC_Coordinate.Row ][ LCD_LMB162AFC_Coordinate.Column ] = LCD_LMB162AFC_Pixel;
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_NotSupported;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetPixel_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate, LCD_LMB162AFC_Pixel_t LCD_LMB162AFC_Pixel )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Coordinate={Row=%d, Column=%d}, Pixel=%02X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate.Row, LCD_LMB162AFC_Coordinate.Column, LCD_LMB162AFC_Pixel );
        //    if ( LCD_LMB162AFC_Coordinate.Row < 0
        //      || LCD_LMB162AFC_Coordinate.Row > LCD_LMB162AFC_HEIGHT
        //      || LCD_LMB162AFC_Coordinate.Column < 0
        //      || LCD_LMB162AFC_Coordinate.Column > LCD_LMB162AFC_WIDTH )
        //    {
        //      LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
        //      break;
        //    }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        //    LCD_LMB162AFC_Instance->Context->Screen[ LCD_LMB162AFC_Coordinate.Row ][ LCD_LMB162AFC_Coordinate.Column ] = LCD_LMB162AFC_Pixel;
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_NotSupported;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetScreen_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen_t ** LCD_LMB162AFC_Screen )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Screen=%p )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen );
        if ( LCD_LMB162AFC_Screen == NULL )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        *LCD_LMB162AFC_Screen = &LCD_LMB162AFC_Instance->Context->Screen;
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_Flush )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_Flush, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        // TODO Verify SPI Status
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Transfer( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_Instance->Context->Screen, UTIL_SizeOf( LCD_LMB162AFC_Instance->Context->Screen ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_Flush;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_Flush_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 200000 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_Clear )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_Clear, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Clear_t LCD_LMB162AFC_Clear = { { LCD_LMB162AFC_Command_Clear } };
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_Clear, UTIL_SizeOf( LCD_LMB162AFC_Clear ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_Clear;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_Clear_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_ReturnHome )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_ReturnHome, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_ReturnHome_t LCD_LMB162AFC_ReturnHome = { { LCD_LMB162AFC_Command_ReturnHome } };
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_ReturnHome, UTIL_SizeOf( LCD_LMB162AFC_ReturnHome ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_ReturnHome;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_ReturnHome_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetMode )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetMode, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift_t LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter_t LCD_LMB162AFC_Counter )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetMode_t LCD_LMB162AFC_SetMode = { { LCD_LMB162AFC_Command_SetMode } };
        LCD_LMB162AFC_SetMode.S = LCD_LMB162AFC_DisplayShift;
        LCD_LMB162AFC_SetMode.I_D = LCD_LMB162AFC_Counter;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetMode, UTIL_SizeOf( LCD_LMB162AFC_SetMode ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetMode;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetMode_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetDisplay )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetDisplay, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink_t LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor_t LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display_t LCD_LMB162AFC_Display )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetDisplay_t LCD_LMB162AFC_SetDisplay = { { LCD_LMB162AFC_Command_SetDisplay } };
        LCD_LMB162AFC_SetDisplay.B = LCD_LMB162AFC_CursorBlink;
        LCD_LMB162AFC_SetDisplay.C = LCD_LMB162AFC_Cursor;
        LCD_LMB162AFC_SetDisplay.D = LCD_LMB162AFC_Display;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetDisplay, UTIL_SizeOf( LCD_LMB162AFC_SetDisplay ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetDisplay;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetDisplay_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetShift )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetShift, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection_t LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift_t LCD_LMB162AFC_Shift )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetShift_t LCD_LMB162AFC_SetShift = { { LCD_LMB162AFC_Command_SetShift } };
        LCD_LMB162AFC_SetShift.R_L = LCD_LMB162AFC_ShiftDirection;
        LCD_LMB162AFC_SetShift.S_C = LCD_LMB162AFC_Shift;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetShift, UTIL_SizeOf( LCD_LMB162AFC_SetShift ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetShift;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetShift_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetFunction )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetFunction, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font_t LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode_t LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface_t LCD_LMB162AFC_Interface )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetFunction_t LCD_LMB162AFC_SetFunction = { { LCD_LMB162AFC_Command_SetFunction } };
        LCD_LMB162AFC_SetFunction.F = LCD_LMB162AFC_Font;
        LCD_LMB162AFC_SetFunction.N = LCD_LMB162AFC_Mode;
        LCD_LMB162AFC_SetFunction.DL = LCD_LMB162AFC_Interface;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetFunction, UTIL_SizeOf( LCD_LMB162AFC_SetFunction ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetFunction;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetFunction_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetCGRAM )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetCGRAM, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM_t LCD_LMB162AFC_CGRAM )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetCGRAM_t LCD_LMB162AFC_SetCGRAM = { { LCD_LMB162AFC_Command_SetCGRAM } };
        LCD_LMB162AFC_SetCGRAM.AC = LCD_LMB162AFC_CGRAM;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetCGRAM, UTIL_SizeOf( LCD_LMB162AFC_SetCGRAM ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetCGRAM;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetCGRAM_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM_Handler( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_SetDDRAM )
        {
            LCD_Error( "Invalid Operation Handler, Expected %d Found %d", LCD_LMB162AFC_OperationType_SetDDRAM, LCD_LMB162AFC_Instance->Context->Operation.Type );
            LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_None );
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status;
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance ) ) == LCD_LMB162AFC_Status_Success )
        {
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM_t LCD_LMB162AFC_DDRAM )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_SetDDRAM_t LCD_LMB162AFC_SetDDRAM = { { LCD_LMB162AFC_Command_SetDDRAM } };
        LCD_LMB162AFC_SetDDRAM.AC = LCD_LMB162AFC_DDRAM;
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance, ( uint8_t * ) &LCD_LMB162AFC_SetDDRAM, UTIL_SizeOf( LCD_LMB162AFC_SetDDRAM ) ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_SetDDRAM;
        LCD_LMB162AFC_Instance->Context->Operation.Handler = LCD_LMB162AFC_SetDDRAM_Handler;
        LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
        LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;
        LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance, 1 ); // FIXME Set Appropriate Delay Value
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsIdle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Handler != NULL )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Busy;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsTimeout( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( LCD_TIM, &LCD_LMB162AFC_Instance->Context->Operation.Timestamp ) ) != TIM_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Busy;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Delay( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint32_t time_us )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMicrosecond( &LCD_LMB162AFC_Instance->Context->Operation.Timestamp, time_us ) ) != TIM_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Process.Type != LCD_LMB162AFC_ProcessType_None
             || LCD_LMB162AFC_Instance->Context->Operation.Type != LCD_LMB162AFC_OperationType_None )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Busy;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Execute( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint8_t * buffer, uint32_t length )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        GPIO_Status_t GPIO_Status = GPIO_Status_Error;
        if ( ( GPIO_Status = GPIO_Write( LCD_LMB162AFC_Instance->RegisterSelect, GPIO_Value_Low ) ) != GPIO_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        SPI_Status_t SPI_Status = SPI_Status_Error;
        if ( ( SPI_Status = SPI_Write( LCD_LMB162AFC_Instance->SPI, buffer, length ) ) != SPI_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Transfer( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, uint8_t * buffer, uint32_t length )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        GPIO_Status_t GPIO_Status = GPIO_Status_Error;
        if ( ( GPIO_Status = GPIO_Write( LCD_LMB162AFC_Instance->RegisterSelect, GPIO_Value_High ) ) != GPIO_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        SPI_Status_t SPI_Status = SPI_Status_Error;
        if ( ( SPI_Status = SPI_Write( LCD_LMB162AFC_Instance->SPI, buffer, length ) ) != SPI_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( LCD_LMB162AFC_Instance == NULL )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context == NULL )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            if ( LCD_LMB162AFC_Instance == NULL )
            {
                LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_ArgumentInvalid;
                break;
            }
            if ( LCD_LMB162AFC_Instance->Context == NULL )
            {
                RAM_Status_t RAM_Status = RAM_Status_Error;
                if ( ( RAM_Status = RAM_Allocate( RAM_1, ( RAM_Reference_t * ) &LCD_LMB162AFC_Instance->Context, UTIL_SizeOf( LCD_LMB162AFC_InstanceContext_t ) ) ) != RAM_Status_Success )
                {
                    LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
                    break;
                }
                // Double check context validity
                if ( LCD_LMB162AFC_Instance->Context == NULL )
                {
                    LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
                    break;
                }
            }
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
        }

        if ( LCD_LMB162AFC_Status == LCD_LMB162AFC_Status_Success )
        {
            UTIL_MemorySetZero( LCD_LMB162AFC_Instance->Context->Screen, UTIL_SizeOf( LCD_LMB162AFC_Instance->Context->Screen ) );

            LCD_LMB162AFC_Instance->Context->Transmit.Length = 0;
            LCD_LMB162AFC_Instance->Context->Transmit.Content[ LCD_LMB162AFC_Instance->Context->Transmit.Length ] = 0;

            LCD_LMB162AFC_Instance->Context->Operation.Type = LCD_LMB162AFC_OperationType_None;
            LCD_LMB162AFC_Instance->Context->Operation.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Operation.Status = LCD_LMB162AFC_Status_Success;
            LCD_LMB162AFC_Instance->Context->Operation.Timestamp = LCD_LMB162AFC_Context.Timestamp;

            LCD_LMB162AFC_Instance->Context->Process.Type = LCD_LMB162AFC_ProcessType_None;
            LCD_LMB162AFC_Instance->Context->Process.Handler = NULL;
            LCD_LMB162AFC_Instance->Context->Process.Status = LCD_LMB162AFC_Status_Success;

            if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Initialize ) ) != LCD_LMB162AFC_Status_Success )
            {
                break;
            }
        }
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_DeInitialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_DeInitialize ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Instance_Cycle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_IsValid( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( LCD_LMB162AFC_Instance->Context->Operation.Handler != NULL )
        {
            if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance->Context->Operation.Handler( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
            {
                // FIXME
            }
        }
        if ( LCD_LMB162AFC_Instance->Context->Process.Handler != NULL )
        {
            if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance->Context->Process.Handler( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
            {
                // FIXME
            }
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_Initialize( void )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( void )", __FUNCTION__ );
        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_GetTimestamp( LCD_TIM, &LCD_LMB162AFC_Context.Timestamp ) ) != TIM_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_DeInitialize( void )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( void )", __FUNCTION__ );
        // Nothing to be done
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

static LCD_LMB162AFC_Status_t LCD_LMB162AFC_Context_Cycle( void )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( void )", __FUNCTION__ );
        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_GetTimestamp( LCD_TIM, &LCD_LMB162AFC_Context.Timestamp ) ) != TIM_Status_Success )
        {
            LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Success;
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

LCD_LMB162AFC_Status_t LCD_LMB162AFC_Initialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Context_Initialize( ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_Initialize( LCD_LMB162AFC_Instance );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_Cycle( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Context_Cycle( ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_Cycle( LCD_LMB162AFC_Instance );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_DeInitialize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Instance_DeInitialize( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Context_DeInitialize( );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetSize( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size_t * LCD_LMB162AFC_Size )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Size=%p )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size );
        // FIXME
        LCD_LMB162AFC_Status = LCD_LMB162AFC_GetSize_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Size );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCursor( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Coordinate={Row=%d, Column=%d} )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate.Row, LCD_LMB162AFC_Coordinate.Column );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetCursor_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_Write( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character_t LCD_LMB162AFC_Character )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Character=%02X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Write_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Character );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetPixel( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate_t LCD_LMB162AFC_Coordinate, LCD_LMB162AFC_Pixel_t LCD_LMB162AFC_Pixel )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Coordinate={Row=%d, Column=%d}, Pixel=%02X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate.Row, LCD_LMB162AFC_Coordinate.Column, LCD_LMB162AFC_Pixel );
        // FIXME
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetPixel_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Coordinate, LCD_LMB162AFC_Pixel );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_GetScreen( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen_t ** LCD_LMB162AFC_Screen )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Screen=%p )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen );
        // FIXME
        LCD_LMB162AFC_Status = LCD_LMB162AFC_GetScreen_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Screen );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_Flush( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Flush_Execute( LCD_LMB162AFC_Instance );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_Clear( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_Clear_Execute( LCD_LMB162AFC_Instance );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_ReturnHome( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p )", __FUNCTION__, LCD_LMB162AFC_Instance );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_ReturnHome_Execute( LCD_LMB162AFC_Instance );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetMode( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift_t LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter_t LCD_LMB162AFC_Counter )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, DisplayShift=%d, Counter=%d )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetMode_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_DisplayShift, LCD_LMB162AFC_Counter );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDisplay( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink_t LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor_t LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display_t LCD_LMB162AFC_Display )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Blink=%d, Cursor=%d, Display=%d )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetDisplay_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_CursorBlink, LCD_LMB162AFC_Cursor, LCD_LMB162AFC_Display );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetShift( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection_t LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift_t LCD_LMB162AFC_Shift )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Direction=%d, Shift=%d )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetShift_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ShiftDirection, LCD_LMB162AFC_Shift );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetFunction( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font_t LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode_t LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface_t LCD_LMB162AFC_Interface )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Font=%d, Mode=%d, Interface=%d )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetFunction_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_Font, LCD_LMB162AFC_Mode, LCD_LMB162AFC_Interface );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetCGRAM( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM_t LCD_LMB162AFC_CGRAM )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Address=%04X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetCGRAM_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_CGRAM );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

LCD_LMB162AFC_Status_t LCD_LMB162AFC_SetDDRAM( LCD_LMB162AFC_Instance_t * LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM_t LCD_LMB162AFC_DDRAM )
{
    LCD_LMB162AFC_Status_t LCD_LMB162AFC_Status = LCD_LMB162AFC_Status_Error;
    do
    {
        LCD_Trace( "%s( Instance=%p, Address=%04X )", __FUNCTION__, LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM );
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_IsReady( LCD_LMB162AFC_Instance ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        if ( ( LCD_LMB162AFC_Status = LCD_LMB162AFC_Process_Set( LCD_LMB162AFC_Instance, LCD_LMB162AFC_ProcessType_Command ) ) != LCD_LMB162AFC_Status_Success )
        {
            break;
        }
        LCD_LMB162AFC_Status = LCD_LMB162AFC_SetDDRAM_Execute( LCD_LMB162AFC_Instance, LCD_LMB162AFC_DDRAM );
    }
    while ( 0 );
    return LCD_LMB162AFC_Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

const char LCD_LMB162AFC_VERSION[] = "0.0.0.v20260412-1852";

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
