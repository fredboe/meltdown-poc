# Meltdown Proof-of-Concept

This repo consists of a proof-of-concept implementation of the Meltdown attack. The code has been tested on Ubuntu 16.04
with kernel version 4.1.30 and an i5-6400 CPU. For a detailed explanation of Meltdown, have a look at the original
paper ([Original Meltdown Paper](https://arxiv.org/pdf/1801.01207)).

### Implementation considerations

Normally, meltdown is used to read from any address in kernel space which is forbidden for a normal user process, but
the read is still done speculatively, which modifies the caches and this information can leak through cache side
channels (namely flush+reload).
In this repo, we decided to take a different approach: instead of letting the illegal read throw an exception, we
manually throw an exception (specifically by nullptr dereferencing).
This allows us to not need a kernel address when testing the program, as it no longer matters if the read throws an
exception.
We can simply enter a known address from user space and see if it leaks through the cache side channels.

### How to run?

Simply use ``make`` to build the project. Then you have two options 1. "flush+reload" 2. "meltdown" and as a second
parameter you have to enter the secret.
The program should then output the secret you entered if the side-channel transmission/meltdown attack worked.

```shell
./meltdown-demo flush+reload "secret"
./meltdown-demo meltdown "secret"
```