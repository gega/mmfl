# mmfl
minimalist message framing library

## overview

prefixed length generic stream protocol format like netstrings

## design highlights

- implemented as macros
- no need to link to anything
- few lines only
- no heap usage
- user provided buffers
- buffer must be large enough to hold the largest message
- one memmove per message
- every read-like i/o supported
- intuitive API
- message start detection
- payload ASCII printable only (isprint())

## format

messages on wire 

```
\n11 hello world\n2 ok
msg1            msg2
```

format: `\n len <space> message`

## generating

use printf
