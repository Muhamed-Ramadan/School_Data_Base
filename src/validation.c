#include "validation.h"

/* ═══════════════════════════════════════════════════════════════════
   CALENDAR HELPERS  (used only by is_valid_date)
═══════════════════════════════════════════════════════════════════ */

/*
 * year : u16 — calendar years are positive and fit within 0-65535.
 *              i32 would be oversized; no negative years are valid here.
 */
static bool is_leap_year(u16 year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/*
 * month : u8  — values 1-12, fits in u8 (0-255).
 * year  : u16 — same reasoning as is_leap_year.
 * return: u8  — max days per month is 31, fits in u8.
 */
static u8 days_in_month(u8 month, u16 year)
{
    u8 days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (month == 2 && is_leap_year(year)) ? 29 : days[month];
}

/* ═══════════════════════════════════════════════════════════════════
   VALIDATORS
═══════════════════════════════════════════════════════════════════ */

/*
 * NAME RULES
 * ─────────────────────────────────────────────────────────────────
 * Allowed characters : A-Z  a-z  space  hyphen (-)
 * Must start and end with a letter (not space, not hyphen).
 * Must contain at least one space -> minimum two words.
 * Each word or hyphen-separated part must be >= 2 characters.
 * A hyphen must be preceded and followed by a letter.
 * No consecutive spaces (double space is rejected).
 * No character (case-insensitive) repeated more than twice in a row:
 *   "Moohamed" is rejected; "Mohammed" is accepted.
 * Overall length after trimming: 5 to NAME_LEN-1.
 *
 * len      : u8 — max NAME_LEN-1 = 29, fits in u8 (0-255).
 * part_len : u8 — counts chars in current word/part, max 29.
 * i        : u8 — loop index up to len (max 29), fits in u8.
 * c        : u8 — single character from the string.
 */
bool is_valid_name(const char *name)
{
    u8   len       = (u8)strlen(name);
    bool has_space = false;
    u8   part_len  = 0;
    u8   i;

    if (len < 5 || len >= NAME_LEN)                         return false;
    if (!isalpha((u8)name[0]))                              return false;
    if (!isalpha((u8)name[len - 1]))                        return false;

    for (i = 0; i < len; i++) {
        u8 c = (u8)name[i];

        if (!isalpha(c) && c != ' ' && c != '-')            return false;
        if (c == ' '  && (u8)name[i + 1] == ' ')            return false;
        if (c == '-') {
            if (i == 0 || !isalpha((u8)name[i - 1]))        return false;
            if (i + 1 >= len || !isalpha((u8)name[i + 1]))  return false;
        }
        if (c == ' ') has_space = true;

        /* no character repeated more than twice consecutively */
        if (i >= 2 &&
            tolower(c)               == tolower((u8)name[i - 1]) &&
            tolower((u8)name[i - 1]) == tolower((u8)name[i - 2])) return false;
    }

    if (!has_space) return false;

    /* every word / hyphen-part must be >= 2 characters */
    for (i = 0; i <= len; i++) {
        if (name[i] == ' ' || name[i] == '-' || name[i] == '\0') {
            if (part_len > 0 && part_len < 2) return false;
            part_len = 0;
        } else {
            part_len++;
        }
    }
    return true;
}

/*
 * ID RULES
 * Allowed  : A-Z  a-z  0-9  (no spaces or symbols)
 * Length   : 4 to ID_LEN-1
 *
 * len : u8 — max ID_LEN-1 = 9, fits in u8.
 * i   : u8 — loop index up to len (max 9), fits in u8.
 */
bool is_valid_id(const char *id)
{
    u8 len = (u8)strlen(id);
    u8 i;

    if (len < 4 || len >= ID_LEN) return false;
    for (i = 0; id[i]; i++)
        if (!isalnum((u8)id[i])) return false;
    return true;
}

/*
 * DATE OF BIRTH RULES
 * Format   : DD/MM/YYYY
 * Calendar : leap-year-aware; correct days per month enforced.
 * Age      : student must be 14 to 21 years old as of today.
 *
 * day, month   : u8  — 1-31 and 1-12, both fit in u8.
 * year         : u16 — 4-digit year (e.g. 2006), fits in u16; no negatives.
 * cur_year     : u16 — current year, same reasoning.
 * cur_month    : u8  — current month 1-12, fits in u8.
 * cur_day      : u8  — current day 1-31, fits in u8.
 * age          : u8  — expected range 14-21, fits in u8.
 */
bool is_valid_date(const char *date)
{
    u8  day, month;
    u16 year;
    u16 cur_year;
    u8  cur_month, cur_day;
    u8  age;

    if ((u8)strlen(date) != 10) return false;

    for (u8 i = 0; i < 10; i++) {
        if (i == 2 || i == 5) { if (date[i] != '/')          return false; }
        else                  { if (!isdigit((u8)date[i]))    return false; }
    }

    day   = (u8)((date[0] - '0') * 10 + (date[1] - '0'));
    month = (u8)((date[3] - '0') * 10 + (date[4] - '0'));
    year  = (u16)((date[6] - '0') * 1000 + (date[7] - '0') * 100 +
                  (date[8] - '0') * 10   + (date[9] - '0'));

    if (month < 1 || month > 12)                          return false;
    if (day < 1 || day > days_in_month(month, year))      return false;

    {
        time_t     now_t  = time(NULL);
        struct tm *now    = localtime(&now_t);
        cur_year  = (u16)(now->tm_year + 1900);
        cur_month = (u8)(now->tm_mon  + 1);
        cur_day   = (u8)now->tm_mday;
    }

    age = (u8)(cur_year - year);
    if (cur_month < month || (cur_month == month && cur_day < day)) age--;

    return (age >= 14 && age <= 21);
}

/*
 * ADDRESS RULES
 * ─────────────────────────────────────────────────────────────────
 * Allowed characters : A-Z  a-z  0-9  space  .  ,  -  /
 * Must not start or end with a space (trimmed before this is called).
 * No two consecutive identical non-alphanumeric characters:
 *   "//"  ".."  ",,"  "--"  and double space are all rejected.
 * Length (after trimming): 10 to ADDRESS_LEN-1.
 *
 * len : u8 — max ADDRESS_LEN-1 = 39, fits in u8.
 * i   : u8 — loop index up to len (max 39), fits in u8.
 * c   : u8 — single character.
 */
bool is_valid_address(const char *address)
{
    u8 len = (u8)strlen(address);
    u8 i;

    if (len < 10 || len >= ADDRESS_LEN)             return false;
    if (address[0] == ' ' || address[len-1] == ' ') return false;

    for (i = 0; i < len; i++) {
        u8 c = (u8)address[i];
        if (!isalnum(c) && c != ' ' && c != ',' &&
            c != '.'    && c != '-' && c != '/')    return false;
        /* reject consecutive identical non-alphanumeric characters */
        if (i > 0 && !isalnum(c) && c == (u8)address[i - 1]) return false;
    }
    return true;
}

/*
 * PHONE RULES
 * Allowed  : 0-9 only  (use 00 prefix for international)
 * Length   : 6 to 15 digits
 *
 * len : u8 — max 15, fits in u8.
 * i   : u8 — loop index up to len (max 15), fits in u8.
 */
bool is_valid_phone(const char *phone)
{
    u8 len = (u8)strlen(phone);
    u8 i;

    if (len < 6 || len > 15) return false;
    for (i = 0; phone[i]; i++)
        if (!isdigit((u8)phone[i])) return false;
    return true;
}
