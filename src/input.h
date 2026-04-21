#ifndef INPUT_H
#define INPUT_H

/*
 * input.h — User Input Collection Interface
 * ─────────────────────────────────────────────────────────────────
 * Internal header. Each function prompts the user, validates the
 * input, and retries until a valid value is entered.
 */

#include <stdbool.h>
#include "school.h"

/* -- String utilities ----------------------------------------- */
void trim_newline     (char *str);
void trim_spaces      (char *str);
void flush_stdin      (void);

/* -- Field collectors (prompt + validate + copy to dest) ------ */
void read_name        (char *dest);
void read_id          (char *dest);
void read_date        (char *dest);
void read_address     (char *dest);
void read_phone       (char *dest);
u8   read_score       (const char *subject);

/* -- Search collectors (return false on bad format) ----------- */
bool read_search_id   (char *dest);
bool read_search_name (char *dest);

#endif /* INPUT_H */
