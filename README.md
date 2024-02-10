# Static-key
A header-only, standalone user-space implementation of the Linux's static key mechanism.

## Usage

C API:
```
#include <static_key.h>

struct static_key key; // Declare a static key

static_key_fuse(&key, true/false); // Set the key to true/false. Don't call it twice with the same key!!!

// Get the value of the key. Don't call it before static_key_fuse() is called.
if (static_key_branch(&key)) {
  // The key is true
} else {
  // The key is false
}

```

C++ wrapper:

```
#include <static_key.hpp>

static_key_cpp::flag key;

key.fuse(true/false);

if (key) {
  // The key is true
} else {
  // The key is false
}

```

## Reference
https://docs.kernel.org/staging/static-keys.html
