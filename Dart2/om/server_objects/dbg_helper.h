
#ifndef _h_DBGHELPER

#define _h_DBGHELPER


#include <assert.h>

#define STACKCHECK

#ifdef _DEBUG

#define STACK_CHECK_RAND 0xCD000000

#define STACK_CHECK_MASK 0x00FFFFFF

#define STACK_CHECK_END_INTERNAL() u_STACK_CHECK_EBP_VALUE_RETURN =((u_STACK_CHECK_EBP_VALUE_RETURN & STACK_CHECK_MASK)\
        | STACK_CHECK_RAND);\
if((u_STACK_CHECK_EBP_VALUE_ENTER & ~STACK_CHECK_MASK) != STACK_CHECK_RAND)\
{\
    fprintf(stderr, \
            "Corrupted u_STACK_CHECK_EBP_VALUE_ENTER!!! It's %x\n", u_STACK_CHECK_EBP_VALUE_ENTER);\
    assert((u_STACK_CHECK_EBP_VALUE_ENTER & ~STACK_CHECK_MASK) == STACK_CHECK_RAND);\
}\
if((u_STACK_CHECK_EBP_VALUE_RETURN & ~STACK_CHECK_MASK) != STACK_CHECK_RAND)\
{\
    fprintf(stderr,\
            "Corrupted u_STACK_CHECK_EBP_VALUE_RETURN!!! It's %x\n", u_STACK_CHECK_EBP_VALUE_RETURN);\
    assert((u_STACK_CHECK_EBP_VALUE_RETURN & ~STACK_CHECK_MASK) == STACK_CHECK_RAND);\
}\
if(u_STACK_CHECK_EBP_VALUE_ENTER != u_STACK_CHECK_EBP_VALUE_RETURN)\
{\
    fprintf(stderr, "Stack overflow!!!\nThe EBP should be %x but it's %x( %s )\n\n",\
            u_STACK_CHECK_EBP_VALUE_ENTER, u_STACK_CHECK_EBP_VALUE_RETURN,\
            (char*)&u_STACK_CHECK_EBP_VALUE_RETURN);\
    assert(u_STACK_CHECK_EBP_VALUE_RETURN == u_STACK_CHECK_EBP_VALUE_ENTER);\
}

//end
//


#define STACK_CHECK_BEGIN() unsigned int u_STACK_CHECK_EBP_VALUE_ENTER = 0; \
    __asm__("mov (%%rbp),%0"\
        : "=r" (u_STACK_CHECK_EBP_VALUE_ENTER));\
u_STACK_CHECK_EBP_VALUE_ENTER = (u_STACK_CHECK_EBP_VALUE_ENTER & STACK_CHECK_MASK) | STACK_CHECK_RAND

#define STACK_CHECK_END() do{unsigned int u_STACK_CHECK_EBP_VALUE_RETURN = 0;\
    __asm__("mov (%%rbp),%0"\
            : "=r" (u_STACK_CHECK_EBP_VALUE_RETURN));\
    STACK_CHECK_END_INTERNAL();}while(0)

#else
#define STACK_CHECK_BEGIN() do{}while(0);
#define STACK_CHECK_END() do{}while(0);

#endif

#endif
