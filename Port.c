 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mohamed Faryed
 ******************************************************************************/

#include "Port.h"
#include "tm4c123gh6pm_registers.h"

#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Dio Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 || ( DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 || ( DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif
 
STATIC const Port_ConfigType * PORT_PinConfig = NULL_PTR;
STATIC uint8 PORT_Status                      = PORT_NOT_INITIALIZED;
STATIC Port_PinChangeType PortPinChangeable[PIN_CONFIG_SIZE] ={UN_CHANGE};
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
void Port_Init(const Port_ConfigType * ConfigPtr )
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID,
		     PORT_E_PARAM_CONFIG_ID);
	}
	else{}
#endif
	{
		/*
		 * Set the module state to initialized and point to the PB configuration structure using a global pointer.
		 * This global pointer is global to be used by other functions to read the PB configuration structures
		 */
		PORT_Status       = PORT_INITIALIZED;
		PORT_PinConfig    = ConfigPtr; /* address of the first Channels structure --> Channels[0] */
	}
        
  volatile uint32 * PortGpio_Ptr = NULL_PTR;   /* point to the required Port Registers base address */
  volatile uint32 delay = 0;
  for(uint8 i=0;i<PIN_CONFIG_SIZE;i++)
  {
    switch(ConfigPtr[i].port_num)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_REGCGC2_REG |= (1<<ConfigPtr[i].port_num);
    delay = SYSCTL_REGCGC2_REG;
    
    if( ((ConfigPtr[i].port_num == 3) && (ConfigPtr[i].pin_num == 7)) || ((ConfigPtr[i].port_num == 5) && (ConfigPtr[i].pin_num == 0)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                       /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , ConfigPtr[i].pin_num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (ConfigPtr[i].port_num == 2) && (ConfigPtr[i].pin_num <= 3) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    
    if(ConfigPtr[i].alter == UN_ALT)
    {
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , ConfigPtr[i].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */\
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , ConfigPtr[i].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    }
    else
    {
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , ConfigPtr[i].pin_num);               /* enable Alternative function for this pin by set the corresponding bit in GPIOAFSEL register */
      if(ConfigPtr[i].pin_mod == GPIO_MODE)
      {
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , ConfigPtr[i].pin_num);      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
      }
      else
      {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , ConfigPtr[i].pin_num);     /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      }
    }
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~((0x0000000F) << (ConfigPtr[i].pin_num * 4));              /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= ((ConfigPtr[i].pin_mod) << (ConfigPtr[i].pin_num * 4));     /* set mode the PMCx bits for this pin */

    if(ConfigPtr[i].direction == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , ConfigPtr[i].pin_num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        
        if(ConfigPtr[i].initial_value == PIN_HIGH)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , ConfigPtr[i].pin_num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , ConfigPtr[i].pin_num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
        }
    }
    else if(ConfigPtr[i].direction == PORT_PIN_IN)
    {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , ConfigPtr[i].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        
        if(ConfigPtr[i].resistor == PULL_UP)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , ConfigPtr[i].pin_num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
        }
        else if(ConfigPtr[i].resistor == PULL_DOWN)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , ConfigPtr[i].pin_num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , ConfigPtr[i].pin_num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , ConfigPtr[i].pin_num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , ConfigPtr[i].pin_num);        /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
    PortPinChangeable[i]=ConfigPtr[i].change_pin;
  }
}

#if ( PORT_SET_PIN_DIRECTION_API == STD_ON)
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
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,
		     PORT_E_UNINIT_ID);
	}
	else{}
        
	/* check if the Pin input > MAX_PIN_NUM */
	if (Pin > MAX_PIN_NUM)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,
		     PORT_E_PARAM_PIN_ID);
	}
	else{}
        
        /* check if Port Pin not configured as changeable */
	if (PortPinChangeable[Pin] == UN_CHANGE)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,
		     PORT_E_DIRECTION_UNCHANGEABLE_ID);
	}
	else{}
        
        /* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID, 
                                PORT_E_UNINIT_ID);
	}
	else{}
        
#endif          
          
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile uint32 delay = 0;
    volatile uint8  PortX = 0;
    
    switch(Pin)
    {
        case  PA0:
        case  PA1:
        case  PA2:
        case  PA3:
        case  PA4:
        case  PA5:
        case  PA6:
        case  PA7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
                   PortX=0;
                   Pin =Pin -(8*PortX);
		     break;
        case  PB0:
        case  PB1:         
        case  PB2:         
        case  PB3:         
        case  PB4:         
        case  PB5:         
        case  PB6:         
        case  PB7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
                   PortX=1;
                   Pin =Pin -(8*PortX);
		     break;             
        case  PC4:             
        case  PC5:             
        case  PC6:             
        case  PC7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
                   PortX=2;
                   Pin =Pin -(8*PortX);
		     break;
        case  PD0:            
        case  PD1:
        case  PD2:            
        case  PD3:            
        case  PD4:            
        case  PD5:            
        case  PD6:            
        case  PD7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
                   PortX=3;
                   Pin =Pin -(8*PortX);
		     break;
        case  PE0:             
        case  PE1:             
        case  PE2:             
        case  PE3:             
        case  PE4:             
        case  PE5:             
        case  PE6:             
        case  PE7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
                   PortX=4;
                   Pin =Pin -(8*PortX);
		     break;
        case  PF0:
        case  PF1:             
        case  PF2:             
        case  PF3:             
        case  PF4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
                   PortX=5;
                   Pin =Pin -(8*PortX);
		     break;
    }
    
    if( ((PortX == 3) && (Pin == 7)) || ((PortX == 5) && (Pin == 0)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;         /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , Pin);     /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortX == 2) && (Pin <= 3) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }

    if(Direction == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Pin);           /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
    }
    else if(Direction == PORT_PIN_IN)
    {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , Pin);         /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , Pin);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Pin);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
    }
    else
    {
        /* Do Nothing */
    }
    
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Pin);        /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
  
}
#endif


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
void Port_RefreshPortDirection(void)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PORT_DIRECTION_SID, 
                                PORT_E_UNINIT_ID);
	}
	else{}
        
        /* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PORT_DIRECTION_SID, 
                                PORT_E_UNINIT_ID);
	}
	else{}
#endif
          
  volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
  volatile uint32 delay = 0;
  for(uint8 i=0;i<PIN_CONFIG_SIZE;i++)
  {
    switch(PORT_PinConfig[i].port_num)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_REGCGC2_REG |= (1<<PORT_PinConfig[i].port_num);
    delay = SYSCTL_REGCGC2_REG;
    
    if( ((PORT_PinConfig[i].port_num == 3) && (PORT_PinConfig[i].pin_num == 7)) || ((PORT_PinConfig[i].port_num == 5) && (PORT_PinConfig[i].pin_num == 0)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                            /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , PORT_PinConfig[i].pin_num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PORT_PinConfig[i].port_num == 2) && (PORT_PinConfig[i].pin_num <= 3) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    
    if(PORT_PinConfig[i].alter == UN_ALT)
    {
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PORT_PinConfig[i].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */\
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PORT_PinConfig[i].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    }
    else
    {
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PORT_PinConfig[i].pin_num);             /* enable Alternative function for this pin by set the corresponding bit in GPIOAFSEL register */
      if(PORT_PinConfig[i].pin_mod == GPIO_MODE)
      {
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PORT_PinConfig[i].pin_num);    /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
      }
      else
      {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PORT_PinConfig[i].pin_num);  /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      }
    }
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~((0x0000000F) << (PORT_PinConfig[i].pin_num * 4));   /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= ((PORT_PinConfig[i].pin_mod) << (PORT_PinConfig[i].pin_num * 4));     /* set mode the PMCx bits for this pin */

    if(PORT_PinConfig[i].direction == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PORT_PinConfig[i].pin_num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        
        if(PORT_PinConfig[i].initial_value == PIN_HIGH)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PORT_PinConfig[i].pin_num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PORT_PinConfig[i].pin_num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
        }
    }
    else if(PORT_PinConfig[i].direction == PORT_PIN_IN)
    {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PORT_PinConfig[i].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        
        if(PORT_PinConfig[i].resistor == PULL_UP)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PORT_PinConfig[i].pin_num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
        }
        else if(PORT_PinConfig[i].resistor == PULL_DOWN)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PORT_PinConfig[i].pin_num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PORT_PinConfig[i].pin_num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PORT_PinConfig[i].pin_num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PORT_PinConfig[i].pin_num);        /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
  }
}


#if (PORT_VERSION_INFO_API == STD_ON)
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
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* Check if input pointer is not Null pointer */
	if(NULL_PTR == versioninfo)
	{
		/* Report to DET  */
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID,
                                PORT_E_PARAM_POINTER_ID);
	}
	else{}
        
        /* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID, 
                                PORT_E_UNINIT_ID);
	}
	else{}
        
#endif /* (PORT_DEV_ERROR_DETECT == STD_ON) */
	{
		/* Copy the vendor Id */
		versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
		/* Copy the module Id */
		versioninfo->moduleID = (uint16)PORT_MODULE_ID;
		/* Copy Software Major Version */
		versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
		/* Copy Software Minor Version */
		versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
		/* Copy Software Patch Version */
		versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
	}
}
#endif


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
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (PORT_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,
		     PORT_E_UNINIT_ID);
	}
	else{}
        
	/* check if the input Pin > MAX_PIN_NUM */
	if (Pin > MAX_PIN_NUM)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,
		     PORT_E_PARAM_PIN_ID);
	}
	else{}
        
        /* check if the input Mode > MAX_MODE_NUM */
	if (Mode > MAX_MODE_NUM)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,
		     PORT_E_PARAM_INVALID_MODE_ID);
	}
	else{}
        
        /* check if Port Pin not configured as changeable */
	if (PortPinChangeable[Pin] == UN_CHANGE)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,
		     PORT_E_MODE_UNCHANGEABLE_ID);
	}
	else{}
        
#endif

          
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile uint32 delay = 0;
    volatile uint8  PortX = 0;
    
    switch(Pin)
    {
        case  PA0:
        case  PA1:
        case  PA2:
        case  PA3:
        case  PA4:
        case  PA5:
        case  PA6:
        case  PA7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
                   PortX=0;
                   Pin =Pin -(8*PortX);
		     break;
        case  PB0:
        case  PB1:         
        case  PB2:         
        case  PB3:         
        case  PB4:         
        case  PB5:         
        case  PB6:         
        case  PB7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
                   PortX=1;
                   Pin =Pin -(8*PortX);
		     break;             
        case  PC4:             
        case  PC5:             
        case  PC6:             
        case  PC7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
                   PortX=2;
                   Pin =Pin -(8*PortX);
		     break;
        case  PD0:            
        case  PD1:
        case  PD2:            
        case  PD3:            
        case  PD4:            
        case  PD5:            
        case  PD6:            
        case  PD7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
                   PortX=3;
                   Pin =Pin -(8*PortX);
		     break;
        case  PE0:             
        case  PE1:             
        case  PE2:             
        case  PE3:             
        case  PE4:             
        case  PE5:             
        case  PE6:             
        case  PE7: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
                   PortX=4;
                   Pin =Pin -(8*PortX);
		     break;
        case  PF0:
        case  PF1:             
        case  PF2:             
        case  PF3:             
        case  PF4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
                   PortX=5;
                   Pin =Pin -(8*PortX);
		     break;
    }
    
    if( ((PortX == 3) && (Pin == 7)) || ((PortX == 5) && (Pin == 0)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;         /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , Pin);     /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortX == 2) && (Pin <= 3) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }

    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~((0x0000000F) << (Pin * 4));   /* Clear the PMCx bits for this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= ((Mode) << (Pin * 4));     /* set mode the PMCx bits for this pin */

    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Pin);        /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */

}

