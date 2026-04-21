/* this is a comment */

/* this is a
   multiline
   comment */
```

It starts with `/*`, ends with `*/`, and can contain **anything in between except the sequence `*/`**.

---

## Why Your Answer Was Wrong

Your answer: `^(///*./*//)$`

Several issues here:

- You were mixing **regex anchors** (`^`, `$`) which are not part of formal regular expression notation used in compiler theory — those are POSIX/grep style
- `//` doesn't mean anything special in formal RE — you likely meant `\/\*` (escaped) or just literally `/*`
- The `.` in formal RE theory doesn't mean "any character" the way it does in Perl/Python regex — in formal/theoretical RE, you must be explicit
- The structure doesn't correctly handle the constraint that `*/` cannot appear inside the comment body
- It doesn't properly account for what characters are allowed between `/*` and `*/`

---

## The Correct Regular Expression

The core challenge is expressing **"any character except `*/`"** — more precisely, any sequence of characters that does not contain `*/`.

### Building It Step by Step

**Step 1: What can appear inside the comment?**

The content between `/*` and `*/` can be:
- Any character that is **not** `*`
- A `*` that is **not followed by** `/`

So one character of valid comment content is:
```
[^*] | (*[^/])
```

That reads as: "a non-star character, OR a star followed by a non-slash."

**Step 2: That content can repeat zero or more times:**
```
([^*] | \*[^/])*
```

**Step 3: Wrap with the delimiters:**
```
/\*([^*] | \*[^/])*\*/
```

In plain notation without escaping (as you'd write on an exam):
```
/  *  (  [^*]  |  *[^/]  )*  *  /
```

Or written more cleanly:

> **`/*` `([^*] | *[^/])*` `*/`**

---

## Tracing Through an Example

For the comment `/* hi * there */`:

The body is ` hi * there ` — let's check:
- ` `, `h`, `i`, ` ` → matched by `[^*]` ✓
- `*` → could be a star, check next char: ` ` (space, not `/`) → matched by `*[^/]` ✓
- ` `, `t`, `h`, `e`, `r`, `e`, ` ` → matched by `[^*]` ✓
- Then we hit `*/` → that's the closing delimiter ✓

For something **invalid** like `/* bad */ still here */` — the comment would close at the **first** `*/`, which is correct behavior.

---

## Why This Is Tricky — The Key Insight

The hard part is always: **how do I say "not this sequence"?**

In formal RE, you can't just write `not */`. You have to think character by character:

> "I'll allow any character, as long as whenever I see a `*`, the next character is not `/`."

This is a common pattern on exams. Here's the general strategy:

---

## General Strategy for "String Not Containing Sequence XY"

If you need to exclude a two-character sequence like `*/`, think:
```
(anything that's not X)  |  (X followed by anything that's not Y)
```

Repeated with `*`, then terminated by `XY`.

So for avoiding `*/` and then ending with `*/`:
```
([^*] | *[^/])* */
```

---

## Exam Strategy: How to Approach These Questions

**Step 1 — Identify the structure.** Break the token into parts:
- What does it **start** with? → `/*`
- What does it **end** with? → `*/`
- What's in the **middle**? → anything except `*/`

**Step 2 — Handle the middle carefully.** Ask yourself: "What characters or sequences am I trying to *avoid*?"

For each dangerous character (like `*`), ask: "When is this character OK?" → When it's not followed by `/`.

**Step 3 — Write the middle as an alternation:**
```
[^*]        ← any safe character
| *[^/]     ← a star is OK if not followed by slash
```

**Step 4 — Star the whole middle** (zero or more repetitions):
```
([^*] | *[^/])*
```

**Step 5 — Combine:**
```
/* ([^*] | *[^/])* */
```

---

## Other Common RE Exam Questions — Worked Examples

### Single-line C++ comment (`//` to end of line)
```
//[^\n]*\n
```
Reads as: `//` followed by any number of non-newline characters, ending at `\n`.

### C String literal (characters between double quotes, no unescaped quote inside)
```
"([^"\\] | \\.)* "
```
Reads as: opening `"`, then any number of (non-quote non-backslash chars, OR a backslash followed by any char), then closing `"`.

### Integer literal
```
[0-9]+
```

### Floating point literal
```
[0-9]+ . [0-9]+
```

### Identifier
```
[a-zA-Z_][a-zA-Z0-9_]*