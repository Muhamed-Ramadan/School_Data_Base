# School Management System

A console-based student database implemented in C, built as the capstone project for the C module of the NTI Embedded Systems diploma. The system manages student profiles and academic records through a fully validated, menu-driven interface backed by a dynamically allocated singly linked list queue.

> **[Releases](https://github.com/Muhamed-Ramadan/School_Data_Base/releases)** — Pre-built Windows executable available. No build required.

---

## Demo

### Main menu and enrollment counter
![Main menu](docs/demo/01_main_menu.png)

### Adding a new student profile with live validation
![Add student](docs/demo/02_add_student.gif)

### Profile lookup, edit, and remove in one session
![Lookup flow](docs/demo/03_lookup_flow.gif)

### Scores view with per-subject GPA and total
![Scores view](docs/demo/04_scores_view.png)

### A-Z list and ranked list — insertion order preserved
![Sort demo](docs/demo/05_sort_demo.gif)

---

## Architecture

### Data structures

```
School (struct queue)
┌─────────────────┐
│ u16   count     │  ← enrolled student count, updated on every add / remove
│ Student* front  │──► [ Student 1 ] ──► [ Student 2 ] ──► · · · ──► NULL
│ Student* rear   │─────────────────────────────────────► [ Student n ]
└─────────────────┘

Student (struct node)
┌──────────────────────┐
│ char  name[30]       │  ← full name
│ char  id[10]         │  ← unique identifier
│ char  birth[11]      │  ← DD/MM/YYYY
│ char  address[40]    │
│ char  phone[16]      │  ← guardian phone
│ u8    comp_science   │  ← scores: 0–100 stored as u8 (no sign, max 100)
│ u8    science        │
│ u8    english        │
│ u8    math           │
│ u8    history        │
│ f32   total          │  ← computed average of all five subjects
│ u8    *_grade        │  ← letter grade per subject and overall (A/B/C/D/F)
│ Student* next  ──────┼──► next node
└──────────────────────┘
```

Nodes are heap-allocated via `malloc` and linked through `next` pointers. The list is **never reordered** — sorted output is produced by building a temporary `Student **` pointer array, sorting it with bubble sort, printing, and freeing.

### Fixed-width types

All variables use the smallest type that fits their range, following embedded C conventions (`types.h`):

| Alias | Backing type | Used for |
|-------|-------------|---------|
| `u8`  | `uint8_t`   | scores (0–100), letter grades, loop indices ≤ 255 |
| `u16` | `uint16_t`  | enrollment count, calendar year |
| `i8`  | `int8_t`    | signed indices that may reach −1 |
| `i32` | `int32_t`   | `scanf` targets, `printf` format widths |
| `f32` | `float`     | total score average |

---

## Menu

```
+---------------------------------------------------------------+
|  MAIN MENU                              12 students enrolled  |
+---------------------------------------------------------------+
|  -- Profiles --                                               |
|   1. Add New Student Profile                                  |
|   2. Student Profile Lookup, Edit & Remove                    |
|   3. View All Student Profiles       (Insertion Order)        |
|   4. List All Student Profiles       (A-Z)                    |
|  -- Scores --                                                 |
|   5. Student Scores Lookup & Edit                             |
|   6. Edit All Students' Scores                                |
|   7. View All Students' Scores       (Insertion Order)        |
|   8. List All Students' Scores       (Ranked by Score)        |
|                                                               |
|   D. Exit                                                     |
+---------------------------------------------------------------+
```

The enrolled-student count updates automatically in the header after every add or remove. Options 3 and 7 display data in insertion order. Options 4 and 8 display sorted data — the underlying list is untouched.

---

## Features

### Option 1 — Add New Student Profile
Collects name, date of birth, ID, address, and guardian phone. Each field is validated before acceptance. Name and ID uniqueness are enforced across the entire database before the node is enqueued.

### Option 2 — Student Profile Lookup, Edit & Remove
Unified session: search by name or ID, view the full profile card, then edit all fields, remove the profile (with Y/N confirmation), or search again — without returning to the main menu between steps.

### Option 3 — View All Student Profiles (Insertion Order)
Displays all profiles in a formatted table in the order they were added.

### Option 4 — List All Student Profiles (A-Z)
Builds a temporary pointer array, sorts alphabetically by name, and displays the table. The linked list is unchanged.

### Option 5 — Student Scores Lookup & Edit
Search by name or ID, view the full score card with per-subject GPA and total, then optionally edit all subjects or one specific subject. Updated results are shown immediately.

### Option 6 — Edit All Students' Scores
Choose a mode once — all subjects or one specific subject — then step through every student sequentially. Per student: any key to edit, **K** to skip, **B** to stop and save progress.

### Option 7 — View All Students' Scores (Insertion Order)
Displays all scores in a formatted table in insertion order.

### Option 8 — List All Students' Scores (Ranked by Score)
Builds a temporary pointer array, sorts by total score descending, and displays the score table. The linked list is unchanged.

---

## GPA System

Scores follow the American grading scale (0–100). Letter grades are computed and stored in the node after every score change via `compute_grades()`.

| Score  | Grade |
|--------|-------|
| 90–100 | A     |
| 80–89  | B     |
| 70–79  | C     |
| 60–69  | D     |
| 0–59   | F     |

The total score is the arithmetic average of all five subjects. The overall letter grade is derived from the rounded total.

---

## Input Validation

All fields are validated before acceptance. Constraints are shown before each prompt and the system retries until input is valid.

| Field | Rules |
|-------|-------|
| **Name** | Letters, spaces, hyphens only. Minimum two words, each ≥ 2 chars. Hyphen must be between two letters. No character repeated more than twice consecutively. Length: 5–29. Must be unique. |
| **ID** | Letters and digits only. No spaces or symbols. Length: 4–9. Must be unique. |
| **Date of Birth** | Format `DD/MM/YYYY`. Leap-year-aware calendar check. Day validated against correct days per month. Student age must be 14–21. |
| **Address** | Letters, digits, spaces, and `. , - /`. No consecutive identical non-alphanumeric characters (e.g. `//` or `..`). Length: 10–39. |
| **Guardian Phone** | Digits only. Use `00` prefix for international numbers. Length: 6–15. |
| **Scores** | Integers 0–100 inclusive. |

---

## Design Notes

**Insertion-order preservation** — Sorted views (A-Z and ranked) build a temporary `Student **` pointer array, sort it, print, and free. The linked list is never reordered, so insertion order is always recoverable.

**Duplicate prevention** — Silent lookups on both name and ID run before any insert or edit. During edit, the field being changed is temporarily cleared before the uniqueness check so the student does not match itself.

**Deletion edge cases** — `delete_student` explicitly handles three structural positions: front (advance `front`), rear (retract `rear`), and middle (`prev->next = cur->next`). Memory is freed in every case.

**Unified lookup flows** — Options 2 and 5 share the same N / I / B navigation pattern. State is managed with labeled `goto` statements, the standard approach for console-state-machine navigation in C.

**GPA caching** — Grades and total are stored directly in the node and recomputed by `compute_grades()` on every score change, avoiding repeated calculation during display.

**config.h** — All menu labels and border strings are isolated in `config.h`. Updating any displayed text requires editing that file only.

---

## Testing

The project includes an automated regression test suite that validates key behaviors without manual interaction.

### How it works

Compiling with `-DTESTING` replaces `getche()` with a `stdin`-compatible wrapper (`testing_read_char`), allowing input to be piped from text files:

```c
/* school.c — excerpt */
#ifdef TESTING
static char testing_read_char(void) { ... }
#define READ_CHAR() testing_read_char()
#else
#define READ_CHAR() getche()
#endif
```

Each test run compiles the binary, pipes a fixed input file through it, captures the output, and compares it against a pre-approved expected file. Any change in output — intended or accidental — is caught immediately.

### Running

```bash
python tests/run_tests.py
```

```
Compiling with -DTESTING...
Compilation successful.

  [PASS]  test_01_add_and_view
  [PASS]  test_02_duplicate_rejection
  [PASS]  test_03_name_validation
  [PASS]  test_04_date_validation
  [PASS]  test_05_delete_positions
  [PASS]  test_06_sort_preserves_order
  [PASS]  test_07_gpa_boundaries
  [PASS]  test_08_empty_database
  [PASS]  test_09_edit_scores_skip_stop
  [PASS]  test_10_rank_preserves_order

--------------------------------------------------
  Results:  10 passed  |  0 failed  |  0 saved
--------------------------------------------------
```

Full test case documentation is in [TEST_CASES.md](TEST_CASES.md).

---

## Build

```bash
# MinGW on Windows
gcc src/main.c src/school.c src/validation.c src/input.c src/display.c \
    -o school_management_system.exe

# Or using Make
make

# Windows users: pre-built executable in Releases — no build required
```

> **Linux / macOS:** Replace `#include <conio.h>` and `getche()` with a `getchar()`-based alternative, and `stricmp()` with `strcasecmp()` from `<strings.h>`.

---

## Project Structure

```
/
├── src/
│   ├── main.c           # Entry point
│   ├── school.h         # Structs, field sizes, and public interface
│   ├── school.c         # Core: queue ops, menu flows, grade computation
│   ├── validation.h     # Internal header
│   ├── validation.c     # Input validators (name, ID, date, address, phone)
│   ├── input.h          # Internal header
│   ├── input.c          # Input collectors (prompt + validate + copy)
│   ├── display.h        # Internal header
│   ├── display.c        # All print and table functions
│   ├── types.h          # Fixed-width type aliases (u8, u16, f32 ...)
│   └── config.h         # UI labels and border strings
├── tests/
│   ├── run_tests.py     # Automated test runner
│   ├── input/           # Input sequences for each test case
│   └── expected/        # Pre-approved expected outputs
├── docs/
│   └── demo/
│       ├── 01_main_menu.png       # Static screenshot of the main menu
│       ├── 02_add_student.gif     # Adding a student with validation retries
│       ├── 03_lookup_flow.gif     # Lookup → edit → remove in one session
│       ├── 04_scores_view.png     # Score table with GPA and total
│       └── 05_sort_demo.gif       # A-Z list then insertion-order view
├── Makefile
├── TEST_CASES.md
└── README.md
```

---

## Planned Features

- **Partial name search** — Find students by first name or partial match. Multiple results will be listed with a numbered selector.

---

## Known Limitations

- `<conio.h>`, `getche()`, and `stricmp()` are Windows/MinGW only. Portable replacements are straightforward but not included.
- Sorting uses bubble sort on a pointer array — appropriate for school-sized datasets.
- Data is in-memory only — no file persistence between sessions.

---

## Author

**Mohamed Ramadan**
[LinkedIn](https://www.linkedin.com/in/muhamed-ramadan) · [GitHub](https://github.com/Muhamed-Ramadan)