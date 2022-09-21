 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mohamed Faryed
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H

/* Id for the company in the AUTOSAR
 * for example Mohamed Tarek's ID = 1000 :) */
#define PORT_VENDOR_ID                                  (1000U)

/* PORT Module Id */
#define PORT_MODULE_ID    (140U)

/* PORT Instance Id */
#define PORT_INSTANCE_ID  (0U)

/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/*
 * Macros for Port Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* Standard AUTOSAR types */
#include "Std_Types.h"

/* AUTOSAR checking between Std Types and Dio Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* PORT Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

/* AUTOSAR Version checking between Dio_Cfg.h and Dio.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Port_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif

#include "Common_Macros.h"
   
/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/
   
/* Service ID for PORT INITIALIZATIO */
#define PORT_INIT_SID                              ((uint8)0x00U)
   
/* Service ID for PORT SET_PIN_DIRECTION */
#define PORT_SET_PIN_DIRECTION_SID                 ((uint8)0x01U)

/* Service ID for REFRESH_PORT_DIRECTION */
#define PORT_REFRESH_PORT_DIRECTION_SID            ((uint8)0x02U)
   
/* Service ID for PORT GET_VERSION */
#define PORT_GET_VERSION_INFO_SID                  ((uint8)0x03U)
   
/* Service ID for SET_PIN_MODE */
#define PORT_SET_PIN_MODE_SID                      ((uint8)0x04U)

/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/
   
/*Invalid Port Pin ID requested
  Function: Port_SetPinMode
            Port_SetPinDirection */
#define PORT_E_PARAM_PIN_ID                                     ((uint8)0x0A)

   
/*Port Pin not configured as changeable
  Function: Port_SetPinDirection */
#define PORT_E_DIRECTION_UNCHANGEABLE_ID                        ((uint8)0x0B)

   
/*API Port_Init service called with wrong parameter
  Function: Port_Init */
#define PORT_E_PARAM_CONFIG_ID                                  ((uint8)0x0C)
 
   
/*API Port_SetPinMode service called when mode is unchangeable
  Function: Port_SetPinMode */
#define PORT_E_PARAM_INVALID_MODE_ID                            ((uint8)0x0D)

   
/*API Port_SetPinMode service called when mode is unchangeable
  Function: Port_SetPinMode */
#define PORT_E_MODE_UNCHANGEABLE_ID                             ((uint8)0x0E)
 
   
/*API service called without module initialization
  Function: Port_SetPinDirection,
            Port_SetPinMode
            Port_GetVersionInfo
            Port_RefreshPortDirection */
#define PORT_E_UNINIT_ID                                        ((uint8)0x0F)   

   
/*APIs called with a Null Pointer
   Function: Port_GetVersionInfo */
#define PORT_E_PARAM_POINTER_ID                                 ((uint8)0x10) 
   
/*******************************************************************************
 *                              Module Definitions                             *
 *******************************************************************************/  
   
/* GPIO Registers base addresses */
#define GPIO_PORTA_BASE_ADDRESS           0x40004000
#define GPIO_PORTB_BASE_ADDRESS           0x40005000
#define GPIO_PORTC_BASE_ADDRESS           0x40006000
#define GPIO_PORTD_BASE_ADDRESS           0x40007000
#define GPIO_PORTE_BASE_ADDRESS           0x40024000
#define GPIO_PORTF_BASE_ADDRESS           0x40025000

/* GPIO Registers offset addresses */
#define PORT_DATA_REG_OFFSET              0x3FC
#define PORT_DIR_REG_OFFSET               0x400
#define PORT_ALT_FUNC_REG_OFFSET          0x420
#define PORT_PULL_UP_REG_OFFSET           0x510
#define PORT_PULL_DOWN_REG_OFFSET         0x514
#define PORT_DIGITAL_ENABLE_REG_OFFSET    0x51C
#define PORT_LOCK_REG_OFFSET              0x520
#define PORT_COMMIT_REG_OFFSET            0x524
#define PORT_ANALOG_MODE_SEL_REG_OFFSET   0x528
#define PORT_CTL_REG_OFFSET               0x52C

#define PIN0               ((uint8)0U)
#define PIN1               ((uint8)1U)
#define PIN2               ((uint8)2U)
#define PIN3               ((uint8)3U)
#define PIN4               ((uint8)4U)
#define PIN5               ((uint8)5U)
#define PIN6               ((uint8)6U)
#define PIN7               ((uint8)7U)

#define PORT_A             ((uint8)0U)
#define PORT_B             ((uint8)1U)
#define PORT_C             ((uint8)2U)
#define PORT_D             ((uint8)3U)
#define PORT_E             ((uint8)4U)
#define PORT_F             ((uint8)5U)

#define GPIO_MODE          ((uint8)0x0U)
#define UART_0_MODE        ((uint8)0x1U)
#define SSI_0_MODE         ((uint8)0x2U)
#define I2C_1_MODE         ((uint8)0x3U)
#define JTAG_MODE          ((uint8)0x1U)

   
#define PA0               ((uint8)0U)
#define PA1               ((uint8)1U)
#define PA2               ((uint8)2U)
#define PA3               ((uint8)3U)
#define PA4               ((uint8)4U)
#define PA5               ((uint8)5U)
#define PA6               ((uint8)6U)
#define PA7               ((uint8)7U)

#define PB0               ((uint8)8U)
#define PB1               ((uint8)9U)
#define PB2               ((uint8)10U)
#define PB3               ((uint8)11U)
#define PB4               ((uint8)12U)
#define PB5               ((uint8)13U)
#define PB6               ((uint8)14U)
#define PB7               ((uint8)15U)
   
#define PC4               ((uint8)20U)
#define PC5               ((uint8)21U)
#define PC6               ((uint8)22U)
#define PC7               ((uint8)23U)

#define PD0               ((uint8)24U)
#define PD1               ((uint8)25U)
#define PD2               ((uint8)26U)
#define PD3               ((uint8)27U)
#define PD4               ((uint8)28U)
#define PD5               ((uint8)29U)
#define PD6               ((uint8)30U)
#define PD7               ((uint8)31U)

#define PE0               ((uint8)32U)
#define PE1               ((uint8)33U)
#define PE2               ((uint8)34U)
#define PE3               ((uint8)35U)
#define PE4               ((uint8)36U)
#define PE5               ((uint8)37U)
#define PE6               ((uint8)38U)
#define PE7               ((uint8)39U) 
   
#define PF0               ((uint8)40U)
#define PF1               ((uint8)41U)
#define PF2               ((uint8)42U)
#define PF3               ((uint8)43U)
#define PF4               ((uint8)44U)
   
#define MAX_PIN_NUM       ((uint8)44U)
#define MAX_MODE_NUM      ((uint8)15U)
/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/

/* Description: Uint8 to hold Port Type */
typedef uint8 Port_NumType;

/* Description: Uint8 to hold PIN Type */
typedef uint8 Port_PinType;

/* Description: Enum to hold PIN direction */
typedef enum
{
     PORT_PIN_IN,
     PORT_PIN_OUT	
}Port_PinDirectionType;

/* Description: Uint8 to hold PIN Mode Type */
typedef uint8 Port_PinModeType;

/* Description: Enum to Selsect Alternative function for PIN */
typedef enum
{
    UN_ALT,ALT
}Port_PinAlternative;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistor;

/* Description: Enum to hold Initial Value for PIN */
typedef enum
{
    PIN_LOW,
    PIN_HIGH
}Port_PinValue;

/* Description: Enum to hold PIN Changable Type */
typedef enum
{
    UN_CHANGE,CHANGE
}Port_PinChangeType;

/* Description: Structure to configure each individual PIN:
 *	1. the PORT Which the pin belongs to. 0, 1, 2, 3, 4 or 5
 *	2. the number of the pin in the PORT.
 *      3. the direction of pin --> INPUT or OUTPUT
 *      4. the Mode of Pin in Port
 *      5. the Alternative Function of Pin in Port
 *      6. the internal resistor --> Disable, Pull up or Pull down
 *      7. the initail value --> PIN_LOW, PIN_HIGH
 *      8. the Status of Pin --> CHANGE, UN_CHANGE
 */

typedef struct
{
	Port_NumType           port_num;
	Port_PinType           pin_num;
	Port_PinDirectionType  direction;
        Port_PinAlternative    alter;
	Port_PinModeType       pin_mod;
        Port_InternalResistor  resistor;
        Port_PinValue          initial_value;
        Port_PinChangeType     change_pin;
}Port_ConfigType;

#include "Port_PBcfg.h"
/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/************************************************************************************
* Service Name: Port_Init
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Initializes the Port Driver module.
************************************************************************************/
void Port_Init(const Port_ConfigType *ConfigPtr );



/************************************************************************************
* Service Name: Port_SetPinDirection
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): -Pin        --> Port Pin ID numb
*                  -Direction  --> Port Pin Direction
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin direction
************************************************************************************/
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);



/************************************************************************************
* Service Name: Port_RefreshPortDirection
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Refreshes port direction
************************************************************************************/
void Port_RefreshPortDirection(void);



/************************************************************************************
* Service Name: Port_GetVersionInfo
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): versioninfo --> Pointer to where to store the version information of this module
* Return value: None
* Description: Returns the version information of this module.
************************************************************************************/
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);



/************************************************************************************
* Service Name: Port_SetPinMode
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): -Pin        --> Port Pin ID numb
*                  -Mode       --> New Port Pin mode to be set on port pin.
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin mode
************************************************************************************/
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

#endif /* PORT_H */
