/**
 * The MIT License (MIT)
 *
 * Author: Junjie Chen (595355940@qq.com)
 *
 * Copyright (C) 2020  Seeed Technology Co.,Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Wire.h>
#include <Arduino.h>
#include "Seeed_Arduino_MLX9064x/src/MLX9064X_I2C_Driver.h"
#include "Seeed_Arduino_MLX9064x/src/MLX90641_API.h"
#include "Seeed_Arduino_MLX9064x/src/MLX90640_API.h"

extern "C"{
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
}

#define TA_SHIFT 8 //Default shift for MLX90641 in open air

int errorno = 0;

typedef struct _mlx90641_obj_t
{
    const byte MLX90641_address = 0x33; //Default 7-bit unshifted address of the MLX90641
    paramsMLX90641 MLX90641;
} mlx90641_obj_t;

mlx90641_obj_t mlx9064x;

extern "C"{
    void common_hal_mlx9064x_construct(abstract_module_t * self){
        Wire.begin();
        Wire.setClock(400000); //Increase I2C clock speed to 400kHz
        uint16_t eeMLX90641[832];
        MLX90641_DumpEE(mlx9064x.MLX90641_address, eeMLX90641);
        MLX90641_ExtractParameters(eeMLX90641, &(mlx9064x.MLX90641));
    }
    void common_hal_mlx9064x_deinit(abstract_module_t * self){
        Wire.end();
    }
    void common_hal_mlx9064x_get_frame_data(abstract_module_t * self , float * result){
        uint16_t MLX90641Frame[242];
        for (byte x = 0 ; x < 2 ; x++) {
            int status = MLX90641_GetFrameData(mlx9064x.MLX90641_address, MLX90641Frame);
            float vdd = MLX90641_GetVdd(MLX90641Frame, &(mlx9064x.MLX90641));
            float Ta = MLX90641_GetTa(MLX90641Frame, &(mlx9064x.MLX90641));
            float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
            float emissivity = 0.95;
            MLX90641_CalculateTo(MLX90641Frame, &(mlx9064x.MLX90641), emissivity, tr, result);
        }
    }    
}