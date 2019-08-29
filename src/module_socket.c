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

#if CONFIG_PUMBAA_MODULE_SOCKET == 1

#define AF_INET     0

#define SOCK_STREAM 0
#define SOCK_DGRAM  1
#define SOCK_RAW    2

extern const mp_obj_type_t module_socket_class_socket;

static mp_obj_t socket_make_new(const mp_obj_type_t *type_p,
                                size_t n_args,
                                size_t n_kw,
                                const mp_obj_t *args_p)
{
    struct class_socket_t *socket_p;
    mp_map_t kwargs;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_family, MP_ARG_INT, { .u_int = AF_INET } },
        { MP_QSTR_type, MP_ARG_INT, { .u_int = SOCK_STREAM } },
        { MP_QSTR_proto, MP_ARG_INT, { .u_int = 0 } }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int family;
    int type;
    int res;

    mp_arg_check_num(n_args, n_kw, 0, 3, true);

    /* Parse args. */
    mp_map_init_fixed_table(&kwargs, n_kw, args_p + n_args);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    family = args[0].u_int;
    type = args[1].u_int;

    if (family != AF_INET) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "Address family not supported by protocol"));
    }

    socket_p = m_new_obj(struct class_socket_t);
    socket_p->base.type = &module_socket_class_socket;

    switch (type) {

    case SOCK_STREAM:
        res = socket_open_tcp(&socket_p->socket);
        break;

    case SOCK_DGRAM:
        res = socket_open_udp(&socket_p->socket);
        break;

    default:
        res = -1;
        break;
    }

    if (res != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "failed to open socket"));
    }

    return (socket_p);
}

static mp_obj_t class_socket_accept(mp_obj_t self_in)
{
    struct class_socket_t *self_p;
    struct class_socket_t *socket_p;
    mp_obj_tuple_t *tuple_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    socket_p = m_new_obj(struct class_socket_t);
    socket_p->base.type = &module_socket_class_socket;

    if (socket_accept(&self_p->socket, &socket_p->socket, NULL) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket accept failed"));
    }

    tuple_p = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    tuple_p->items[0] = socket_p;
    tuple_p->items[1] = mp_const_none;

    return (MP_OBJ_FROM_PTR(tuple_p));
}

static mp_obj_t class_socket_bind(mp_obj_t self_in, mp_obj_t addr_in)
{
    struct class_socket_t *self_p;
    mp_obj_t *addr_items;
    struct inet_addr_t addr;

    self_p = MP_OBJ_TO_PTR(self_in);

    /* Convert the address. */
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    inet_aton(mp_obj_str_get_str(addr_items[0]), &addr.ip);
    addr.port = mp_obj_get_int(addr_items[1]);

    if (socket_bind(&self_p->socket, &addr) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket bind failed"));
    }

    return (mp_const_none);
}

static mp_obj_t class_socket_close(mp_obj_t self_in)
{
    struct class_socket_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    if (socket_close(&self_p->socket) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket close failed"));
    }

    return (mp_const_none);
}

static mp_obj_t class_socket_connect(mp_obj_t self_in,
                                     mp_obj_t address_in)
{
    struct class_socket_t *self_p;
    mp_obj_t *address_items;
    struct inet_addr_t address;

    self_p = MP_OBJ_TO_PTR(self_in);

    /* Convert the address. */
    mp_obj_get_array_fixed_n(address_in, 2, &address_items);
    inet_aton(mp_obj_str_get_str(address_items[0]), &address.ip);
    address.port = mp_obj_get_int(address_items[1]);

    if (socket_connect(&self_p->socket, &address) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket connect failed"));
    }

    return (mp_const_none);
}

static mp_obj_t class_socket_listen(mp_obj_t self_in,
                                    mp_obj_t backlog_in)
{
    struct class_socket_t *self_p;
    int backlog;

    self_p = MP_OBJ_TO_PTR(self_in);
    backlog = mp_obj_get_int(backlog_in);

    if (socket_listen(&self_p->socket, backlog) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket listen failed"));
    }

    return (mp_const_none);
}

static mp_obj_t class_socket_recv(mp_obj_t self_in,
                                  mp_obj_t bufsize_in)
{
    struct class_socket_t *self_p;
    vstr_t vstr;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(self_in);
    size = mp_obj_get_int(bufsize_in);

    vstr_init(&vstr, size);
    size = socket_read(&self_p->socket, vstr.buf, size);

    if (size < 0) {
        size = 0;
    }

    vstr.len = size;

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

static mp_obj_t class_socket_recvfrom(mp_obj_t self_in,
                                      mp_obj_t bufsize_in)
{
    struct class_socket_t *self_p;
    vstr_t vstr;
    ssize_t size;
    vstr_t vstr_remote_address;
    struct inet_addr_t remote_address;
    mp_obj_tuple_t *tuple_p;
    mp_obj_tuple_t *address_tuple_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    size = mp_obj_get_int(bufsize_in);

    /* Read the data from the socket. */
    vstr_init(&vstr, size);

    size = socket_recvfrom(&self_p->socket,
                           vstr.buf,
                           size,
                           0,
                           &remote_address);

    if (size < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket recvfrom failed"));
    }

    vstr.len = size;

    /* Convert the remote address IP to a vstr. */
    vstr_init(&vstr_remote_address, 20);

    if (inet_ntoa(&remote_address.ip, vstr_remote_address.buf) == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
                                           "socket recvfrom failed"));
    }

    vstr_remote_address.len = strlen(vstr_remote_address.buf);

    /* Address tuple. */
    address_tuple_p = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    address_tuple_p->items[0] = mp_obj_new_str_from_vstr(&mp_type_bytes,
                                                         &vstr_remote_address);
    address_tuple_p->items[1] = mp_obj_new_int(remote_address.port);

    /* Return tuple. */
    tuple_p = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    tuple_p->items[0] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    tuple_p->items[1] = MP_OBJ_FROM_PTR(address_tuple_p);

    return (MP_OBJ_FROM_PTR(tuple_p));
}

static mp_obj_t class_socket_recv_into(mp_obj_t self_in)
{
    mp_not_implemented("socket.recv_into");

    return (mp_const_none);
}

static mp_obj_t class_socket_recvfrom_into(mp_obj_t self_in)
{
    mp_not_implemented("socket.recvfrom_into");

    return (mp_const_none);
}

static mp_obj_t class_socket_send(mp_obj_t self_in, mp_obj_t string_in)
{
    struct class_socket_t *self_p;
    mp_buffer_info_t buffer_info;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(string_in),
                        &buffer_info,
                        MP_BUFFER_READ);

    size = socket_write(&self_p->socket, buffer_info.buf, buffer_info.len);

    if (size < 0) {
        size = 0;
    }

    return (mp_obj_new_int(size));
}

static mp_obj_t class_socket_sendall(mp_obj_t self_in,
                                     mp_obj_t string_in)
{
    return (class_socket_send(self_in, string_in));
}

static mp_obj_t class_socket_sendto(mp_obj_t self_in,
                                    mp_obj_t string_in,
                                    mp_obj_t address_in)
{
    struct class_socket_t *self_p;
    mp_buffer_info_t buffer_info;
    mp_obj_t *address_items;
    struct inet_addr_t address;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(self_in);

    /* Get the string. */
    mp_get_buffer_raise(MP_OBJ_TO_PTR(string_in),
                        &buffer_info,
                        MP_BUFFER_READ);

    /* Convert the address. */
    mp_obj_get_array_fixed_n(address_in, 2, &address_items);
    inet_aton(mp_obj_str_get_str(address_items[0]), &address.ip);
    address.port = mp_obj_get_int(address_items[1]);

    size = socket_sendto(&self_p->socket,
                         buffer_info.buf,
                         buffer_info.len,
                         0,
                         &address);

    if (size < 0) {
        size = 0;
    }

    return (mp_obj_new_int(size));
}

static mp_obj_t class_socket_shutdown(mp_obj_t self_in)
{
    mp_not_implemented("socket.shutdown");

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, class_socket_accept);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, class_socket_bind);
static MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, class_socket_close);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, class_socket_connect);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, class_socket_listen);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, class_socket_recv);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, class_socket_recvfrom);
static MP_DEFINE_CONST_FUN_OBJ_1(socket_recv_into_obj, class_socket_recv_into);
static MP_DEFINE_CONST_FUN_OBJ_1(socket_recvfrom_into_obj, class_socket_recvfrom_into);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, class_socket_send);
static MP_DEFINE_CONST_FUN_OBJ_2(socket_sendall_obj, class_socket_sendall);
static MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, class_socket_sendto);
static MP_DEFINE_CONST_FUN_OBJ_1(socket_shutdown_obj, class_socket_shutdown);

static const mp_rom_map_elem_t class_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv_into), MP_ROM_PTR(&socket_recv_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom_into), MP_ROM_PTR(&socket_recvfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall), MP_ROM_PTR(&socket_sendall_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_shutdown), MP_ROM_PTR(&socket_shutdown_obj) }
};

static MP_DEFINE_CONST_DICT(class_socket_locals_dict, class_socket_locals_dict_table);

/**
 * The socket class.
 */
const mp_obj_type_t module_socket_class_socket = {
    { &mp_type_type },
    .name = MP_QSTR_SocketType,
    .make_new = socket_make_new,
    .locals_dict = (void*)&class_socket_locals_dict,
};

/**
 * Function called when this module is imported.
 */
static mp_obj_t module_init(void)
{
    socket_module_init();

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_init_obj, module_init);

/**
 * The module globals table.
 */
static const mp_rom_map_elem_t mp_module_socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usocket) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&module_init_obj) },

    /* Types. */
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&module_socket_class_socket) },

    /* Constants. */
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(AF_INET) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCK_RAW) }
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals, mp_module_socket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_socket_globals,
};

#endif
