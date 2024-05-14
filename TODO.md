# TODO
A small list for future plans and TODOs which are bigger than a single line
comment.

## Green Threads
A green thread implementation using `makecontext` and `swapcontext` I want the
go concurrency model

## str8
- rename concat to cat
- all str8 should always be 0 terminated for smoother interop
- a table which maps str8view to uintptr_t
- some slice fundamentals 
- findall
- optimized find (see findall impl in replace)

## hash function
some general purpose (see: hash table) hash function

## random number generators

## nolibc version
we shouldnt be reliant on libc

## cross platform stuff

## stack2
is stack2 a good idea?

## automatic testing
how is a git ci/cd pipeline done?

## remove mrln_ prefix it gets annoying and is not necessary
## rename str8 to s8, full on copy nullprogrom
## store strings inplace if they are less than the size of the struct itself, maybe some nice bitmagic there? 
## reimplement all stdc functions for cstr for s8s
