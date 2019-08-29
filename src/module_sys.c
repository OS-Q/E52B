/**
 * @section License
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016-2017, Erik Moqvist
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * Standard input.
 */
struct class_input_t {
    mp_obj_base_t base;
};

/**
 * Standard output and error.
 */
struct class_output_t {
    mp_obj_base_t base;
};

static void class_input_print(const mp_print_t *print_p,
                              mp_obj_t self_in,
                              mp_print_kind_t kind)
{
    mp_printf(print_p, "<io.Input>");
}

static mp_uint_t class_input_read(mp_obj_t self_in,
                                  void *buf_p,
                                  mp_uint_t size,
                                  int *errcode_p)
{
    int i;
    int c;

    for (i = 0; i < size; i++) {
        c = mp_hal_stdin_rx_chr();

        if (c == '\r') {
            c = '\n';
        }

        ((byte*)buf_p)[i] = c;
    }

    return (size);
}

static void class_output_print(const mp_print_t *print_p,
                               mp_obj_t self_in,
                               mp_print_kind_t kind)
{
    mp_printf(print_p, "<io.Output>");
}

static mp_uint_t class_output_write(mp_obj_t self_in,
                                    const void *buf_p,
                                    mp_uint_t size,
                                    int *errcode_p)
{
    mp_hal_stdout_tx_strn_cooked(buf_p, size);
    
    return (size);
}

static mp_obj_t sys_obj___exit__(mp_uint_t n_args, const mp_obj_t *args)
{
    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sys_obj___exit___obj, 4, 4, sys_obj___exit__);

static const mp_rom_map_elem_t class_input_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&sys_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(class_input_locals_dict, class_input_locals_dict_table);

static const mp_stream_p_t class_input_stream = {
    .read = class_input_read,
    .is_text = false,
};

static const mp_obj_type_t class_input = {
    { &mp_type_type },
    .name = MP_QSTR_Input,
    .print = class_input_print,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &class_input_stream,
    .locals_dict = (mp_obj_t)&class_input_locals_dict,
};

static const mp_rom_map_elem_t class_output_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&sys_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(class_output_locals_dict, class_output_locals_dict_table);

static const mp_stream_p_t class_output_stream = {
    .write = class_output_write,
    .is_text = false,
};

static const mp_obj_type_t class_output = {
    { &mp_type_type },
    .name = MP_QSTR_Output,
    .print = class_output_print,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &class_output_stream,
    .locals_dict = (mp_obj_t)&class_output_locals_dict,
};

const struct class_input_t mp_sys_stdin_obj = { { &class_input } };
const struct class_output_t mp_sys_stdout_obj = { { &class_output } };
const struct class_output_t mp_sys_stderr_obj = { { &class_output } };
