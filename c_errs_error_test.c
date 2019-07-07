#include "c_errs_error.h"

int
main(void) {
    error_t err;
    EXIT_CODE_T r = init_internal_error(&err, (int8_t)ERROR_TYPE_INTERNAL, "internal error");
    if (r != EXIT_CODE_OK) {
        exit(r);
    }

    drop_error(&err);

    r = init_bad_format_error(&err, (int8_t)ERROR_TYPE_BAD_FORMAT, "bad format error");
    if (r != EXIT_CODE_OK) {
        exit(r);
    }

    drop_error(&err);

    return 0;
}
