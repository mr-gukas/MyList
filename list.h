#include <stdio.h>
#include <stdbool.h>
#include "log/LOG.h"

#define LOG_MODE

#ifdef LOG_MODE
    #define ASSERT(condition)                                       \
        if (!(condition)){                                           \
            fprintf(stderr, "Error in %s:\n"                          \
                            "FILE: %s\n"                               \
                            "LINE: %d\n"                                \
                            "FUNCTION: %s\n",                            \
                   #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
            abort();}

    #define ASSERT_OK(list)                   \
        if (ListVerify(list) != LIST_STATUS_OK)\
        {                                       \
            ListDump(list);                      \
            ASSERT(0 && "Crashed list")           \
        }

#else
    #define ASSERT(condition) ;
    #define ASSERT_OK(stk)    ;
#endif

#define TYPE_ARG_FMT "%d"

#define $$(cmd)                         \
    if (cmd){                            \
        fprintf(stderr, "Error in " #cmd "\n");}

#define ListDump(list) \
    ListDumpFunc(list, __LINE__, __FILE__, __PRETTY_FUNCTION__)

typedef int Elem_t;

const size_t POISON         = 0xDEADBEAF;
const size_t BASED_CAPACITY = 10;
const size_t FREEE          = 31415;
const size_t MAX_STR_SIZE   = 20;

enum ListStatus
{
    LIST_STATUS_OK          = 0 << 0,
    LIST_NULL_PTR           = 1 << 0,
    LIST_DATA_NULL_PTR      = 1 << 1,
    CAN_NOT_ALLOCATE_MEMORY = 1 << 2,
    LIST_IS_DESTRUCTED      = 1 << 3,
    LIST_IS_EMPTY           = 1 << 4,
    BAD_RESIZE              = 1 << 5,
    BAD_CAPACITY            = 1 << 6,
    LIST_UB                 = 1 << 7,
    BAD_INSERT              = 1 << 8,
    BAD_REMOVE              = 1 << 9,
};

struct Node_t
{
    size_t    prev;
    Elem_t value;
    size_t    next;
};

struct List_t
{
    Node_t* data;
    size_t  head;
    size_t  tale;
    size_t  freeHead;
    size_t  size;
    size_t  capacity;
    bool    isSorted;
    int     status;
};

enum CapacityMode
{
    DOWN = 0,
    UP   = 1,
};

enum EditMode
{
    BEFORE = 0,
    AFTER  = 1,
};

enum DoLinear
{
    NO_SORT = 0, 
    SORT    = 1,
};

int ListCtor(List_t* list, size_t capacity);
int ListDtor(List_t* list);
Node_t* ListResize(List_t* list, CapacityMode capMode, DoLinear linearMode = NO_SORT);
size_t ListTale(List_t* list);
size_t ListHead(List_t* list);
size_t ListPrev(List_t* list, size_t physIndex);
size_t ListNext(List_t* list, size_t physIndex);
size_t ReturnPhysicalIndexFromLogical(List_t* list, size_t logIndex);
int ListIsEmpty(List_t* list);
int ListIsDestructed(List_t* list);
int ListLinearize(List_t *list);
size_t ListInsertAfter(List_t *list, size_t physIndex, Elem_t value);
void ListDumpFunc(List_t* list, size_t line, const char file[MAX_STR_SIZE], const char func[MAX_STR_SIZE]);
size_t ListInsertBefore(List_t *list, size_t physIndex, Elem_t value);
