# mmfl
minimalist message framing library

prefixed length generic stream protocol format like netstrings

- implemented as macros
- no need to link to anything
- few lines only
- no heap usage
- user provided buffers
- buffer must be large enough to hold the largest message
- one memmove per message
- every read-like i/o supported
- intuitive API
