#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
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

    #define ASSERT_OK(list)                                                         \
        if (ListVerify(list) != LIST_STATUS_OK && ListIsEmpty(list) != LIST_IS_EMPTY)\
        {                                                                             \
            ListDumpFunc(list, "Troubles in programm");                             \
            ASSERT(0 && "Crashed list")                                                 \
        }

#else
    #define ASSERT(condition) ;
    #define ASSERT_OK(list)   ;
#endif

#define TYPE_ARG_FMT "%d"


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
    EXTRA_ELEM_RUINED       = 1 << 10,
    SIZE_MORETHAN_CAPACITY  = 1 << 11,
    TOO_LONG_TAIL           = 1 << 12,
    TOO_BIG_HEAD            = 1 << 13,
    TOO_MUCH_FREEDOM        = 1 << 14,
    RUINED_HEAD             = 1 << 15,
    RUINED_TAIL             = 1 << 16,
    BAD_INDEX               = 1 << 17, 
    LIST_RUINED             = 1 << 18,
};

struct Node_t
{
    Node_t* prev;
    Elem_t  value;
    Node_t* next;
};

size_t ListTail(Node_t* head);

Node_t* ListCtor(Node_t* head, Elem_t startValue);
Node_t* ListInsertAfter(Node_t* root, Elem_t value);

void ListDumpFunc(Node_t* head, char dumpReason[MAX_STR_SIZE], ...);

Elem_t ListRemove(Node_t* rmNode);

Node_t* FindElemByValue(Node_t* head, Elem_t value);
Node_t* FindElemByIndex(Node_t* head, size_t index);

int ListTotalCleaning(Node_t* head);

void MakePngName(char* name, char num);

