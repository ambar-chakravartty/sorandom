# sorandom: A random number generating character device driver for Linux

  - PRNG algorithm currently used is LCG.

## Implementation Plan
   - [X] Simple LCG implementation, returns random integer on calling `read()`.
   - [X] Uses Kernel Crypto API for better PRNG algorithm, returns random integer on calling `read()`.
   - [ ] Uses Kernel Crypto API, returns random integer on calling `read()` and also exposes custom `ioctl()` call for arbitrary length output of random bytes.

## Usage

```shell
❯ cat /dev/sorandom                                                                
```
