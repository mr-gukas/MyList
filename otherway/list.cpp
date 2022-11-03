#include "list.h"

extern FILE* LogFile;

Node_t* ListCtor(Node_t* head, Elem_t startValue)
{
	head = (Node_t*) calloc(1, sizeof(Node_t));
	
	head->value = startValue;
	
	return head;
}

size_t ListTail(Node_t* head)
{
    ASSERT(head != NULL);

	size_t	index   = 1;
	Node_t* curNode = head;

	while (curNode->next != NULL)
	{
		++index;
		curNode = curNode->next;
	}

	return index;
}

Node_t* ListInsertAfter(Node_t* root, Elem_t value)
{
	if (root == NULL) return NULL;
	
	if (root->next == NULL)
	{
		root->next = (Node_t*) calloc(1, sizeof(Node_t));

		if (root->next == NULL)
	    {
            return NULL;
        }
   
		root->next->next  = NULL;
		root->next->value = value;
		root->next->prev  = root;
	
		return root->next;
	}
	else
	{
		Node_t* curNext = root->next;
		Node_t* newNext = (Node_t*) calloc(1, sizeof(Node_t));
		
		if (newNext == NULL)
	    {
            return NULL;
        }
   
		newNext->value = value;
		newNext->next  = curNext;
		newNext->prev  = root;
		
		curNext->prev = newNext;
		root->next    = newNext;
				
		return newNext;
	}

    return NULL;
}

Elem_t ListRemove(Node_t* rmNode)
{
	if (rmNode == NULL) return POISON;

	Elem_t rmVal = 0;
	
	if (rmNode->next == NULL)
	{
		rmNode->prev->next = NULL;
		rmVal              = rmNode->value;

		free(rmNode);
	}
	else
	{
		Node_t* newNext = rmNode->next;
		Node_t* newPrev = rmNode->prev;

		rmNode->prev->next = newNext;
		rmNode->next->prev = newPrev;
		
		rmVal = rmNode->value;

		free(rmNode);
	}

    return rmVal;
}

Node_t* FindElemByValue(Node_t* head, Elem_t value)
{
    ASSERT(head);

	Node_t* curNode = head; 

    while (curNode->next != NULL)
    {
		if (curNode->value == value)
			break;

		curNode = curNode->next;
    }
	
	if (curNode->value != value)
		return NULL;

	return curNode;
}

Node_t* FindElemByIndex(Node_t* head, size_t index)
{
    ASSERT(head);

	Node_t* curNode  = head; 
	size_t  curIndex = 1;

    while (curNode->next != NULL)
    {
		if (curIndex == index)
			break;

		curNode = curNode->next;
		++curIndex;
    }
	
	if (curIndex != index)
		return NULL;

	return curNode;
}

int ListTotalCleaning(Node_t* head)
{
    if (head == NULL)
        return 1;
	
	Node_t* curNode = head;
	Node_t* rmNode  = NULL;

    while (curNode != NULL)
	{
		rmNode  = curNode;
		curNode	= curNode->next;
		
		free(rmNode);
	}
	
    return LIST_STATUS_OK;
}

void ListDumpFunc(Node_t* head, char dumpReason[MAX_STR_SIZE], ...)
{   
    ASSERT(head != NULL);

    fprintf(LogFile, "\n<hr>\n");
   
    va_list arg = {};
    va_start (arg, dumpReason);
	
	fprintf(LogFile, "<h1>");
	vfprintf(LogFile, dumpReason, arg);
	fprintf(LogFile, "</h1>\n");

    va_end (arg);

    fprintf(LogFile, "List status:\n");


	fputc('\n', LogFile);

    FILE* DumpFile = fopen("obj/dump", "w+");

    fprintf(DumpFile, "digraph G{\n");
    fprintf(DumpFile, "node [color=black, shape=box, style=\"rounded, filled\"];\n");
    fprintf(DumpFile, "rankdir=LR;\n");

   fprintf(DumpFile, "size [fillcolor=\"#FFFEB6\", "
                     "label=\"SIZE = %lu\"];\n",
                      ListTail(head));
    
    fprintf(DumpFile, "node [color=black, shape=record, style=\"rounded, filled\"];\n");
    fprintf(DumpFile, "\n");
    fprintf(DumpFile, "edge [style=invis, constraint=true];\n");
	
	Node_t* curNode = head;
	size_t  index   = 1;

	while (curNode != NULL)
    {
		if (index == 1) 
			fprintf(DumpFile, "node%lu [fillcolor=\"#FAA76C\",", index);
		else if (index == ListTail(head)) 
			fprintf(DumpFile, "node%lu [fillcolor=\"#C1AED1\",", index);
		else
			fprintf(DumpFile, "node%lu [fillcolor=\"#FFB2D0\",", index); 

        fprintf(DumpFile, "label=\" <i> Node %lu | Address %p | Val. %d | <n> Next %p | <p> Prev. %p \"];\n", 
			    index, curNode, curNode->value, curNode->next, curNode->prev);
		
		if (index != 1)
			fprintf(DumpFile, "    node%lu -> node%lu;\n", index - 1, index);

		++index;
		curNode = curNode->next;
    }
	
    fprintf(DumpFile, "\n    edge [style=solid, constraint=false];\n");
	
	curNode = head;
	index   = 1;

	while (curNode != NULL)
    {
        if (curNode->next != NULL)
            fprintf(DumpFile, "node%lu: <n> -> node%lu;\n", index, index + 1);

        if (curNode->prev != NULL && index != 1)
            fprintf(DumpFile, "node%lu: <p> -> node%lu;\n", index, index - 1);
        fprintf(DumpFile, "\n");

		curNode = curNode->next;
		++index;
    }

    fprintf(DumpFile, "\n edge [style=bold, constraint=false];\n");

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
