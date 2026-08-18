/**********************************************************************************************************
 *  Marturion Electronics Ltd
 *
 *  Knockmore Hill Business Park
 *  9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  tst_ltc2943.c
 * Date Created:  Wed 27 Sep 2017 10:18:01 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "app_battery_fuel_guage.h"
#include "tst_ltc2943.h"
#include "hal_ltc2943.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"

/**********************************************************************************************************
**********************************************************************************************************/



/*************************************************************************************************
* Function Name : 	tst_LTC2943_menu
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t tst_LTC2943_menu(void)
{
/* Local Variables */
	uint8_t				i2c_status;
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;

/* Code */
	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){

		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST LTC2943 menu****");
				printf("\r\nEsc  Exit Menu");
				printf("\r\ni    init");
				printf("\r\np    reg print");

				printf("\r\ns    save");
				printf("\r\nn    New Battery");
				printf("\r\n0    Set Battery at 0.55%%");
				printf("\r\n1    Set Battery at 2.1%%");
				printf("\r\n2    Set Battery at 10.1%%");
				printf("\r\n3    Set Battery at 21.1%%");
				printf("\r\n9    set battery at 100%%");
				break;

			case 0x1b:
				return(0);
				//break;
			case 'i':	hal_ltc2943_init();				break;
			case 'P':	tst_LTC2943_reg_print();		break;
			case 'p':	tst_LTC2943_print();			break;

			case '0':
				printf("\r\nSet battery capacity at (0.55%% charged)");
				hal_ltc2943_set_charge(0.0055);
				break;
			case '1':
				printf("\r\nSet battery capacity at (2.1%% charged)");
				hal_ltc2943_set_charge(0.021);
				break;
			case '2':
				printf("\r\nSet battery capacity at (10.1%% charged)");
				hal_ltc2943_set_charge(0.101);
				break;
			case '3':
				printf("\r\nSet battery capacity at (21.1%% charged)");
				hal_ltc2943_set_charge(0.211);
				break;
			case '9':
				printf("\r\nSet battery capacity at (fully charged)");
				hal_ltc2943_set_charge(1.0);
				break;

			case 's':
				printf("\r\nSave settings");
				i2c_status	=  app_battery_fuel_save();
				if(i2c_status != 0){
					printf("\r\ni2c status err");
				}
				break;
			case 'n':
				printf("\r\nNew Battery");
				i2c_status	=  app_battery_new_battery();
				if(i2c_status != 0){
					printf("\r\ni2c status err");
				}
				break;
			default:
					printf("\r\n Invalid Command");
					break;
		}
	}
	return(1);
}

/*************************************************************************************************
* Function Name : 	tst_LTC2943_reg_print
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/09/17	W. Paul			Created
*************************************************************************************************/
void tst_LTC2943_reg_print(void)
{
	uint8_t		i2c_status	=  0;

	i2c_status	+= hal_ltc2943_status_read();
 	i2c_status	+= hal_ltc2943_control_read();

	i2c_status	+= hal_ltc2943_charge_read();
	i2c_status	+= hal_ltc2943_charge_threshold_read();

	i2c_status	+= hal_ltc2943_voltage_read();
	i2c_status	+= hal_ltc2943_voltage_threshold_read();

	i2c_status	+= hal_ltc2943_current_read();
	i2c_status	+= hal_ltc2943_current_threshold_read();

	i2c_status	+= hal_ltc2943_temp_read();
	i2c_status	+= hal_ltc2943_temp_threshold_read();

	if(i2c_status != 0){
		printf("\r\ni2c_status not 0 (%d)",i2c_status);
	}

	printf("\r\nstatus  0x%x",(uint8_t)FuelGaugeReg.status.a);
	printf("\r\ncontrol 0x%x",FuelGaugeReg.control);

    printf("\r\ncharge");
	printf("\r\n val     0x%x",FuelGaugeReg.charge.a);						// charge
	printf("\r\n thres h 0x%x",FuelGaugeReg.charge_thres_h.a);			//Charge threshold high
	printf("\r\n thres l 0x%x",FuelGaugeReg.charge_thres_l.a);			//Charge threshold low

	printf("\r\nvoltage");
	printf("\r\n val     0x%x",FuelGaugeReg.voltage.a);					//Voltage
	printf("\r\n thres h 0x%x",FuelGaugeReg.voltage_thres_h.a);			//Voltage threshold high
	printf("\r\n thres l 0x%x",FuelGaugeReg.voltage_thres_l.a);				//Voltage threshold low

	printf("\r\ncurrent");
	printf("\r\n val     0x%x",FuelGaugeReg.current.a);					//Current
	printf("\r\n thres h 0x%x",FuelGaugeReg.current_thres_h.a);			//Current threshold high
	printf("\r\n thres l 0x%x",FuelGaugeReg.current_thres_l.a);			//Current threshold low

	printf("\r\ntemperature");
	printf("\r\n val     0x%x",FuelGaugeReg.temperature.a);				//Temperature
	printf("\r\n thres h 0x%x",FuelGaugeReg.temperature_thres_h);
	printf("\r\n thres l 0x%x",FuelGaugeReg.temperature_thres_l);

	return;
}

/*************************************************************************************************
* Function Name : 	tst_LTC2943_print
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
void tst_LTC2943_print(void)
{
	uint8_t		i2c_status	=  0;

	i2c_status	+= hal_ltc2943_charge_read();
	i2c_status	+= hal_ltc2943_voltage_read();
	i2c_status	+= hal_ltc2943_current_read();
	i2c_status	+= hal_ltc2943_temp_read();

	if(i2c_status != 0){
		printf("\r\ni2c_status not 0 (%d)",i2c_status);
	}

	printf("\r\n mAh         %f"				,FuelGauge.act_mAh);
	printf("\r\n Voltage     %f"				,FuelGauge.act_voltage);
	printf("\r\n Current     %f av %f"			,FuelGauge.act_current
												,FuelGauge.av_current.av		);
	printf("\r\n Temperature %f"				,FuelGauge.act_temperature);

	printf("\r\n charging    %d"				,FuelGauge.charging);// Indication if the battery is charging or not
	printf("\r\n percentage  %f%%"				,FuelGauge.percentage);// Indicates the battery Charge percentage
	printf("\r\n t remaining %ds (%dh%2dm%2ds)"	,FuelGauge.sec_remaining
												,FuelGauge.sec_remaining / 3600
												,(FuelGauge.sec_remaining%3600) / 60
												,FuelGauge.sec_remaining % 60);

	return;
}
/**********************************************************************************************************
**********************************************************************************************************/
//end of file
