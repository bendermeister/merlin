#ifndef MRLN_STACK2_H
#define MRLN_STACK2_H

// general idea is to give the user a second stack which he can use to store
// temp data which is not fit to be stored on the main stack. stack2 is
// implemented using virtual memory mappings and writing to the stack2 may cause
// a sigsegv. But this is very unlikely if you are regularly 'resetting'
// (meaning: starting to write from the base address) your stack2 and the gained
// code simplifications and performance is worth it most of the times. each
// stack is process private and cannot be used to share data between processes

// returns the currents base of the stack2 writing
void *mrln_stack2(void);

// set the base of the stack2 this is here to prevent called functions to
// overwrite stored, each subsequent call to `mrln_stack2` in
// the current process will return `new_base`. This function should always be
// called twice otherwise you will be leaking memory. The first time with the
// new_base and the second time with the old base when you no longer need the
// stored data
void mrln_stack2_set(void *new_base);

#endif // MRLN_STACK2_H
