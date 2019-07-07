#ifndef C_ERRS_ERROR_H
#define C_ERRS_ERROR_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_CODE_T int

#define EXIT_CODE_OK    0
#define EXIT_CODE_ERROR 1

enum error_type {
    ERROR_TYPE_INTERNAL          = 0,
    ERROR_TYPE_BAD_FORMAT        = 1,
    ERROR_TYPE_INVALID_FORMAT    = 2,
    ERROR_TYPE_CONFLICT          = 3,
    ERROR_TYPE_NOT_FOUND         = 4,
    ERROR_TYPE_NOT_AUTHORIZED    = 5,
    ERROR_TYPE_PERMISSION_DENIED = 6,
    ERROR_TYPE_NOT_IMPLEMENTED   = 7,
};

typedef enum error_type error_type_t;

#define STRING_T     \
    struct {         \
        char *data;  \
        size_t len; \
    }

struct error {
    int8_t type;

    int8_t code;
    char *message;

    STRING_T json;
    STRING_T text;
};

typedef struct error error_t;

EXIT_CODE_T
init_error(error_t *err, error_type_t type, int8_t code, const char *message);

#define __ERROR_INITIALIZER(name, err_type)                \
    EXIT_CODE_T                                            \
    name(error_t *err, int8_t code, const char *message) { \
        return init_error(err, err_type, code, message);   \
    }

__ERROR_INITIALIZER(init_internal_error, ERROR_TYPE_INTERNAL)
__ERROR_INITIALIZER(init_bad_format_error, ERROR_TYPE_BAD_FORMAT)
__ERROR_INITIALIZER(init_invalid_format_error, ERROR_TYPE_INVALID_FORMAT)
__ERROR_INITIALIZER(init_conflict_error, ERROR_TYPE_CONFLICT)
__ERROR_INITIALIZER(init_not_found_error, ERROR_TYPE_NOT_FOUND)
__ERROR_INITIALIZER(init_not_authorized_error, ERROR_TYPE_NOT_AUTHORIZED)
__ERROR_INITIALIZER(init_permission_denied_error, ERROR_TYPE_PERMISSION_DENIED)
__ERROR_INITIALIZER(init_not_implemented_error, ERROR_TYPE_NOT_IMPLEMENTED)

EXIT_CODE_T
drop_error(error_t *err);

static
EXIT_CODE_T
__init_error_message(error_t *err, const char *message) {
    size_t message_len = (strlen(message) + 1);

    err->message = (char*)malloc((message_len + 1) * sizeof(char));
    if (!err->message) {
        return EXIT_CODE_ERROR;
    }

    memmove(err->message, message, message_len);
    err->message[message_len] = '\0';

    return EXIT_CODE_OK;
}

#define __INIT_EMPTY_STRING(name) \
    name.data = NULL;             \
    name.len  = 0;

#define __MAX_INT8_STR_LEN 4

#define __INIT_STRING(__name, __buffer_len, __fmt, ...)      \
    __name.data = (char*)malloc((__buffer_len + 1) * sizeof(char)); \
    if (!__name.data) {                                              \
        return EXIT_CODE_ERROR;                                      \
    }                                                                \
                                                                     \
    __name.len = sprintf(__name.data, __fmt, __VA_ARGS__);           \
    __name.data[__name.len] = '\0';                                  \
                                                                     \
    return EXIT_CODE_OK;

#define __FMT_MODIFIER_LEN 2

static
EXIT_CODE_T
__init_error_json(error_t *err) {
    const char *fmt = "{\"code\":%d,\"message\":\"%s\"}";
    const size_t schema_len = strlen(fmt) - (2 * __FMT_MODIFIER_LEN);

    const size_t max_code_str_len = __MAX_INT8_STR_LEN;
    size_t message_len = strlen(err->message);

    size_t buffer_len =
            schema_len +
            max_code_str_len + message_len;

    __INIT_STRING(err->json, buffer_len, fmt,
                  err->code, err->message)
}

static
EXIT_CODE_T
__init_error_text(error_t *err) {
    const char *fmt = "error: [type = %d; code = %d; message = '%s']";
    const size_t schema_len = strlen(fmt) - (3 * __FMT_MODIFIER_LEN);

    const size_t max_type_str_len = __MAX_INT8_STR_LEN;
    const size_t max_code_str_len = __MAX_INT8_STR_LEN;
    size_t message_len = strlen(err->message);

    size_t buffer_len =
            schema_len +
            max_type_str_len + max_code_str_len +
            message_len;

    __INIT_STRING(err->text, buffer_len, fmt,
                  err->type, err->code, err->message)
}

EXIT_CODE_T
init_error(error_t *err, error_type_t type, int8_t code, const char *message) {
    err->type = type;
    err->code = code;

    __init_error_message(err, message);

    if (type == ERROR_TYPE_INTERNAL) {
        __init_error_text(err);
        __INIT_EMPTY_STRING(err->json)
    } else {
        __init_error_json(err);
        __INIT_EMPTY_STRING(err->text)
    }

    return EXIT_CODE_OK;
}

#define __FREE_ERROR_FIELD(err_ptr, field) \
    if (err_ptr->field) {                  \
        free(err_ptr->field);              \
    }

EXIT_CODE_T
drop_error(error_t *err) {
    __FREE_ERROR_FIELD(err, message)
    __FREE_ERROR_FIELD(err, json.data)
    __FREE_ERROR_FIELD(err, text.data)

    return EXIT_CODE_OK;
}

#endif //C_ERRS_ERROR_H
