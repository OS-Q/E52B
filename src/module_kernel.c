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

#if CONFIG_PUMBAA_SYS_LOCK == 1

/**
 * def sys_lock()
 */
static mp_obj_t module_sys_lock()
{
    sys_lock();

    return (mp_const_none);
}

/**
 * def sys_unlock()
 */
static mp_obj_t module_sys_unlock()
{
    sys_unlock();

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_sys_lock_obj, module_sys_lock);
static MP_DEFINE_CONST_FUN_OBJ_0(module_sys_unlock_obj, module_sys_unlock);

#endif

#if CONFIG_PUMBAA_SYS_REBOOT == 1

/**
 * def sys_reboot()
 */
static mp_obj_t module_sys_reboot()
{
    sys_reboot();

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_sys_reboot_obj, module_sys_reboot);

#endif

#if CONFIG_PUMBAA_THRD == 1

static mp_obj_t module_thrd_yield(void)
{
    thrd_yield();

    return (mp_const_none);
}

static mp_obj_t module_thrd_join(mp_obj_t thrd_in)
{
    mp_not_implemented("thrd_join");

    return (mp_const_none);
}

static mp_obj_t module_thrd_self(void)
{
    return (mp_obj_new_int((uintptr_t)thrd_self()));
}

static mp_obj_t module_thrd_set_name(mp_obj_t name_in)
{
    mp_not_implemented("thrd_set_name");

    return (mp_const_none);
}

static mp_obj_t module_thrd_get_name(void)
{
    const char *name_p;

    name_p = thrd_get_name();

    if (name_p == NULL) {
        return (mp_const_none);
    }

    return (mp_obj_new_str(name_p, strlen(name_p), 0));
}

static mp_obj_t module_thrd_get_by_name(mp_obj_t name_in)
{
    void *thrd_p;

    thrd_p = thrd_get_by_name(mp_obj_str_get_str(name_in));

    if (thrd_p == NULL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                                                "No such thread: '%s'",
                                                mp_obj_str_get_str(name_in)));
    }

    return (mp_obj_new_int((uintptr_t)thrd_p));
}

static mp_obj_t module_thrd_set_log_mask(mp_obj_t thrd_in, mp_obj_t mask_in)
{
    thrd_set_log_mask((struct thrd_t *)(uintptr_t)mp_obj_get_int(thrd_in),
                      mp_obj_get_int(mask_in));

    return (mp_const_none);
}

static mp_obj_t module_thrd_get_log_mask(void)
{
    return (MP_OBJ_NEW_SMALL_INT(thrd_get_log_mask()));
}

static mp_obj_t module_thrd_set_prio(mp_obj_t thrd_in, mp_obj_t prio_in)
{
    thrd_set_prio((struct thrd_t *)(uintptr_t)mp_obj_get_int(thrd_in),
                  mp_obj_get_int(prio_in));

    return (mp_const_none);
}

static mp_obj_t module_thrd_get_prio()
{
    return (MP_OBJ_NEW_SMALL_INT(thrd_get_prio()));
}

static mp_obj_t module_thrd_set_global_env(mp_obj_t name_in, mp_obj_t value_in)
{
    mp_not_implemented("thrd_set_global_env");

    return (mp_const_none);
}

static mp_obj_t module_thrd_get_global_env(mp_obj_t name_in)
{
    const char *value_p;

    value_p = thrd_get_global_env(mp_obj_str_get_str(name_in));

    if (value_p == NULL) {
        return (mp_const_none);
    }

    return (mp_obj_new_str(value_p, strlen(value_p), 0));
}

static mp_obj_t module_thrd_set_env(mp_obj_t name_in, mp_obj_t value_in)
{
    mp_not_implemented("thrd_set_env");

    return (mp_const_none);
}

static mp_obj_t module_thrd_get_env(mp_obj_t name_in)
{
    const char *value_p;

    value_p = thrd_get_env(mp_obj_str_get_str(name_in));

    if (value_p == NULL) {
        return (mp_const_none);
    }

    return (mp_obj_new_str(value_p, strlen(value_p), 0));
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_thrd_yield_obj, module_thrd_yield);
static MP_DEFINE_CONST_FUN_OBJ_1(module_thrd_join_obj, module_thrd_join);
static MP_DEFINE_CONST_FUN_OBJ_0(module_thrd_self_obj, module_thrd_self);
static MP_DEFINE_CONST_FUN_OBJ_1(module_thrd_set_name_obj, module_thrd_set_name);
static MP_DEFINE_CONST_FUN_OBJ_0(module_thrd_get_name_obj, module_thrd_get_name);
static MP_DEFINE_CONST_FUN_OBJ_1(module_thrd_get_by_name_obj, module_thrd_get_by_name);
static MP_DEFINE_CONST_FUN_OBJ_2(module_thrd_set_log_mask_obj, module_thrd_set_log_mask);
static MP_DEFINE_CONST_FUN_OBJ_0(module_thrd_get_log_mask_obj, module_thrd_get_log_mask);
static MP_DEFINE_CONST_FUN_OBJ_2(module_thrd_set_prio_obj, module_thrd_set_prio);
static MP_DEFINE_CONST_FUN_OBJ_0(module_thrd_get_prio_obj, module_thrd_get_prio);
static MP_DEFINE_CONST_FUN_OBJ_2(module_thrd_set_global_env_obj, module_thrd_set_global_env);
static MP_DEFINE_CONST_FUN_OBJ_1(module_thrd_get_global_env_obj, module_thrd_get_global_env);
static MP_DEFINE_CONST_FUN_OBJ_2(module_thrd_set_env_obj, module_thrd_set_env);
static MP_DEFINE_CONST_FUN_OBJ_1(module_thrd_get_env_obj, module_thrd_get_env);

#endif

/**
 * Function called when this module is imported.
 */
static mp_obj_t module_init(void)
{
#if CONFIG_PUMBAA_CLASS_TIMER == 1
    timer_module_init();
#endif

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_init_obj, module_init);

/**
 * A table of all the modules' global objects.
 */
static const mp_rom_map_elem_t module_kernel_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kernel) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&module_init_obj) },

    /* Module classes. */
#if CONFIG_PUMBAA_CLASS_TIMER == 1
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&module_kernel_class_timer) },
#endif

    /* Module functions. */
#if CONFIG_PUMBAA_SYS_LOCK == 1
    { MP_ROM_QSTR(MP_QSTR_sys_lock), MP_ROM_PTR(&module_sys_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_sys_unlock), MP_ROM_PTR(&module_sys_unlock_obj) },
#endif

#if CONFIG_PUMBAA_SYS_REBOOT == 1
    { MP_ROM_QSTR(MP_QSTR_sys_reboot), MP_ROM_PTR(&module_sys_reboot_obj) },
#endif

#if CONFIG_PUMBAA_THRD == 1
    { MP_ROM_QSTR(MP_QSTR_thrd_yield), MP_ROM_PTR(&module_thrd_yield_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_join), MP_ROM_PTR(&module_thrd_join_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_self), MP_ROM_PTR(&module_thrd_self_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_set_name), MP_ROM_PTR(&module_thrd_set_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_name), MP_ROM_PTR(&module_thrd_get_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_by_name), MP_ROM_PTR(&module_thrd_get_by_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_set_log_mask), MP_ROM_PTR(&module_thrd_set_log_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_log_mask), MP_ROM_PTR(&module_thrd_get_log_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_set_prio), MP_ROM_PTR(&module_thrd_set_prio_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_prio), MP_ROM_PTR(&module_thrd_get_prio_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_set_global_env), MP_ROM_PTR(&module_thrd_set_global_env_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_global_env), MP_ROM_PTR(&module_thrd_get_global_env_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_set_env), MP_ROM_PTR(&module_thrd_set_env_obj) },
    { MP_ROM_QSTR(MP_QSTR_thrd_get_env), MP_ROM_PTR(&module_thrd_get_env_obj) },
#endif
};

static MP_DEFINE_CONST_DICT(module_kernel_globals, module_kernel_globals_table);

const mp_obj_module_t module_kernel = {
    { &mp_type_module },
    .globals = (mp_obj_t)&module_kernel_globals,
};
