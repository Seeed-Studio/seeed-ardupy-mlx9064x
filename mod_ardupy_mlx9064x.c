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

#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "shared-bindings/util.h"

void common_hal_mlx9064x_construct(abstract_module_t * self);
void common_hal_mlx9064x_get_frame_data(abstract_module_t * self , float * result);
void common_hal_mlx9064x_deinit(abstract_module_t * self);
void common_hal_mlx9064x_set_refresh_rate(abstract_module_t * self , uint8_t rate);

extern const mp_obj_type_t grove_mlx9064x_type;

float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f);
void get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(float *src, uint8_t src_rows, uint8_t src_cols,
                       float *dest, uint8_t dest_rows, uint8_t dest_cols);

float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y)
{
    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (x >= cols)
    {
        x = cols - 1;
    }
    if (y >= rows)
    {
        y = rows - 1;
    }
    return p[y * cols + x];
}

void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f)
{
    if ((x < 0) || (x >= cols))
    {
        return;
    }
    if ((y < 0) || (y >= rows))
    {
        return;
    }
    p[y * cols + x] = f;
}

// src is a grid src_rows * src_cols
// dest is a pre-allocated grid, dest_rows*dest_cols
void interpolate_image(float *src, uint8_t src_rows, uint8_t src_cols,
                       float *dest, uint8_t dest_rows, uint8_t dest_cols)
{
    float mu_x = (src_cols - 1.0) / (dest_cols - 1.0);
    float mu_y = (src_rows - 1.0) / (dest_rows - 1.0);

    float adj_2d[16]; // matrix for storing adjacents

    for (uint8_t y_idx = 0; y_idx < dest_rows; y_idx++)
    {
        for (uint8_t x_idx = 0; x_idx < dest_cols; x_idx++)
        {
            float x = x_idx * mu_x;
            float y = y_idx * mu_y;
            //  Serial.print("("); Serial.print(y_idx); Serial.print(", "); Serial.print(x_idx); Serial.print(") = ");
            //  Serial.print("("); Serial.print(y); Serial.print(", "); Serial.print(x); Serial.print(") = ");
            get_adjacents_2d(src, adj_2d, src_rows, src_cols, x, y);

            //  Serial.print("[");
            //  for (uint8_t i=0; i<16; i++) {
            //    Serial.print(adj_2d[i]); Serial.print(", ");
            //  }
            //  Serial.println("]");

            float frac_x = x - (int)x; // we only need the ~delta~ between the points
            float frac_y = y - (int)y; // we only need the ~delta~ between the points
            float out = bicubicInterpolate(adj_2d, frac_x, frac_y);
            //  Serial.print("\tInterp: "); Serial.println(out);
            set_point(dest, dest_rows, dest_cols, x_idx, y_idx, out);
        }
    }
}

// p is a list of 4 points, 2 to the left, 2 to the right
float cubicInterpolate(float p[], float x)
{
    float r = p[1] + (0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0]))));

    // Serial.print("interpolating: [");
    // Serial.print(p[0],2); Serial.print(", ");
    // Serial.print(p[1],2); Serial.print(", ");
    // Serial.print(p[2],2); Serial.print(", ");
    // Serial.print(p[3],2); Serial.print("] w/"); Serial.print(x); Serial.print(" = ");
    // Serial.println(r);

    return r;
}

// p is a 16-point 4x4 array of the 2 rows & columns left/right/above/below
float bicubicInterpolate(float p[], float x, float y)
{
    float arr[4] = {0, 0, 0, 0};
    arr[0] = cubicInterpolate(p + 0, x);
    arr[1] = cubicInterpolate(p + 4, x);
    arr[2] = cubicInterpolate(p + 8, x);
    arr[3] = cubicInterpolate(p + 12, x);
    return cubicInterpolate(arr, y);
}

// src is rows*cols and dest is a 4-point array passed in already allocated!
void get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y)
{
    // Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");
    // pick two items to the left
    dest[0] = get_point(src, rows, cols, x - 1, y);
    dest[1] = get_point(src, rows, cols, x, y);
    // pick two items to the right
    dest[2] = get_point(src, rows, cols, x + 1, y);
    dest[3] = get_point(src, rows, cols, x + 2, y);
}

// src is rows*cols and dest is a 16-point array passed in already allocated!
void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y)
{
    // Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");
    float arr[4];
    for (int8_t delta_y = -1; delta_y < 3; delta_y++)
    {                                          // -1, 0, 1, 2
        float *row = dest + 4 * (delta_y + 1); // index into each chunk of 4
        for (int8_t delta_x = -1; delta_x < 3; delta_x++)
        { // -1, 0, 1, 2
            row[delta_x + 1] = get_point(src, rows, cols, x + delta_x, y + delta_y);
        }
    }
}

m_generic_make(mlx9064x) {
    abstract_module_t * self = new_abstruct_module(type);
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    common_hal_mlx9064x_construct(self);
    return self;
}

mp_obj_t  mlx9064x_get_frame_data(mp_obj_t self_in){
    abstract_module_t * self = (abstract_module_t *)self_in;  
    int MLX9064xSize = 192;
    float MLX90641To[MLX9064xSize];
    int width = 40;
    int height = 40;
    float *dest = (float *)m_malloc(width * height * sizeof(float));
    mp_obj_t *ret_val = (mp_obj_t *)m_malloc(width * height * sizeof(mp_obj_t));
    common_hal_mlx9064x_get_frame_data(self,MLX90641To);
    interpolate_image(MLX90641To, 12, 16, dest, width, height);
    for (int i = 0; i < width * height; i++)
    {
        ret_val[i] = mp_obj_new_float(dest[i]);
    }
    mp_obj_t ret = mp_obj_new_list(width * height, ret_val);
    m_free(ret_val);
    return ret;
}
MP_DEFINE_CONST_FUN_OBJ_1(mlx9064x_get_frame_data_obj, mlx9064x_get_frame_data);

mp_obj_t  mlx9064x_get_raw_data(mp_obj_t self_in){
    abstract_module_t * self = (abstract_module_t *)self_in;  
    int MLX9064xSize = 192;
    float MLX90641To[MLX9064xSize];
    mp_obj_t *ret_val = (mp_obj_t *)m_malloc(MLX9064xSize * sizeof(mp_obj_t));
    common_hal_mlx9064x_get_frame_data(self,MLX90641To);
    for (int i = 0; i < MLX9064xSize; i++)
    {
        ret_val[i] = mp_obj_new_float(MLX90641To[i]);
    }
    mp_obj_t ret = mp_obj_new_list(MLX9064xSize, ret_val);
    m_free(ret_val);
    return ret;
}
MP_DEFINE_CONST_FUN_OBJ_1(mlx9064x_get_raw_data_obj, mlx9064x_get_raw_data);


mp_obj_t  mlx9064x_set_refresh_rate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    abstract_module_t *self = (abstract_module_t *)pos_args[0];
    uint8_t SampleRate = mp_obj_get_int(pos_args[1]);
    common_hal_mlx9064x_set_refresh_rate(self,SampleRate);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mlx9064x_set_refresh_rate_obj,1,mlx9064x_set_refresh_rate);

const mp_rom_map_elem_t mlx9064x_rate_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR__0_5HZ), MP_ROM_INT(0x00)},
    {MP_ROM_QSTR(MP_QSTR__1HZ), MP_ROM_INT(0X01)},
    {MP_ROM_QSTR(MP_QSTR__2HZ), MP_ROM_INT(0X02)},
    {MP_ROM_QSTR(MP_QSTR__4HZ), MP_ROM_INT(0X03)},
    {MP_ROM_QSTR(MP_QSTR__8HZ), MP_ROM_INT(0X04)},
};
MP_DEFINE_CONST_DICT(mlx9064x_rate_locals_dict, mlx9064x_rate_locals_dict_table);

static const mp_obj_type_t mlx9064x_rate_type = {
    {&mp_type_type},
    .name = MP_QSTR_rate,
    .locals_dict = (mp_obj_t)&mlx9064x_rate_locals_dict,
};

const mp_rom_map_elem_t mlx9064x_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_deinit),    MP_ROM_PTR(&mlx9064x_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),  MP_ROM_PTR(&mlx9064x_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_get_frame_data),  MP_ROM_PTR(&mlx9064x_get_frame_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_data),  MP_ROM_PTR(&mlx9064x_get_raw_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_refresh_rate),  MP_ROM_PTR(&mlx9064x_set_refresh_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_rate), MP_ROM_PTR(&mlx9064x_rate_type) },
};

MP_DEFINE_CONST_DICT(mlx9064x_locals_dict, mlx9064x_locals_dict_table);

const mp_obj_type_t grove_mlx9064x_type = {
    {&mp_type_type},
    .name = MP_QSTR_grove_mlx9064x,
    .make_new = mlx9064x_make_new,
    .locals_dict = (mp_obj_t)&mlx9064x_locals_dict,
};
