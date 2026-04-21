#include "input.h"
#include "validation.h"

/* ═══════════════════════════════════════════════════════════════════
   STRING UTILITIES
═══════════════════════════════════════════════════════════════════ */

/*
 * Strip the trailing '\n' that fgets always stores.
 * len : u8 — all our strings are <= INPUT_BUF-1 = 63 chars, fits in u8.
 */
void trim_newline(char *str)
{
    u8 len = (u8)strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

/*
 * Remove leading and trailing space characters in-place.
 * end   : i8 — starts at len-1 (max 38) and can reach -1; needs signed.
 *              i8 range is -128 to 127, covers -1 to 38. ✓
 * start : u8 — only increments forward; always >= 0.
 */
void trim_spaces(char *str)
{
    i8 end   = (i8)strlen(str) - 1;   /* signed: may reach -1 on empty str  */
    u8 start = 0;

    while (end >= 0 && str[end] == ' ') str[end--] = '\0';
    while (str[start] == ' ') start++;
    if (start > 0) memmove(str, str + start, strlen(str + start) + 1);
}

/* Discard all remaining characters in stdin up to and including newline. */
void flush_stdin(void)
{
    i32 c;   /* i32: getchar() returns int to represent EOF (-1) as well    */
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ═══════════════════════════════════════════════════════════════════
   FIELD COLLECTORS
   Each function prints constraints, reads input, validates,
   and copies the accepted value into dest. Retries until valid.
═══════════════════════════════════════════════════════════════════ */

void read_name(char *dest)
{
    char buf[INPUT_BUF];

    printf("\n  Name\n");
    printf("    Allowed : A-Z   a-z   space   -\n");
    printf("    Rules   : Minimum two words  |  Each word min 2 chars\n");
    printf("              Hyphen ( - ) must be between two letters\n");
    printf("              No character repeated more than twice in a row\n");
    printf("    Length  : 5 to 29 characters\n");

    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        trim_spaces(buf);
        if (!is_valid_name(buf))
            printf("    Invalid. Please follow the rules above.\n");
        else {
            strncpy(dest, buf, NAME_LEN - 1);
            dest[NAME_LEN - 1] = '\0';
            break;
        }
    }
}

void read_id(char *dest)
{
    char buf[INPUT_BUF];

    printf("\n  ID\n");
    printf("    Allowed : A-Z   a-z   0-9   ( no spaces or symbols )\n");
    printf("    Length  : 4 to 9 characters\n");

    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!is_valid_id(buf))
            printf("    Invalid. Please follow the rules above.\n");
        else {
            strncpy(dest, buf, ID_LEN - 1);
            dest[ID_LEN - 1] = '\0';
            break;
        }
    }
}

void read_date(char *dest)
{
    char buf[INPUT_BUF];

    printf("\n  Date of Birth\n");
    printf("    Format  : DD/MM/YYYY   e.g. 14/03/2005\n");
    printf("    Note    : Student must be 14 to 21 years old\n");

    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!is_valid_date(buf))
            printf("    Invalid. Check the format, calendar date,"
                   " and age range (14-21).\n");
        else {
            strncpy(dest, buf, BIRTH_LEN - 1);
            dest[BIRTH_LEN - 1] = '\0';
            break;
        }
    }
}

void read_address(char *dest)
{
    char buf[INPUT_BUF];

    printf("\n  Address\n");
    printf("    Allowed : A-Z   a-z   0-9   space   .   ,   -   /\n");
    printf("    Rules   : No consecutive identical symbols   e.g. // or ..\n");
    printf("    Length  : 10 to 39 characters\n");

    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        trim_spaces(buf);
        if (!is_valid_address(buf))
            printf("    Invalid. Please follow the rules above.\n");
        else {
            strncpy(dest, buf, ADDRESS_LEN - 1);
            dest[ADDRESS_LEN - 1] = '\0';
            break;
        }
    }
}

void read_phone(char *dest)
{
    char buf[INPUT_BUF];

    printf("\n  Guardian Phone\n");
    printf("    Allowed : 0-9   ( digits only )\n");
    printf("    Note    : Use 00 prefix for international"
           "   e.g. 0020100123456\n");
    printf("    Length  : 6 to 15 digits\n");

    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!is_valid_phone(buf))
            printf("    Invalid. Digits only, 6 to 15 characters.\n");
        else {
            strncpy(dest, buf, PHONE_LEN - 1);
            dest[PHONE_LEN - 1] = '\0';
            break;
        }
    }
}

/*
 * Read and validate a single score (0-100); retries on bad input.
 *
 * raw : i32 — scanf requires int*; converted to u8 on success.
 *             i32 used to safely receive out-of-range values before checking.
 * return: u8 — validated score 0-100, fits in u8.
 */
u8 read_score(const char *subject)
{
    i32 raw;   /* temporary signed int for scanf; cast to u8 after validation */

    printf("    %-16s: ", subject);
    while (1) {
        if (scanf("%d", &raw) == 1 && raw >= 0 && raw <= 100) {
            flush_stdin();
            return (u8)raw;
        }
        flush_stdin();
        printf("      Invalid. Enter a whole number between 0 and 100.\n");
        printf("    %-16s: ", subject);
    }
}

/* ═══════════════════════════════════════════════════════════════════
   SEARCH COLLECTORS
   Validate format, copy to dest, return false on bad input.
═══════════════════════════════════════════════════════════════════ */

bool read_search_id(char *dest)
{
    char buf[INPUT_BUF];

    printf("    Allowed : A-Z   a-z   0-9   |  Length: 4 to 9 chars\n");
    printf("    > ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);

    if (!is_valid_id(buf)) {
        printf("  Invalid ID format.\n\n");
        return false;
    }
    strncpy(dest, buf, ID_LEN - 1);
    dest[ID_LEN - 1] = '\0';
    return true;
}

bool read_search_name(char *dest)
{
    char buf[INPUT_BUF];

    printf("    Allowed : A-Z   a-z   space   -   |  Min two words\n");
    printf("    > ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    trim_spaces(buf);

    if (!is_valid_name(buf)) {
        printf("  Invalid name format.\n\n");
        return false;
    }
    strncpy(dest, buf, NAME_LEN - 1);
    dest[NAME_LEN - 1] = '\0';
    return true;
}
