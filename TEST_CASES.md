# Test Cases

Detailed documentation of all test cases for the School Management System.  
Test input files are in `tests/input/` and expected outputs are in `tests/expected/`.  
Run all tests with `python tests/run_tests.py` from the repository root.

---

## How to Add a New Test Case

1. Create `tests/input/test_NN_description.txt` with the input sequence
2. Run `python tests/run_tests.py` — the new test will show `[SAVED]`
3. Review the output in `tests/output/test_NN_description.txt`
4. If correct, copy it to `tests/expected/test_NN_description.txt`
5. Next run will show `[PASS]` or `[FAIL]`

---

## Input File Format

Each line in an input file represents one keypress or one line of text input.  
Single-character lines (e.g. `1`, `A`, `N`) are read by `READ_CHAR()`.  
Multi-character lines (e.g. `Ahmed Ibrahim`) are read by `fgets()`.

---

## Test Cases

---

### test_01 — Add and View (Insertion Order)

**Purpose:** Verify that two students are added successfully and displayed in insertion order.

**Input sequence:**
```
1                        ← Add New Student Profile
Ahmed Ibrahim            ← Name
01/03/2006               ← Date of Birth
STU001                   ← ID
12 Nile St, Cairo        ← Address
01001234567              ← Guardian Phone
1                        ← Add New Student Profile
Ziad Hassan              ← Name
15/07/2005               ← Date of Birth
STU002                   ← ID
45 Pyramids Rd, Giza     ← Address
00201098765432           ← Guardian Phone
3                        ← View All Student Profiles (Insertion Order)
D                        ← Exit
```

**Expected behavior:**
- Both students added successfully
- View shows Ahmed first, Ziad second (insertion order preserved)
- Header shows "2 students enrolled"

---

### test_02 — Duplicate Rejection

**Purpose:** Verify that duplicate name and duplicate ID are both rejected.

**Input sequence:**
```
1                        ← Add first student (Ahmed Ibrahim / STU001)
Ahmed Ibrahim
01/03/2006
STU001
12 Nile St, Cairo
01001234567
1                        ← Attempt to add second student
Ahmed Ibrahim            ← REJECTED: name already exists
Basel Mohamed            ← Accepted
10/05/2006
STU001                   ← REJECTED: ID already exists
STU003                   ← Accepted
15 Hassan Rd, Alex
00201112345678
3                        ← View to confirm two distinct students
D
```

**Expected behavior:**
- First `Ahmed Ibrahim` attempt rejected with duplicate name message
- First `STU001` attempt rejected with duplicate ID message
- Final database contains Ahmed Ibrahim (STU001) and Basel Mohamed (STU003)

---

### test_03 — Name Validation

**Purpose:** Verify that six invalid name formats are rejected before a valid name is accepted.

**Invalid inputs tested:**
| Input | Reason rejected |
|-------|----------------|
| `Ahmed` | Single word — minimum two words required |
| `A Ibrahim` | First word less than 2 characters |
| `Ahmed  Ibrahim` | Double space |
| `Ahhhmed Ibrahim` | Character repeated more than twice in a row |
| `Ahmed-Ibrahim` | Hyphen not between two words (no space separation) |
| `Al- Ibrahim` | Hyphen not followed immediately by a letter |

**Valid input:** `Ahmed Ibrahim`

**Expected behavior:** Six "Invalid" messages, then successful profile creation.

---

### test_04 — Date of Birth Validation

**Purpose:** Verify calendar-aware and age-range validation.

**Invalid inputs tested:**
| Input | Reason rejected |
|-------|----------------|
| `01/01/2000` | Student would be older than 21 |
| `01/01/2015` | Student would be younger than 14 |
| `29/02/2007` | 2007 is not a leap year |
| `31/04/2006` | April has only 30 days |
| `1/1/2006` | Wrong format (missing leading zeros) |

**Valid input:** `29/02/2008` (2008 is a leap year; age falls in range)

**Expected behavior:** Five "Invalid" messages, then successful profile creation.

---

### test_05 — Delete from All Positions

**Purpose:** Verify correct deletion from the middle, front, and rear of the linked list.

**Setup:** Three students added — Ahmed Ibrahim (STU001), Basel Mohamed (STU002), Ziad Hassan (STU003).

**Deletion sequence:**
1. Delete STU002 (Basel) — **middle node**
2. Verify with View — Ahmed and Ziad remain
3. Delete STU001 (Ahmed) — **front node**
4. Verify with View — only Ziad remains
5. Delete STU003 (Ziad) — **rear node** (also the only node)
6. Verify with View — "Database is empty"

**Expected behavior:**
- Each deletion succeeds with confirmation message
- After all deletions, header shows "No students enrolled"
- View on empty database returns "Database is empty"

---

### test_06 — Sorted View Does Not Affect Insertion Order

**Purpose:** Verify that the pointer-array sorting used by option 4 does not modify the linked list.

**Setup:** Students added in order: Ziad Hassan (STU001), Ahmed Ibrahim (STU002), Basel Mohamed (STU003).

**Sequence:**
1. Option 4 — List A-Z
2. Option 3 — View insertion order

**Expected behavior:**
- Option 4 shows: Ahmed, Basel, Ziad (alphabetical)
- Option 3 shows: Ziad, Ahmed, Basel (original insertion order unchanged)

---

### test_07 — GPA Boundary Scores

**Purpose:** Verify correct letter grade assignment at each grade boundary.

**Scores entered:** 90 (A boundary), 80 (B boundary), 70 (C boundary), 60 (D boundary), 59 (F)

**Expected results:**
| Subject | Score | Expected Grade |
|---------|-------|----------------|
| Comp. Science | 90 | A |
| Science | 80 | B |
| English | 70 | C |
| Math | 60 | D |
| History | 59 | F |
| Total | 71.8 | C |

---

### test_08 — Empty Database Operations

**Purpose:** Verify that all relevant options handle an empty database gracefully.

**Options tested on empty database:**
| Option | Expected response |
|--------|------------------|
| 2 (Lookup) | "Database is empty" |
| 3 (View profiles) | "Database is empty" |
| 4 (List A-Z) | "Database is empty" |
| 5 (Scores lookup) | "Database is empty" |
| 6 (Edit all scores) | "Database is empty" |
| 7 (View scores) | "Database is empty" |
| 8 (Ranked list) | "Database is empty" |

**Expected behavior:** No crash, no undefined behavior — clean message for each option.

---

### test_09 — Edit All Scores with Skip and Stop

**Purpose:** Verify that K (skip) and B (stop and save) work correctly during bulk score editing.

**Setup:** Three students — Ahmed Ibrahim, Basel Mohamed, Ziad Hassan.

**Edit sequence:**
- Mode: All subjects
- Ahmed Ibrahim: **Edit** → scores 90, 85, 88, 92, 80 entered
- Basel Mohamed: **K** (skip)
- Ziad Hassan: **B** (stop and save)

**Expected behavior:**
- Ahmed Ibrahim's scores updated to 90/85/88/92/80, total 87.0, grade B
- Basel Mohamed's scores remain 0 (skipped)
- Ziad Hassan's scores remain 0 (stopped before reaching edit)
- View scores confirms the above state

---

### test_10 — Ranked View Does Not Affect Insertion Order

**Purpose:** Verify that the pointer-array sorting used by option 8 does not modify the linked list.

**Setup:** Three students with different scores — Ahmed (total 70), Basel (total 90), Ziad (total 80). Added in that order.

**Sequence:**
1. Option 8 — List by total score (ranked)
2. Option 7 — View scores (insertion order)

**Expected behavior:**
- Option 8 shows: Basel (90), Ziad (80), Ahmed (70) — ranked descending
- Option 7 shows: Ahmed, Basel, Ziad — original insertion order unchanged

---

## Planned Test Cases

The following test cases are planned for future implementation:

- **Partial name search** — once the feature is implemented, verify that searching by first name or partial name returns all matching results
- **Edit profile then verify** — edit all fields of a student and confirm changes via lookup
- **Case-insensitive search** — search for `ahmed ibrahim` and `AHMED IBRAHIM` and confirm both find the same record
- **Score edit single subject** — use option 6 with mode S and verify only the selected subject changes
- **Scores lookup then edit** — use option 5 to find a student, edit one subject, and confirm the updated score card
