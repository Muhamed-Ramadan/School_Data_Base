# School Management System

A console-based student database implemented in C, built as the capstone project for the C module of the NTI Embedded Systems diploma. The system manages student profiles and academic records through a fully validated, menu-driven interface backed by a dynamically allocated singly linked list.

> **[Releases](https://github.com/Muhamed-Ramadan/School_Data_Base/releases)** — Pre-built Windows executable available. No build required.

---

**Mohamed Ramadan**
[![GitHub](https://img.shields.io/badge/GitHub-Muhamed--Ramadan-black?logo=github)](https://github.com/Muhamed-Ramadan)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-muhamed--ramadan-blue?logo=linkedin)](https://www.linkedin.com/in/muhamed-ramadan)

---

## Table of Contents

- [Overview](#overview)
- [Demo](#demo)
- [Architecture](#architecture)
- [Menu](#menu)
- [Features](#features)
- [GPA System](#gpa-system)
- [Input Validation](#input-validation)
- [Design Notes](#design-notes)
- [Testing](#testing)
- [Build](#build)
- [Project Structure](#project-structure)
- [Planned Features](#planned-features)
- [Known Limitations](#known-limitations)

---

## Project Overview

This project implements a **console-based student database** for a school administration system. It supports creating and managing student profiles with full personal information, tracking academic scores across five subjects, and producing sorted and ranked views — all through a validated, menu-driven terminal interface.

The system is written in **C99** with no external libraries — all data management is built from scratch using a dynamically allocated **singly linked list modelled as a queue**, with manual memory management via `malloc` and `free`. Input validation, display, and database logic are separated across dedicated modules following a **layered architecture**, and all variables use **fixed-width types** (`u8`, `u16`, `f32`) following embedded C conventions.

The project includes an **automated regression test suite** that compiles a testing build with stdin redirection and compares output against pre-approved expected files, making it safe to refactor and extend.

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

The school database is modelled as a **queue implemented over a singly linked list**. Insertion always happens at the rear; traversal starts at the front. The `School` struct holds the two boundary pointers and the enrollment count.

```
School (queue over a singly linked list)
┌─────────────────┐
│ u16   count     │  ← enrollment count, updated on every add / remove
│ Student* front  │──► [ Node 1 ] ──► [ Node 2 ] ──► · · · ──► NULL
│ Student* rear   │───────────────────────────────► [ Node n ]
└─────────────────┘

Student (linked-list node)
┌──────────────────────┐
│ char  name[30]       │  ← full name, validated on entry
│ char  id[10]         │  ← unique identifier
│ char  birth[11]      │  ← DD/MM/YYYY
│ char  address[40]    │
│ char  phone[16]      │  ← guardian phone
│ u8    comp_science   │  ← scores: 0–100, stored as u8 (unsigned, max 255)
│ u8    science        │
│ u8    english        │
│ u8    math           │
│ u8    history        │
│ f32   total          │  ← arithmetic average, recomputed by compute_grades()
│ u8    *_grade        │  ← per-subject letter grade + overall grade (A–F)
│ Student* next  ──────┼──► next node in the list
└──────────────────────┘
```

Nodes are heap-allocated via `malloc` and linked through `next` pointers. The list is **never reordered** by display operations — sorted output is produced by building a temporary `Student **` pointer array, sorting it, printing, and freeing.

### Queue and sort diagrams

**Queue operations** — how the queue evolves across add and delete calls:

![Queue operations](docs/demo/queue_operations.svg)

**Sorted view — A-Z** — how `list_profiles_sorted()` sorts without modifying the list:

![A-Z sort](docs/demo/sort_az.svg)

**Sorted view — by score** — how `print_scores_ranked()` ranks without modifying the list:

![Rank by score](docs/demo/rank_by_score.svg)

### Fixed-width types

All variables use the smallest type that correctly represents their range, following embedded C conventions defined in `types.h`:

| Alias | Backing type | Used for |
|-------|-------------|----------|
| `u8`  | `uint8_t`   | scores (0–100), letter grades (ASCII A–F), loop indices ≤ 255 |
| `u16` | `uint16_t`  | enrollment count (0–65535), calendar year (no negatives needed) |
| `i8`  | `int8_t`    | signed indices that may reach −1 (e.g. `trim_spaces` end pointer) |
| `i32` | `int32_t`   | `scanf` targets, `printf` format widths, general signed arithmetic |
| `f32` | `float`     | total score average (0.0–100.0) |

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

### Option 1 — Add New Student Profile · `add_student()`
Collects name, date of birth, ID, address, and guardian phone via `read_name()`, `read_date()`, `read_id()`, `read_address()`, and `read_phone()`. Each field is validated before acceptance. Name and ID uniqueness are enforced with silent lookups before the node is enqueued at the rear.

### Option 2 — Student Profile Lookup, Edit & Remove · `flow_profile_lookup()`
A unified session driven by a labeled-goto state machine: search by name or ID, view the full profile card via `print_profile_card()`, then choose to edit all fields, remove the profile (Y/N confirmation triggers `delete_student()`), or search again — without returning to the main menu.

### Option 3 — View All Student Profiles (Insertion Order) · `print_all_profiles()`
Builds a temporary `Student **` pointer array via `build_pointer_array()` and prints it as a formatted table in insertion order. The linked list is not modified.

### Option 4 — List All Student Profiles (A-Z) · `list_profiles_sorted()`
Builds the same temporary pointer array, sorts it alphabetically using `sort_by_name()` (bubble sort), and prints the table. The linked list is not modified.

### Option 5 — Student Scores Lookup & Edit · `flow_scores_lookup()`
Search by name or ID, view the full score card with per-subject GPA and total via `print_score_card()`, then optionally edit all subjects or one specific subject. Updated results display immediately via `compute_grades()`.

### Option 6 — Edit All Students' Scores · `edit_all_scores()`
Choose a mode once — all subjects or one specific subject — then step through every student in the list. Per student: any key to edit, **K** to skip, **B** to stop and save progress. Grades are recomputed after each student via `compute_grades()`.

### Option 7 — View All Students' Scores (Insertion Order) · `print_all_scores()`
Builds a temporary pointer array and prints a score table in insertion order. The linked list is not modified.

### Option 8 — List All Students' Scores (Ranked by Score) · `print_scores_ranked()`
Builds a temporary pointer array, sorts it by `total` descending via `sort_by_score()`, and prints the score table. The linked list is not modified.

---

## GPA System

Scores follow the American grading scale (0–100). Letter grades are computed and stored in the node by `compute_grades()` after every score change.

| Score  | Grade |
|--------|-------|
| 90–100 | A     |
| 80–89  | B     |
| 70–79  | C     |
| 60–69  | D     |
| 0–59   | F     |

The total score is the arithmetic average of all five subjects. The overall grade letter is derived from the rounded total.

---

## Input Validation

All fields are validated by the functions in `validation.c` before acceptance. Constraints are displayed before each prompt and the system retries until input is valid.

| Field | Validator | Rules |
|-------|-----------|-------|
| **Name** | `is_valid_name()` | Letters, spaces, hyphens only. Minimum two words, each ≥ 2 chars. Hyphen must be between two letters. No character repeated more than twice consecutively. Length: 5–29. Must be unique. |
| **ID** | `is_valid_id()` | Letters and digits only. No spaces or symbols. Length: 4–9. Must be unique. |
| **Date of Birth** | `is_valid_date()` | Format `DD/MM/YYYY`. Leap-year-aware via `is_leap_year()`. Correct days per month via `days_in_month()`. Student age must be 14–21 as of today. |
| **Address** | `is_valid_address()` | Letters, digits, spaces, `.` `,` `-` `/`. No consecutive identical non-alphanumeric characters (e.g. `//`). Length: 10–39. |
| **Guardian Phone** | `is_valid_phone()` | Digits only. Use `00` prefix for international numbers. Length: 6–15. |
| **Scores** | `read_score()` | Integers 0–100 inclusive. |

---

## Design Notes

**Insertion-order preservation** — `build_pointer_array()` allocates a temporary `Student **`, fills it from the list, sorts it, prints it, and frees it. The list itself is never touched by display operations.

**Duplicate prevention** — `student_exists_by_name()` and `student_exists_by_id()` perform silent traversals before any insert or edit. During edit, the field being changed is temporarily cleared in the node before the uniqueness check so the student does not match itself (self-match prevention in `flow_profile_lookup()`).

**Deletion edge cases** — `delete_student()` explicitly handles three structural positions: front (`school->front = cur->next`), rear (`prev->next = NULL; school->rear = prev`), and middle (`prev->next = cur->next`). Memory is freed in every case.

**State-machine navigation** — `flow_profile_lookup()` and `flow_scores_lookup()` use labeled `goto` statements for menu-state transitions. This is the standard pattern for console-state-machine navigation in C, avoiding the deep nesting that nested loops would produce.

**GPA caching** — `compute_grades()` stores letter grades and total directly in the node after every score change, avoiding repeated calculation during any subsequent display operation.

**config.h** — All menu labels and border strings are isolated in `config.h`. Updating any displayed text requires editing that file only — no changes to `school.c` are needed.

---

## Testing

The project includes an automated regression test suite that validates key behaviors without manual interaction.

### How it works

Compiling with `-DTESTING` replaces `getche()` with `testing_read_char()`, a `stdin`-compatible wrapper that reads from piped input files:

```c
/* school.c — excerpt */
#ifdef TESTING
static char testing_read_char(void) { ... }
#define READ_CHAR() testing_read_char()
#else
#include <conio.h>
#define READ_CHAR() getche()
#endif
```

The runner compiles the binary, pipes each input file through it, captures the output, and compares it against a pre-approved expected file. Any change in output — intended or accidental — is caught immediately, making the suite useful for catching regressions after refactoring.

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
│   ├── main.c           # Entry point — calls run_main_menu()
│   ├── school.h         # Structs, field sizes, and public interface
│   ├── school.c         # Core: queue ops, menu flows, grade computation
│   ├── validation.h     # Internal header
│   ├── validation.c     # is_valid_name/id/date/address/phone
│   ├── input.h          # Internal header
│   ├── input.c          # read_name/id/date/address/phone/score
│   ├── display.h        # Internal header
│   ├── display.c        # print_section/end, tables, cards, sorted views
│   ├── types.h          # Fixed-width type aliases (u8, u16, i8, i32, f32)
│   └── config.h         # UI labels and border strings
├── tests/
│   ├── run_tests.py     # Automated test runner
│   ├── input/           # Input sequences for each test case
│   └── expected/        # Pre-approved expected outputs
├── docs/
│   └── demo/
│       ├── 01_main_menu.png       # Screenshot: main menu with enrollment count
│       ├── 02_add_student.gif     # GIF: adding a student with validation retries
│       ├── 03_lookup_flow.gif     # GIF: lookup, edit, and remove in one session
│       ├── 04_scores_view.png     # Screenshot: score table with GPA and total
│       ├── 05_sort_demo.gif       # GIF: A-Z list then insertion-order view
│       ├── queue_operations.svg   # Diagram: queue state across add and delete
│       ├── sort_az.svg            # Diagram: pointer array sorted A-Z, list unchanged
│       └── rank_by_score.svg      # Diagram: pointer array ranked by score, list unchanged
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
