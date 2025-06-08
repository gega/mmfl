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

## usage

### `RB_INIT(rb, buf, size, fd)`

Initializes the message reader.

- **Parameters**:
  - `rb`: Pointer to an `rb_t` struct (reader state).
  - `buf`: Buffer for storing incoming bytes.
  - `size`: Size of `buf`. One byte is internally reserved, so actual usable size is `size - 1`.
  - `fd`: File descriptor or opaque user context passed to your `read` function.

**Example**:

```c
char buffer[128];
rb_t reader;
RB_INIT(&reader, buffer, sizeof(buffer), fd);
```

### `RB_READMSG(rb, msg_ptr, msg_len, read_func)`

Reads and decodes the next complete message from the stream.

- **Parameters**:
  - `rb`: Pointer to the initialized rb_t struct.
  - `msg_ptr`: Output pointer to the start of the message (within rb->buf).
  - `msg_len`: Output variable, will contain the length of the message.
  - `read_func`: A function with signature int (*)(int fd, void *buf, int len) for reading data from the stream (e.g., read() or your own source).

  Returns:
    0 when a complete message is available.
    1 when more data is needed (incomplete message).
   -1 on error (e.g., malformed header or read failure).

**Example**:

```c
char *msg;
int len;
int rc = RB_READMSG(&reader, msg, len, my_read_func);
if (rc == 0) {
    // msg points to message content of length len
}
```

## generating

use printf
