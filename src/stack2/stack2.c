#include <merlin/stack2.h>
#include <stddef.h>
#include <sys/mman.h>

static _Thread_local void *base = NULL;
static _Thread_local void *current = NULL;

// 4 GiB stack size
#define STACK_SIZE (8ull * (1ull << 30))

__attribute__((__constructor__)) static void setup(void) {
  base = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_STACK | MAP_ANONYMOUS, -1, 0);
  if (base == MAP_FAILED) {
    base = NULL;
    return;
  }

  current = base;
}

__attribute__((__destructor__)) static void cleanup(void) {
  if (base) {
    (void)munmap(base, STACK_SIZE);
  }
}

void *mrln_stack2(void) { return current; }
void mrln_stack2_set(void *new_base) { current = new_base; }
