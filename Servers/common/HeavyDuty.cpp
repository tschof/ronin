#include "HeavyDuty.h"
#include "Utility.h"
#include <errno.h>

bool CHeavyDuty::ProcessMessage(tReqList& reqlist)
{
    tReqListIt it = reqlist.begin();
    tDuty duty = {NULL, NULL};
    pthread_t dummy = 0;

    while (it != reqlist.end())
    {
        duty = *it;
        
        if (duty.tmp)
        {
            pthread_create(&dummy, NULL, duty.theFunc, duty.pArg);
        }
        else
        {
            (duty.theFunc)(duty.pArg);
        }

        it++;
    }

    return true;
}






