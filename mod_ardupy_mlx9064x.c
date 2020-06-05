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

extern const mp_obj_type_t grove_mlx9064x_type;

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
    mp_obj_t *ret_val = (mp_obj_t *)m_malloc(MLX9064xSize * sizeof(mp_obj_t));
    common_hal_mlx9064x_get_frame_data(self,MLX90641To);
    for (int i = 0; i < MLX9064xSize; i++)
    {
        ret_val[i] = mp_obj_new_float(MLX90641To[i]);
    }
    mp_obj_t ret = mp_obj_new_tuple(MLX9064xSize, ret_val);
    m_free(ret_val);
    return ret;
}
MP_DEFINE_CONST_FUN_OBJ_1(mlx9064x_get_frame_data_obj, mlx9064x_get_frame_data);

mp_obj_t  mlx9064x_obj_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest){
    abstract_module_t * self = (abstract_module_t *)self_in;  
}   

const mp_rom_map_elem_t mlx9064x_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_deinit),    MP_ROM_PTR(&mlx9064x_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),  MP_ROM_PTR(&mlx9064x_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_get_frame_data),  MP_ROM_PTR(&mlx9064x_get_frame_data_obj) },
};

MP_DEFINE_CONST_DICT(mlx9064x_locals_dict, mlx9064x_locals_dict_table);

const mp_obj_type_t grove_mlx9064x_type = {
    {&mp_type_type},
    .name = MP_QSTR_grove_mlx9064x,
    .make_new = mlx9064x_make_new,
    .locals_dict = (mp_obj_t)&mlx9064x_locals_dict,
    .attr = &mlx9064x_obj_attr,
};
