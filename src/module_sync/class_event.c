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
 * Print the event object.
 */
static void class_event_print(const mp_print_t *print_p,
                              mp_obj_t self_in,
                              mp_print_kind_t kind)
{
    struct class_event_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Event object associated with the id. If additional
 * arguments are given, they are used to initialise the event. See
 * `init`.
 *
 * class Event()
 */
static mp_obj_t class_event_make_new(const mp_obj_type_t *type_p,
                                     mp_uint_t n_args,
                                     mp_uint_t n_kw,
                                     const mp_obj_t *args_p)
{
    struct class_event_t *self_p;

    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    /* Create a new Event object. */
    self_p = m_new_obj(struct class_event_t);
    self_p->base.type = &module_sync_class_event;

    /* Initialize the event if event and mode are given. */
    event_init(&self_p->event);

    return (self_p);
}

/**
 * def read(self, mask)
 */
static mp_obj_t class_event_read(mp_obj_t self_in, mp_obj_t mask_in)
{
    struct class_event_t *self_p;
    uint32_t mask;

    self_p = MP_OBJ_TO_PTR(self_in);
    mask = mp_obj_get_int(mask_in);

    if (event_read(&self_p->event, &mask, sizeof(mask)) != sizeof(mask)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "failed to read event mask"));
    }

    return (MP_OBJ_NEW_SMALL_INT(mask));
}

/**
 * def write(self, mask)
 */
static mp_obj_t class_event_write(mp_obj_t self_in, mp_obj_t mask_in)
{
    struct class_event_t *self_p;
    uint32_t mask;

    self_p = MP_OBJ_TO_PTR(self_in);
    mask = mp_obj_get_int(mask_in);

    if (event_write(&self_p->event, &mask, sizeof(mask)) != sizeof(mask)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "failed to write event mask"));
    }

    return (mp_const_none);
}

/**
 * def size(self)
 */
static mp_obj_t class_event_size(mp_obj_t self_in)
{
    struct class_event_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    return (MP_OBJ_NEW_SMALL_INT(event_size(&self_p->event)));
}

static MP_DEFINE_CONST_FUN_OBJ_2(class_event_read_obj, class_event_read);
static MP_DEFINE_CONST_FUN_OBJ_2(class_event_write_obj, class_event_write);
static MP_DEFINE_CONST_FUN_OBJ_1(class_event_size_obj, class_event_size);

static const mp_rom_map_elem_t class_event_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&class_event_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&class_event_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&class_event_size_obj) },
};

static MP_DEFINE_CONST_DICT(class_event_locals_dict, class_event_locals_dict_table);

/**
 * Event class type.
 */
const mp_obj_type_t module_sync_class_event = {
    { &mp_type_type },
    .name = MP_QSTR_Event,
    .print = class_event_print,
    .make_new = class_event_make_new,
    .locals_dict = (mp_obj_t)&class_event_locals_dict,
};
