/**
 * @file       ws28-presets.h
 * @brief      Logging API
 *
 * @author     Oleksandr Kholiavko <HalavicH@gmail.com>
 */

#ifndef INC_WS28_PRESETS_H_
#define INC_WS28_PRESETS_H_

#include "ws2812b.h"

/*
   Params:
    RGB - array of uint8_t RGB colours
 */

extern uint8_t ws28_comet_condition;

void ws28_comet(ws28_data_st_t *ws28_data, uint8_t *RGB);

#endif /* INC_WS28_PRESETS_H_ */

