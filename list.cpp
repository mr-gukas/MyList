#include "list.h"

extern FILE* LogFile;

Node_t* ListResize(List_t* list, CapacityMode capMode, DoLinear linearMode)
{
    if (list == NULL) return NULL;

    size_t updCapacity = 0;
    
    if (capMode == UP)
    {
        if (list->size >= list->capacity)
        {
            updCapacity  = list->capacity * 2;
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
        size_t fHead   = list->freeHead;

        while (dataptr[fHead].next != 0)
        {
            fHead = list->data[fHead].next;
        }

        if (fHead == list->freeHead)
            list->freeHead = updFree;
        else
            dataptr[fHead].next = updFree;

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
 
        if (linearMode && (list->isSorted == false || (list->isSorted == true && list->tale > updCapacity)))
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

    return NULL;
}

int ListLinearize(List_t *list)
{
    if (list == NULL) return LIST_NULL_PTR;

    Node_t* dataptr = (Node_t*) calloc(1 + list->capacity, sizeof(Node_t));
    if (dataptr == NULL) return CAN_NOT_ALLOCATE_MEMORY;

    size_t ptr = list->head;

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

    list->isSorted = true;
    list->head     = 1;
    list->tale     = (list->size == 0 ? 1 : list->size);
    list->data     = dataptr;

    return LIST_STATUS_OK;
}

int ListCtor(List_t* list, size_t capacity)
{
    if (list     == NULL) return LIST_NULL_PTR;
    if (capacity == 0   ) return BAD_CAPACITY;
    
    capacity = (capacity / BASED_CAPACITY + 1) * BASED_CAPACITY;

    list->data = (Node_t*) calloc(capacity + 1, sizeof(Node_t));
    if (list->data == NULL) return CAN_NOT_ALLOCATE_MEMORY;

    list->head     = 1;
    list->tale     = 1;
    list->freeHead = 1;
    list->size     = 0;
    list->isSorted = true;
    list->capacity = capacity;

    for (size_t index = 1; index < list->capacity; ++index)
    {
        list->data[index].prev = FREEE;
        list->data[index].next = index + 1;
    }

    list->data[list->capacity].prev  = FREEE;
    list->data[list->capacity].next  = 0;

    return LIST_STATUS_OK;
}

int ListDtor(List_t* list)
{
    if (list                   == NULL              ) return LIST_NULL_PTR;
    if (ListIsDestructed(list) == LIST_IS_DESTRUCTED) return LIST_IS_DESTRUCTED;
    
    free(list->data);

    list->head     = POISON;
    list->tale     = POISON;
    list->freeHead = POISON;
    list->size     = POISON;
    list->capacity = POISON;

    return LIST_IS_DESTRUCTED;
}

int ListIsDestructed(List_t* list)
{
    if (list == NULL) return LIST_NULL_PTR;
    
    if (list->head == POISON && list->tale     == POISON && list->freeHead == POISON &&
        list->size == POISON && list->capacity == POISON)
        return LIST_IS_DESTRUCTED;
    
    return LIST_UB;
}

int ListIsEmpty(List_t* list)
{
    if (list == NULL) return LIST_NULL_PTR;
    
    if (list->head == 1 && list->tale == 1 && list->freeHead == 2 &&
        list->size == 0)
        return LIST_IS_EMPTY;

    return LIST_UB;
}

size_t ReturnPhysicalIndexFromLogical(List_t* list, size_t logIndex)
{
    if (list == NULL || logIndex == 0 || logIndex > list->capacity) return size_t (-1);

    if (list->isSorted)
        return list->head - 1 + logIndex;
    
    size_t phys  = list->head;
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
    return list->head;
}

size_t ListTale(List_t* list)
{
    return list->tale;
}

size_t ListNext(List_t* list, size_t physIndex)
{
    return list->data[physIndex].next;
}

size_t ListPrev(List_t* list, size_t physIndex)
{
    return list->data[physIndex].prev;
}

size_t ListInsertAfter(List_t *list, size_t physIndex, Elem_t value)
{
    Node_t* dataptr = ListResize(list, UP);
    if (dataptr == NULL)
    {
        list->status |= BAD_INSERT;
        return 0;
    }
    
    list->data = dataptr;
    
    size_t elemIndex = 0;

    if (physIndex != list->tale)
    {
        list->isSorted = false;
    }
    
    if (list->size == 0)
    {
        elemIndex = list->tale;

        list->freeHead = list->data[list->freeHead].next;

        list->data[list->tale].value = value;
        list->data[list->tale].next  = 0;
        list->data[list->tale].prev  = 0;
        
    }
    else if (physIndex == list->tale) //if insert after tale
    {
        elemIndex = list->freeHead;

        if (list->freeHead != list->tale + 1)
        {
            list->isSorted = false;
        }

        list->data[list->freeHead].prev  = list->tale;
        list->data[list->freeHead].value = value; 
        list->data[list->tale].next      = list->freeHead;
        list->tale                       = list->freeHead;

        size_t oldFreeHead = list->freeHead;

        list->freeHead               = list->data[oldFreeHead].next;
        list->data[oldFreeHead].next = 0; 
    }
    else //if insert after element in body
    {  
        elemIndex   = list->freeHead;

        size_t newFreeHead = list->data[list->freeHead].next;

        list->data[list->freeHead].next  = list->data[physIndex].next;
        list->data[list->freeHead].prev  = physIndex;
        list->data[list->freeHead].value = value;

        list->data[list->data[list->freeHead].prev].next = list->freeHead;
        list->data[list->data[list->freeHead].next].prev = list->freeHead;

        list->isSorted = false; 
        list->freeHead = newFreeHead;

    }

    ++list->size;

    return elemIndex;
}

size_t ListInsertBefore(List_t *list, size_t physIndex, Elem_t value)
{
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

    if (physIndex == list->head)
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

    return elemIndex;
}

void ListDumpFunc(List_t* list, size_t line, const char file[MAX_STR_SIZE], const char func[MAX_STR_SIZE])
{   
    
    fprintf(LogFile, "\n---------------------------ListDump---------------------------------------\n");
   
    if (list->status & LIST_NULL_PTR)
    {
        fprintf(LogFile, "Lists's pointer is null\n");
    }

    fprintf(LogFile, "Called at %s at %s(%lu)\n", file, func, line);
    fprintf(LogFile, "List status:\n");
    

    fprintf(LogFile, "Head: %lu\n"
                     "Tale: %lu\n"
                     "FreeHead: %lu\n"
                     "Size: %lu\n"
                     "Capacity: %lu\n",
                     list->head, list->tale, list->freeHead, list->size, list->capacity);

    fprintf(LogFile, "Num.    Previous    Value   Next\n");
    
    for (size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(LogFile, "%4lu    %8lu    %5d   %4lu\n", index, list->data[index].prev, list->data[index].value, list->data[index].next);
    }

    fprintf(LogFile, "}\n");
    fprintf(LogFile, "\n---------------------------------------------------------------------------\n");

}
