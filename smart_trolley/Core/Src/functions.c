/*
 * functions.c
 *
 *  Created on: Feb 15, 2020
 *      Author: Adam
 */

#include "functions.h"

void delay (uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim1))<us);
}

void delay1 (uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim3))<us);
}

uint32_t receive_data (void)
{
	uint32_t code=0;

		  /* The START Sequence begin here
	   * there will be a pulse of 9ms LOW and
	   * than 4.5 ms space (HIGH)
	   */
	  while (!(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin)));  // wait for the pin to go high.. 9ms LOW

	  while ((HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin)));  // wait for the pin to go low.. 4.5ms HIGH

	  /* START of FRAME ends here*/

	  /* DATA Reception
	   * We are only going to check the SPACE after 562.5us pulse
	   * if the space is 562.5us, the bit indicates '0'
	   * if the space is around 1.6ms, the bit is '1'
	   */

	  for (int i=0; i<32; i++)
	  {
		  count=0;

		  while (!(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin))); // wait for pin to go high.. this is 562.5us LOW

		  while ((HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin)))  // count the space length while the pin is high
		  {
			  count++;
			  delay1(100);
		  }

		  if (count > 12) // if the space is more than 1.2 ms
		  {
			  code |= (1UL << (31-i));   // write 1
		  }

		  else code &= ~(1UL << (31-i));  // write 0
	  }

		return code;
}

int convert_code (uint32_t code)
{
	int wynik;
	code = code - 16700000;

	if(code == 24175)
	{
		wynik = 1;
	}
	else if (code == 18055)
	{
		wynik = 2;
	}
	else if (code == 43045)
	{
		wynik = 3;
	}
	else if (code == 16015)
	{
		wynik = 4;
	}
	else if (code == 26215)
	{
		wynik = 5;
	}
	else if (code == 34885)
	{
		wynik = 6;
	}
	else if (code == 28765)
	{
		wynik = 7;
	}
	else if (code == 30805)
	{
		wynik = 8;
	}
	else if (code == 32845)
	{
		wynik = 9;
	}
	else if (code == 38455)
	{
		wynik = 0;
	}
	else if (code == 36925)
	{
		wynik = 100; //exit
	}
	else
	{
		wynik = 10;
	}

	return wynik;
}

uint32_t hcsr04_read (void)
{
	local_time=0;
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_RESET);  // pull the TRIG pin HIGH
	delay(2);  // wait for 2 us


	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low

	// read the time for which the pin is high

	while (!(HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin)));  // wait for the ECHO pin to go high
	while (HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin))    // while the pin is high
	 {
		local_time++;   // measure time for which the pin is high
		delay (1);
	 }
	return local_time;
}

void screenON(int number)
{
			if(number == 1)
			{
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
			}
			else if(number == 2)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
			}
			else if(number == 3)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
			}
			else if(number == 4)
			{
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
			}
			else if(number == 5)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
			}
			else if(number == 6)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
			}
			else if(number == 7)
			{
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
			}
			else if(number == 8)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
			}
			else if(number == 9)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
			}
			else if(number == 0)
			{
				HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
			}
			else;
}

void screenOFF(void)
{
					HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);

}

void display(int cyfra, int czas)
{
	HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_RESET);

	if(cyfra <10)
	{
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
		screenON(cyfra);
		HAL_Delay(czas);
	}
	else if (cyfra >= 10 && cyfra < 20)
	{
		for(long i = 0 ; i<czas/4 ; i++)
		{
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
		screenON(1);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
		screenON(cyfra-10);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
		}
	}
	else if (cyfra >= 20 && cyfra < 30)
	{
		for(long i = 0 ; i<czas/4 ; i++)
		{
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
		screenON(2);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
		screenON(cyfra-20);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
		}
	}
	else if (cyfra >= 30 && cyfra < 40)
	{
		for(long i = 0 ; i<czas/4 ; i++)
		{
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
		screenON(3);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
		screenON(cyfra-30);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
		}
	}
	else if (cyfra >= 40 && cyfra < 50)
	{
		for(long i = 0 ; i<czas/4 ; i++)
		{
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
		screenON(4);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
		screenON(cyfra-40);
		HAL_Delay(1);
		screenOFF();
		HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
		}
	}
	else;
}

void servo_forward()
{
	htim4.Instance->CCR1 = 18600;
}

void servo_backward()
{
	htim4.Instance->CCR1 = 18300; //1700
}

void servo_stop()
{
	htim4.Instance->CCR1 = 0;
}
