/*
 * vehiculo.c
 *
 *  Created on: Jun 28, 2021
 *      Author: Elias
 */

#include "vehiculo.h"
void vehiculo(int accion){
	if(accion==0){
		/* entonces avanza */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
	}
	if(accion==1){
		/* entonces retrocede */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
	}
	if(accion==2){
		/* entonces detente */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
	}
	else{
		/* entonces detente */
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
	}
}
