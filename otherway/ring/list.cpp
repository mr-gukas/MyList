#include "list.h"

extern FILE* LogFile;

int ListCtor(List_t* list, size_t capacity)
{
    if (list     == NULL) return LIST_NULL_PTR;
    if (capacity == 0   ) return BAD_CAPACITY;
    
    capacity = (capacity / BASED_CAPACITY + 1) * BASED_CAPACITY;

    list->data = (Node_t*) calloc(capacity + 1, sizeof(Node_t));
    if (list->data == NULL) return CAN_NOT_ALLOCATE_MEMORY;

    list->data[0].next = 1;
    list->data[0].prev = 1;
    list->freeHead	   = 1;
    list->size         = 0;
    list->isSorted     = true;
    list->capacity     = capacity;
    
    for (size_t index = 1; index < list->capacity; ++index)
    {
        list->data[index].prev = FREEE;
        list->data[index].next = index + 1;
    }

    list->data[list->capacity].prev  = FREEE;
    list->data[list->capacity].next  = 0;
    
    ASSERT_OK(list);

    return LIST_STATUS_OK;
}

int ListDtor(List_t* list)
{
    if (list                   == NULL              ) return LIST_NULL_PTR;
    if (ListIsDestructed(list) == LIST_IS_DESTRUCTED) return LIST_IS_DESTRUCTED;
    
    free(list->data);

    list->data[0].next = POISON;
    list->data[0].next = POISON;
    list->freeHead	   = POISON;
    list->size         = POISON;
    list->capacity     = POISON;

    return LIST_IS_DESTRUCTED;
}

int ListIsDestructed(List_t* list)
{
    if (list == NULL) return LIST_NULL_PTR;
    
    if (list->data[0].prev == POISON && list->data[0].next == POISON && list->freeHead == POISON &&
        list->size         == POISON && list->capacity     == POISON)
        return LIST_IS_DESTRUCTED;
    
    return LIST_UB;
}

int ListIsEmpty(List_t* list)
{
    if (list == NULL) return LIST_NULL_PTR;
    
    if (list->data[0].prev == 1 && list->data[0].next == 1 && list->freeHead == 1 &&
        list->size == 0)
        return LIST_IS_EMPTY;

    return LIST_UB;
}

size_t ReturnPhysicalIndexFromLogicalButItIsBetterIfYouSaveIndexesInOtherPlace(List_t* list, size_t logIndex)
{
    ASSERT_OK(list);

    if (list == NULL || logIndex == 0 || logIndex > list->capacity) return size_t (-1);

    if (list->isSorted)
        return list->data[0].next - 1 + logIndex;
    
    size_t phys  = list->data[0].next;
    size_t index = 1;

    while (index < logIndex) 
    {
        phys = list->data[phys].next;
        index++;
    }

    return phys;
}

size_t ListHead(List_t* list)
{
    ASSERT_OK(list);

    return list->data[0].next;
}

size_t ListTail(List_t* list)
{
    ASSERT_OK(list);

    return list->data[0].prev;
}

size_t ListNext(List_t* list, size_t physIndex)
{
    ASSERT_OK(list);
    
    if (physIndex == 0)
        return POISON;

    return list->data[physIndex].next;
}

size_t ListPrev(List_t* list, size_t physIndex)
{
    ASSERT_OK(list);
    
    if (physIndex == 0)
        return POISON;

    return list->data[physIndex].prev;
}

size_t ListInsertAfter(List_t *list, size_t physIndex, Elem_t value)
{
    ASSERT_OK(list);

	if (physIndex > list->capacity          ||
       (list->data[physIndex].prev == FREEE &&         
       (list->size != 0 || physIndex != list->data[0].prev))) //if user want to insert after free node
    {
        return BAD_INSERT;
    }

    Node_t* dataptr = ListResize(list, UP);
    if (dataptr == NULL)
    {
        list->status |= BAD_INSERT;
        return 0;
    }
    
    list->data = dataptr;
    
    size_t elemIndex = 0;

    
    if (list->data[list->data[physIndex].next].prev == physIndex)
    {
        list->isSorted = false;
    }
    
    if (list->size == 0)
    {
        elemIndex = list->data[0].prev;

        list->freeHead = list->data[list->freeHead].next;

        list->data[list->data[0].prev].value = value;
        list->data[list->data[0].prev].next  = 0;
        list->data[list->data[0].prev].prev  = 0;
        
    }
/*    else if (physIndex == list->tail) //if insert after tail
    {
        elemIndex = list->freeHead;

        if (list->freeHead != list->tail + 1)
        {
            list->isSorted = false;
        }

        list->data[list->freeHead].prev  = list->tail;
        list->data[list->freeHead].value = value; 
        list->data[list->tail].next      = list->freeHead;
        list->tail                       = list->freeHead;

        size_t oldFreeHead = list->freeHead;

        list->freeHead               = list->data[oldFreeHead].next;
        list->data[oldFreeHead].next = 0; 
    }
    else //if insert after element in body
    {  

        size_t newFreeHead = list->data[list->freeHead].next;

        list->data[list->freeHead].next  = list->data[physIndex].next;
        list->data[list->freeHead].prev  = physIndex;
        list->data[list->freeHead].value = value;

        list->data[list->data[list->freeHead].prev].next = list->freeHead;
        list->data[list->data[list->freeHead].next].prev = list->freeHead;

        list->isSorted = false; 
        list->freeHead = newFreeHead;

    }

*/  
	else
	{
        elemIndex   = list->freeHead;

		size_t newFreeHead = list->data[list->freeHead].next;

		list->data[list->freeHead].value = value;
		list->data[list->freeHead].next  = list->data[physIndex].next;
		list->data[list->freeHead].prev  = physIndex;
		
		list->data[list->data[physIndex].next].prev = list->freeHead;
		list->data[physIndex].next                  = list->freeHead;
		
		list->freeHead = newFreeHead;

	}
		
    ++list->size;

    ASSERT_OK(list);

    return elemIndex;
}

size_t ListInsertBefore(List_t *list, size_t physIndex, Elem_t value)
{
/*    if (list == NULL || physIndex == 0) return 0;

    ASSERT_OK(list);
    
    if (physIndex == 0 || physIndex >= list->capacity)
    {
        return BAD_INSERT;
    }

   size_t elemIndex = 0;

    Node_t* dataptr = ListResize(list, UP);
    if (dataptr == NULL)
    {
        list->status |= BAD_INSERT;
        return 0;
    }
    
    list->data = dataptr;
    
    if (list->size == 0)
        elemIndex = ListInsertAfter(list, physIndex, value);

    if (physIndex == list->data[0].next)
    {
        size_t newFreeHead = list->data[list->freeHead].next;

        list->data[list->head].prev      = list->freeHead;
        list->data[list->freeHead].next  = list->head;
        list->data[list->freeHead].prev  = 0;
        list->data[list->freeHead].value = value;

        list->head     = list->freeHead;
        list->freeHead = newFreeHead;
    
        ++list->size;
        
        elemIndex = list->head;
    }

    else
    {
       elemIndex = ListInsertAfter(list, list->data[physIndex].prev, value);
    }
    
    ASSERT_OK(list);
    return elemIndex;
*/

	
}
  
Elem_t ListRemove(List_t* list, size_t physIndex)
{
    ASSERT_OK(list);
    Elem_t value = 0;
    
    if (list->size == 0)
    {
        list->status |= BAD_REMOVE;
        return POISON;
    }
	
	if (physIndex < list->data[0].prev && physIndex > list->data[0].next)
		list->isSorted = false;
	
	value = list->data[physIndex].value;

	size_t next = list->data[physIndex].next;
	size_t prev = list->data[physIndex].prev;

	list->data[physIndex].prev  = FREEE;
	list->data[physIndex].next  = list->freeHead;
	list->data[physIndex].value = 0;

	list->freeHead = physIndex;

	list->data[next].prev = prev;
	list->data[prev].next = next;

    --list->size;

    if (list->isSorted)
    {
        Node_t* dataptr = ListResize(list, DOWN);
        if (dataptr == NULL)
        {
            list->status |= BAD_REMOVE;
            return POISON;
        }

        list->data = dataptr;
    }

    ASSERT_OK(list);

    return value;
}

Node_t* ListResize(List_t* list, CapacityMode capMode, DoLinear linearMode)
{
    if (list == NULL) return NULL;
   
    ASSERT_OK(list);

    size_t updCapacity = 0;
    
    if (capMode == UP)
    {
        if (list->size >= list->capacity)
        {
            updCapacity = list->capacity * 2;
        }
        else
            return list->data;
        
        if (linearMode && (list->isSorted == false))
        {   
            if (ListLinearize(list))
                return NULL;
        }
 
        Node_t* dataptr = (Node_t *) realloc(list->data, (1 + updCapacity) * sizeof(Node_t));
        if (dataptr == NULL)
        {
            list->status |= BAD_RESIZE | CAN_NOT_ALLOCATE_MEMORY;
            return NULL;
        }

        size_t updFree = list->capacity + 1;
        //size_t fHead   = list->freeHead;

        //while (dataptr[fHead].next != 0)
        //{
        //    fHead = dataptr[fHead].next;
        //}

        //if (fHead == list->freeHead)
            list->freeHead = updFree;
        //else
        //    dataptr[fHead].next = updFree;

        for (size_t index = updFree; index < updCapacity; ++index)
        {
            dataptr[index].prev  = FREEE;
            dataptr[index].next  = index + 1;
            dataptr[index].value = 0;
        }

        dataptr[updCapacity].next  = 0;
        dataptr[updCapacity].prev  = FREEE;
        dataptr[updCapacity].value = 0;


        list->capacity = updCapacity;
        
        return dataptr;
    }

    else if (capMode == DOWN)
    {
        if (list->capacity > BASED_CAPACITY && list->size <= list->capacity * 3 / 8)
        {
            updCapacity = list->capacity / 2;
        }
        else
            return list->data;
 
        if (linearMode && (list->isSorted == false || (list->isSorted == true && list->data[0].prev > updCapacity)))
        {   
            if (ListLinearize(list))
                return NULL;
        }

        list->freeHead = 0;

        for (size_t index = updCapacity; index >= 1; --index)
        {
            if (list->data[index].prev == FREEE)
            {
                if (list->freeHead == 0)
                {
                    list->freeHead         = index;
                    list->data[index].next = 0;
                }
                else
                {
                    list->data[index].next = list->freeHead;
                    list->freeHead         = index;
                }
            }
        }

        Node_t* dataptr = (Node_t*) realloc(list->data, (1 + updCapacity) * sizeof(Node_t));
        if (dataptr == NULL)
        {
            list->status |= BAD_RESIZE | CAN_NOT_ALLOCATE_MEMORY;
            return NULL;
        }

        list->capacity = updCapacity;

        return dataptr;
    }

    list->status |= BAD_RESIZE;

    ASSERT_OK(list);

    return NULL;
}

int ListLinearize(List_t *list)
{
    if (list == NULL) return LIST_NULL_PTR;
    
    ASSERT_OK(list);

    Node_t* dataptr = (Node_t*) calloc(1 + list->capacity, sizeof(Node_t));
    if (dataptr == NULL) return CAN_NOT_ALLOCATE_MEMORY;

    size_t ptr = list->data[0].next;

    if (list->size == 0)
        ptr = 0;

    list->freeHead = 0;

    for (size_t index = 1; index <= list->capacity; ++index)
    {
        if (ptr != 0)
        {
            dataptr[index].prev  = (index == 1          ? 0 : index - 1);
            dataptr[index].next  = (index == list->size ? 0 : index + 1);
            dataptr[index].value = list->data[ptr].value;
        }
        else
        {
            if (list->freeHead == 0) 
                list->freeHead = index;
            
            dataptr[index].prev  = FREEE;
            dataptr[index].next  = (index == list->capacity ? 0 : index + 1);
        }

        ptr = list->data[ptr].next;
    }

    free(list->data);
	
	printf("---------%lu\n", dataptr[list->freeHead].next);
	
	for (size_t index = dataptr[list->freeHead].next; index <= list->capacity; ++index)
	{
		dataptr[index].next  = (index == list->capacity) ? 0 : index+1;
		dataptr[index].prev  = FREEE;
		dataptr[index].value = 0;
	}

	list->data         = dataptr;
    list->isSorted	   = true;
    list->data[0].next = 1;
    list->data[0].prev = (list->size == 0 ? 1 : list->size);
    ASSERT_OK(list);

    return LIST_STATUS_OK;
}

size_t ListInsertTail(List_t* list, Elem_t value)
{
    ASSERT_OK(list);
    return ListInsertAfter(list, ListTail(list), value);
}

size_t ListInsertHead(List_t* list, Elem_t value)
{
    ASSERT_OK(list);
    return ListInsertBefore(list, ListHead(list), value);
}

Elem_t ListRemoveTail(List_t* list)
{
    ASSERT_OK(list);
    return ListRemove(list, ListTail(list));
}

Elem_t ListRemoveHead(List_t *list)
{
    ASSERT_OK(list);
    return ListRemove(list, ListHead(list));
}

size_t FindElemByValue(List_t* list, Elem_t value)
{
    ASSERT_OK(list);

    size_t physIndex = list->data[0].next;

    while (physIndex != 0 && list->data[physIndex].value != value)
    {
        physIndex = list->data[physIndex].next;
    }

    return physIndex;
}

int ListTotalCleaning(List_t* list)
{
    if (list == NULL)
        return LIST_NULL_PTR;

    ASSERT_OK(list);

    if (ListIsEmpty(list) == LIST_IS_EMPTY)
        return LIST_IS_EMPTY;
    
    while (list->size != 0)
        ListRemoveTail(list);
    
    list->status |= LIST_IS_EMPTY;

    return LIST_STATUS_OK;
}

int ListVerify(List_t* list)
{
    if (list == NULL)                                 return LIST_NULL_PTR;
    
    if (ListIsEmpty(list) == LIST_IS_EMPTY)           return LIST_IS_EMPTY;

    if (ListIsDestructed(list) == LIST_IS_DESTRUCTED) return LIST_IS_DESTRUCTED;

    int status = LIST_STATUS_OK;

    if (list->size > list->capacity)
        status |= SIZE_MORETHAN_CAPACITY;

    if (list->data[0].prev > list->capacity)
        status |= TOO_LONG_TAIL;

    if (list->data[0].next > list->capacity)
        status |= TOO_BIG_HEAD;

    if (list->freeHead > list->capacity)
        status |= TOO_MUCH_FREEDOM;

    if (list->data == NULL) 
        status |= LIST_DATA_NULL_PTR;
    
    if (status)
    {
        list->status = status;
        return status;
    }

    if (list->data[0].prev != list->data[0].next && list->data[list->data[0].next].prev != 0)
        status |= RUINED_HEAD;
    
    if (list->data[0].prev != list->data[0].next && list->data[list->data[0].prev].next != 0) 
        status |= RUINED_TAIL;

    if (status)
    {
        list->status = status;
        return status;
    }
    
    for (size_t index = 1; index <= list->capacity; ++index)
    {
        if (list->data[index].next > list->capacity ||
            (list->data[index].prev != FREEE && list->data[index].prev > list->capacity))
        {
			printf("%lu--\n", index);
            status |= BAD_INDEX;
            break;
        }
    }

    size_t curIndex = list->data[0].next;
    size_t size     = 0;

    while (size <= list->capacity)
    {
        if (list->data[curIndex].next == 0)
        {
            if (curIndex != list->capacity && curIndex != list->data[0].prev)
            {
                status |= BAD_INDEX; 
            }

            ++size;
            break;
        }
        else if (list->data[list->data[curIndex].next].prev != FREEE    &&
                (list->data[list->data[curIndex].next].prev != curIndex ||
                (list->data[curIndex].prev != 0                         && 
                 list->data[list->data[curIndex].prev].next != curIndex)))
        {
            status |= BAD_INDEX;

            break;
        }

        curIndex = list->data[curIndex].next;
        ++size;
    }

    curIndex = list->freeHead;

    while (size < list->capacity && curIndex != 0)
    {
        if (list->data[curIndex].next == 0)
        {
            if (list->data[curIndex].prev  != FREEE)
            {
                status |= LIST_RUINED;
            }

            ++size;
            break;
        }
        else if (list->data[curIndex].prev != FREEE)
        {

            status |= LIST_RUINED;
            break;
        }
        
        curIndex = list->data[curIndex].next;
        ++size;
    }

    if (size != list->capacity)
    {
        status |= LIST_RUINED;
    }
    
    list->status = status;

    return status;
}
    
void ListDumpFunc(List_t* list, char dumpReason[MAX_STR_SIZE], ...)
{   
    ASSERT(list != NULL);

    list->status = ListVerify(list);
 
    fprintf(LogFile, "\n<hr>\n");
   
    va_list arg = {};
    va_start (arg, dumpReason);
	
	fprintf(LogFile, "<h1>");
	vfprintf(LogFile, dumpReason, arg);
	fprintf(LogFile, "</h1>\n");

    va_end (arg);

    fprintf(LogFile, "List status:\n");

    #define StatPrint_(STATUS, text) \
    if (list->status & STATUS)        \
    {                                  \
        fprintf(LogFile, #text "\n");   \
    }

    StatPrint_(LIST_RUINED,              >>>List is ruined!!!!!);
    StatPrint_(LIST_IS_EMPTY,            >>>List is empty);
    StatPrint_(LIST_IS_DESTRUCTED,       >>>List is destructed);
    StatPrint_(LIST_UB,                  >>>List has undefined behavior);
    StatPrint_(BAD_RESIZE,               >>>List has resize problem);
    StatPrint_(CAN_NOT_ALLOCATE_MEMORY,  >>>Allocate memory problems);
    StatPrint_(SIZE_MORETHAN_CAPACITY,   >>>List size more than capacity);
    StatPrint_(LIST_DATA_NULL_PTR,       >>>List data pointer is null);
    StatPrint_(BAD_CAPACITY,             >>>Incorrect list capacity);
    StatPrint_(BAD_INSERT,               >>>Insert operation troubles);
    StatPrint_(BAD_REMOVE,               >>>Remove operation troubles);
    StatPrint_(TOO_LONG_TAIL,            >>>Tail is more than capacity);
    StatPrint_(TOO_BIG_HEAD,             >>>Head is more than capacity);
    StatPrint_(TOO_MUCH_FREEDOM,         >>>Free zone head is more than capacity);
    StatPrint_(RUINED_HEAD,              >>>Ruined head);
    StatPrint_(RUINED_TAIL,              >>>Ruined tail);
    StatPrint_(EXTRA_ELEM_RUINED,        >>>Zero element is broken);
    StatPrint_(BAD_INDEX,                >>>Troubles in list index);

    if (!list->status)
    {
        fprintf(LogFile, "List is ok!\n");
    }
	
	fputc('\n', LogFile);

    FILE* DumpFile = fopen("obj/dump", "w+");

    fprintf(DumpFile, "digraph G{\n");
    fprintf(DumpFile, "node [color=black, shape=box, style=\"rounded, filled\"];\n");
    fprintf(DumpFile, "rankdir=LR;\n");

    fprintf(DumpFile, "head [fillcolor=\"#FFFEB6\", "
                      "label=\"HEAD = %lu\"];\n",
                      list->data[0].next);

    fprintf(DumpFile, "tail  [fillcolor=\"#FFFEB6\", "
                     "label=\"TAIL = %lu\"];\n",
                      list->data[0].prev);

    fprintf(DumpFile, "freeHead [fillcolor=\"#FFFEB6\", "
                     "label=\"FREE HEAD = %lu\"];\n",
                      list->freeHead);
 
    fprintf(DumpFile, "capacity [fillcolor=\"#FFFEB6\", "
                     "label=\"CAPACITY = %lu\"];\n",
                      list->capacity);

   fprintf(DumpFile, "size [fillcolor=\"#FFFEB6\", "
                     "label=\"SIZE = %lu\"];\n",
                      list->size);
    
   fprintf(DumpFile, "isSorted [fillcolor=\"#FFFEB6\", "
                     "label=\"SORTED = %s\"];\n",
                      (list->isSorted) ? "YES" : "NO");

    fprintf(DumpFile, "node [color=black, shape=record, style=\"rounded, filled\"];\n");
    fprintf(DumpFile, "\n");
    fprintf(DumpFile, "edge [style=invis, constraint=true];\n");

    for (size_t index = 0; index <= list->capacity; ++index)
    {
		if (index == 0)		  
			fprintf(DumpFile, "node%lu [fillcolor=\"#C0C0C0\",", index);
		else if (index == list->data[0].next) 
			fprintf(DumpFile, "node%lu [fillcolor=\"#FAA76C\",", index);
		else if (index == list->data[0].prev) 
			fprintf(DumpFile, "node%lu [fillcolor=\"#C1AED1\",", index);
		else if (list->data[index].prev == FREEE)
			fprintf(DumpFile, "node%lu [fillcolor=\"#98FF98\",", index);
		else
			fprintf(DumpFile, "node%lu [fillcolor=\"#FFB2D0\",", index); 

        fprintf(DumpFile, "label=\" <i> Node %lu | Val. %d | <n> Next %lu | <p> Prev.", 
			    index, list->data[index].value, list->data[index].next);
		
        if (list->data[index].prev == FREEE)
        {
            fprintf(DumpFile, " Free node \"];\n");
        }
        else
        {
            fprintf(DumpFile, " %lu \"];\n", list->data[index].prev);
        }
        
        if (index != 0) 
        {
           fprintf(DumpFile, "    node%lu -> node%lu;\n", index - 1, index);
        }
    }
	
    fprintf(DumpFile, "\n    edge [style=solid, constraint=false];\n");

    for (size_t index = 1; index <= list->capacity; ++index)
    {
        if (list->data[index].next != 0)
            fprintf(DumpFile, "node%lu: <n> -> node%lu;\n", index, list->data[index].next);
        if (list->data[index].prev != FREEE && list->data[index].prev != 0)
            fprintf(DumpFile, "node%lu: <p> -> node%lu;\n", index, list->data[index].prev);
        fprintf(DumpFile, "\n");
    }

    fprintf(DumpFile, "\n edge [style=bold, constraint=false];\n");

    fprintf(DumpFile, "head     -> node%lu; \n", list->data[0].next);
    fprintf(DumpFile, "tail     -> node%lu; \n", list->data[0].prev);
    fprintf(DumpFile, "freeHead -> node%lu; \n", list->freeHead);

    fprintf(DumpFile, "}\n");
    
    fclose(DumpFile);

    static char pngIndex    = 1;
    char dumpName[MAX_STR_SIZE] = "";
    MakePngName(dumpName, pngIndex);

    char buff[100] = "";
    sprintf(buff, "dot obj/dump -T svg -o %s", dumpName);
    system(buff);

    fprintf(LogFile, "<img src = %s>\n", dumpName + 4);
    ++pngIndex;

    #undef StatPrint_
}

void MakePngName(char* name, char num)
{
    ASSERT(name != NULL);
	
	sprintf(name, "obj/dump%03d.svg", num);
} 
