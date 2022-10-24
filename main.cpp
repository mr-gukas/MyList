#include "list.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
    List_t list = {};
    ListCtor(&list, 9);

    for (size_t index = 0; index <= 15; ++index)
    {
        ListInsertAfter(&list, index, index);
    }

    ListDump(&list);

    ListInsertBefore(&list, 1, 666);
    ListDump(&list);
    ListDtor(&list);


#ifdef LOG_MODE
    endLog(LogFile);
#endif

    return 0;
}
