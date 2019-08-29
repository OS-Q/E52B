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
 * Print the queue object.
 */
static void class_queue_print(const mp_print_t *print_p,
                              mp_obj_t self_in,
                              mp_print_kind_t kind)
{
    struct class_queue_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Queue object associated with the id. If additional
 * arguments are given, they are used to initialise the queue. See
 * `init`.
 *
 * class Queue()
 */
static mp_obj_t class_queue_make_new(const mp_obj_type_t *type_p,
                                     mp_uint_t n_args,
                                     mp_uint_t n_kw,
                                     const mp_obj_t *args_p)
{
    struct class_queue_t *self_p;

    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    /* Create a new Queue object. */
    self_p = m_new_obj(struct class_queue_t);
    self_p->base.type = &module_sync_class_queue;

    /* Initialize the queue if queue and mode are given. */
    queue_init(&self_p->queue, &self_p->buf[0], sizeof(self_p->buf));

    return (self_p);
}

/**
 * def read(self, mask)
 */
static mp_obj_t class_queue_read(mp_obj_t self_in, mp_obj_t size_in)
{
    struct class_queue_t *self_p;
    vstr_t vstr;
    size_t size;

    self_p = MP_OBJ_TO_PTR(self_in);
    size = mp_obj_get_int(size_in);
    vstr_init_len(&vstr, size);

    size = queue_read(&self_p->queue, vstr.buf, size);

    if (size <= 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "failed to read from queue"));
    }

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

/**
 * def write(self, mask)
 */
static mp_obj_t class_queue_write(mp_obj_t self_in, mp_obj_t buf_in)
{
    struct class_queue_t *self_p;
    mp_buffer_info_t bufinfo;
    ssize_t res;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    res = queue_write(&self_p->queue, bufinfo.buf, bufinfo.len);

    if (res <= 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "failed to write to queue"));
    }

    return (MP_OBJ_NEW_SMALL_INT(res));
}

/**
 * def size(self)
 */
static mp_obj_t class_queue_size(mp_obj_t self_in)
{
    struct class_queue_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    return (MP_OBJ_NEW_SMALL_INT(queue_size(&self_p->queue)));
}

static MP_DEFINE_CONST_FUN_OBJ_2(class_queue_read_obj, class_queue_read);
static MP_DEFINE_CONST_FUN_OBJ_2(class_queue_write_obj, class_queue_write);
static MP_DEFINE_CONST_FUN_OBJ_1(class_queue_size_obj, class_queue_size);

static const mp_rom_map_elem_t class_queue_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&class_queue_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&class_queue_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&class_queue_size_obj) },
};

static MP_DEFINE_CONST_DICT(class_queue_locals_dict, class_queue_locals_dict_table);

/**
 * Queue class type.
 */
const mp_obj_type_t module_sync_class_queue = {
    { &mp_type_type },
    .name = MP_QSTR_Queue,
    .print = class_queue_print,
    .make_new = class_queue_make_new,
    .locals_dict = (mp_obj_t)&class_queue_locals_dict,
};
