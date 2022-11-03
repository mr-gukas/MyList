#include "list.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
	Node_t* head = ListCtor(head, 1);
	
	Node_t* curNode = head;

    for (Elem_t index = 2; index <= 10; ++index)
    {
        curNode = ListInsertAfter(curNode, index); 
		ListDumpFunc(head, "Insert %d after %p", index, curNode);
    }
	
	Node_t* rmNode = FindElemByIndex(head, 5);

    ListRemove(rmNode);
	ListDumpFunc(head, "Remove %luth element", 5); 

    ListTotalCleaning(head);
	
#ifdef LOG_MODE
    endLog(LogFile);
#endif

    return 0;
}
