#ifndef PUSCON_UACCESS_H
#define PUSCON_UACCESS_H

#include <puscon/puscon.h>
#include <puscon/types.h>

unsigned long puscon_copy_from_user(puscon_context*, void* to, const void __user* from, unsigned long n);

unsigned long puscon_copy_to_user(puscon_context*, void __user* to, const void* from, unsigned long n);

#endif
