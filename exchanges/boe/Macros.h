#ifndef __MACROS_H__
#define __MACROS_H__

#define member_sizeof(type, member) sizeof(((type *) 0)->member)
#define narray(array) (sizeof (array) / sizeof (array)[0])

#endif
