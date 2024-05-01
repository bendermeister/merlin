#ifndef MRLN_STACK2_H
#define MRLN_STACK2_H

// general idea is to give the user a second stack which he can use to store
// temp data which is not fit to be stored on the main stack. stack2 is
// implemented using virtual memory mappings and writing to the stack2 may cause
// a sigsegv. But this is very unlikely if you are regularly 'resetting'
// (meaning: starting to write from the base address) your stack2 and the gained
// code simplifications and performance is worth it most of the times. each
// stack is process private and cannot be used to share data between processes
void *mrln_stack2(void);

#endif // MRLN_STACK2_H
