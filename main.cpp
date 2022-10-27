#include "list.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
    List_t list = {};
    ListCtor(&list, 9);
    ListDump(&list);

    for (size_t index = 0; index <= 15; ++index)
    {
        ListInsertAfter(&list, index, index);
    }
    ListDump(&list);

    ListInsertBefore(&list, 1, 666);
    ListDump(&list);

    ListRemove(&list, 5);

    ListLinearize(&list);
    ListDump(&list);

    ListDump(&list);
    ListTotalCleaning(&list);

    ListDtor(&list);

#ifdef LOG_MODE
    endLog(LogFile);
#endif

    return 0;
}
