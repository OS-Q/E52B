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

#if CONFIG_PUMBAA_CLASS_DAC == 1

/**
 * Print the dac object.
 */
static void class_dac_print(const mp_print_t *print_p,
                            mp_obj_t self_in,
                            mp_print_kind_t kind)
{
    struct class_dac_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Dac object associated with the id. If additional
 * arguments are given, they are used to initialise the dac. See
 * `init`.
 */
static mp_obj_t class_dac_make_new(const mp_obj_type_t *type_p,
                                   mp_uint_t n_args,
                                   mp_uint_t n_kw,
                                   const mp_obj_t *args_p)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_devices, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sample_rate, MP_ARG_INT, { .u_int = 100 } }
    };
    struct class_dac_t *self_p;
    mp_map_t kwargs;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    struct mp_obj_t *devices_p;
    int sampling_rate;
    struct pin_device_t *pins[2];
    mp_uint_t len;
    mp_obj_t *items_p;
    int i;

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Parse args. */
    mp_map_init(&kwargs, 0);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* Create a new dac object. */
    self_p = m_new0(struct class_dac_t, 1);
    self_p->base.type = &module_drivers_class_dac;

    devices_p = MP_OBJ_TO_PTR(args[0].u_obj);

    for (i = 0; i < membersof(pins); i++) {
        pins[i] = NULL;
    }

    if (MP_OBJ_IS_TYPE(devices_p, &mp_type_list)) {
        mp_obj_list_get(devices_p, &len, &items_p);

        if (len > 2) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                               "too many devices"));
        }

        for (i = 0; i < len; i++) {
            pins[i] = &pin_device[mp_obj_get_int(items_p[i])];
        }
    } else if (MP_OBJ_IS_INT(devices_p)) {
        pins[0] = &pin_device[mp_obj_get_int(devices_p)];
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                                           "bad devices"));
    }

    sampling_rate = args[1].u_int;

    if (dac_init((struct dac_driver_t *)&self_p->drv,
                 &dac_device[0],
                 pins[0],
                 pins[1],
                 sampling_rate) != 0) {
        return (mp_const_none);
    }

    return (self_p);
}

/**
 * def convert(self, samples)
 */
static mp_obj_t class_dac_convert(mp_obj_t self_in, mp_obj_t samples_in)
{
    struct class_dac_t *self_p;
    mp_buffer_info_t buffer_info;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(samples_in),
                        &buffer_info,
                        MP_BUFFER_READ);

    if (dac_convert(&self_p->drv, buffer_info.buf, buffer_info.len) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "dac_convert() failed"));
    }

    return (mp_const_none);
}

/**
 * def async_convert(self, samples)
 */
static mp_obj_t class_dac_async_convert(mp_obj_t self_in, mp_obj_t samples_in)
{
    struct class_dac_t *self_p;
    mp_buffer_info_t buffer_info;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(samples_in),
                        &buffer_info,
                        MP_BUFFER_READ);

    if (dac_async_convert(&self_p->drv,
                          buffer_info.buf,
                          buffer_info.len) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "dac_async_convert() failed"));
    }

    return (mp_const_none);
}

/**
 * def async_wait(self)
 */
static mp_obj_t class_dac_async_wait(mp_obj_t self_in)
{
    struct class_dac_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    if (dac_async_wait(&self_p->drv) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "dac_async_wait() failed"));
    }

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_2(class_dac_convert_obj, class_dac_convert);
static MP_DEFINE_CONST_FUN_OBJ_2(class_dac_async_convert_obj, class_dac_async_convert);
static MP_DEFINE_CONST_FUN_OBJ_1(class_dac_async_wait_obj, class_dac_async_wait);

static const mp_rom_map_elem_t class_dac_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_convert), MP_ROM_PTR(&class_dac_convert_obj) },
    { MP_ROM_QSTR(MP_QSTR_async_convert), MP_ROM_PTR(&class_dac_async_convert_obj) },
    { MP_ROM_QSTR(MP_QSTR_async_wait), MP_ROM_PTR(&class_dac_async_wait_obj) },
};

static MP_DEFINE_CONST_DICT(class_dac_locals_dict, class_dac_locals_dict_table);

/**
 * Dac class type.
 */
const mp_obj_type_t module_drivers_class_dac = {
    { &mp_type_type },
    .name = MP_QSTR_Dac,
    .print = class_dac_print,
    .make_new = class_dac_make_new,
    .locals_dict = (mp_obj_t)&class_dac_locals_dict,
};

#endif
