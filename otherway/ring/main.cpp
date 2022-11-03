#include "list.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
    List_t list = {};
    ListCtor(&list, 9);
    ListDumpFunc(&list, "List constructor");

    for (size_t index = 0; index <= 15; ++index)
    {
        ListInsertAfter(&list, index, index); 
		ListDumpFunc(&list, "Insert %d before %lu", index, index);
    }

    ListInsertAfter(&list, 0, 666);
	ListDumpFunc(&list, "Insert %d after %lu", 666, 1);

    ListRemove(&list, 5);
	ListDumpFunc(&list, "Remove %lu element", 5); 

    ListLinearize(&list);
	ListDumpFunc(&list, "List Linearize");

    ListTotalCleaning(&list);

    ListDtor(&list);

#ifdef LOG_MODE
    endLog(LogFile);
#endif

    return 0;
}
