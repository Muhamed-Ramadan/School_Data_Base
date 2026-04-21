#ifndef VALIDATION_H
#define VALIDATION_H

/*
 * validation.h — Input Validation Interface
 * ─────────────────────────────────────────────────────────────────
 * Internal header. Include in any .c file that needs to validate
 * user input before accepting it.
 */

#include <stdbool.h>
#include "school.h"

bool is_valid_name    (const char *name);
bool is_valid_id      (const char *id);
bool is_valid_date    (const char *date);
bool is_valid_address (const char *address);
bool is_valid_phone   (const char *phone);

#endif /* VALIDATION_H */
