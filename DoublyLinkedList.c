#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NDEBUG

#ifdef DEBUG
    #define ONDEB(...) __VA_ARGS__
#else
    #define ONDEB(...)
#endif

static FILE* List_log_file = stdout;
#define LIST_LOG_FILE_NAME "list_log.txt"

typedef enum {LIST_NORMAL          = 0b00000000, LIST_DATA_ERROR      = 0b00000001,
              LIST_CAPACITY_ERROR  = 0b00000010, LIST_SIZE_ERROR      = 0b00000100,
              LIST_HEAD_INDEX_ERROR = 0b00001000, LIST_TAIL_INDEX_ERROR = 0b00010000,
              /*LIST_FREE_HEAD_ERROR = 0b00100000,*/ LIST_FREE_INDEX_ERROR = 0b01000000,
              LIST_ZERO_ERROR      = 0b10000000}                                              List_err_t;
typedef enum {SUCCESS = 0, FATAL_ERROR = 1, OVERFLOW = -1, VACUUM = -2, INCORRECT_INPUT = -3} List_report_t;

#define LIST_TYPE INT

#if LIST_TYPE == INT
    #define List_type_t int
    #define LIST_TYPE_SPECIFIER "%d"
    #define POISON 67//1666667
#elif LIST_TYPE == FLOAT
    #define List_type_t float
    #define LIST_TYPE_SPECIFIER "%f"
    #define POISON NAN
#elif LIST_TYPE == DOUBLE
    #define List_type_t double
    #define LIST_TYPE_SPECIFIER "%lf"
    #define POISON NAN
#elif LIST_TYPE == POINTER
    #define List_type_t void*
    #define LIST_TYPE_SPECIFIER "%p"
    #define POISON NULL
#else
    #define List_type_t long long int
    #define LIST_TYPE_SPECIFIER "%lld"
    #define POISON 166666666667
#endif

typedef enum {ZERO = 0, FREE = -1, DATA = 1} Status_t; // STUB -1 vmesto nulevogo elementa

#define INCORRECT_INDEX -1

struct List_element_t {
    List_type_t value;
    size_t next_index;
    size_t prev_index;
};

struct Doubly_linked_list_t { // STUB dl_list
    List_element_t* data;

    size_t capacity;
    size_t size;

    size_t FREE_INDEX;

    List_err_t error;
    List_report_t last_function_success;
};

#define PrintErrorMessage(message, object)                                                         \
    fprintf(stderr, "%s: %s(): %s - %s, line %d\n", __FILE__, __func__, message, #object, __LINE__);

Doubly_linked_list_t* ListInit(size_t capacity);

List_element_t* ListDataInit(size_t capacity);

void ListDestroy(Doubly_linked_list_t* dl_list); // FIXME return err

#define LIST_VERIFY(dl_list) ListVerify(dl_list, __func__, __LINE__)

List_err_t ListVerify(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line);

List_err_t ListDiagnoseError(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line);

void ListDump(Doubly_linked_list_t* dl_list);

List_report_t ListAddHead(Doubly_linked_list_t* dl_list, List_type_t value); // STUB return err/success

List_report_t ListAddTail(Doubly_linked_list_t* dl_list, List_type_t value);

List_report_t ListAddIndex(Doubly_linked_list_t* dl_list, size_t index, List_type_t value);

List_report_t ListDelHead(Doubly_linked_list_t* dl_list);

List_report_t ListDelTail(Doubly_linked_list_t* dl_list);

List_report_t ListDelIndex(Doubly_linked_list_t* dl_list, size_t index);

size_t GetFreeIndex(Doubly_linked_list_t* dl_list);

List_report_t PutFreeIndex(Doubly_linked_list_t* dl_list, size_t index);


int main() {
    ONDEB(
        List_log_file = fopen(LIST_LOG_FILE_NAME, "w");
        if (List_log_file == NULL) {
            printf("Cant open %s with \"w\" permission\n", LIST_LOG_FILE_NAME);
            return 1;
        }
    )

    Doubly_linked_list_t* dllst_1 = ListInit(5);

    ListDump(dllst_1);

    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 3);
    ListDump(dllst_1);
    ListDelIndex(dllst_1, 3);
    ListDump(dllst_1);
    ListDelHead(dllst_1);
    ListDump(dllst_1);
    ListDelHead(dllst_1);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 7);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 7);
    ListDump(dllst_1);
    ListAddHead(dllst_1, 7);
    ListDump(dllst_1);

    ListDestroy(dllst_1);

    return 0;
}


#define HEAD_INDEX dl_list->data[0].prev_index
#define TAIL_INDEX dl_list->data[0].next_index

Doubly_linked_list_t* ListInit(size_t capacity) {
    Doubly_linked_list_t* dl_list = (Doubly_linked_list_t*)malloc(sizeof(Doubly_linked_list_t));
    if (dl_list == NULL) {
        PrintErrorMessage("Failed to allocate memory", dl_list);
        return NULL;
    }

    dl_list->data = ListDataInit(capacity);
    if (dl_list->data == NULL) {
        free(dl_list);
        return NULL;
    }

    dl_list->error = LIST_NORMAL;
    dl_list->last_function_success = SUCCESS;

    dl_list->capacity = capacity;
    dl_list->size = 0;

    dl_list->FREE_INDEX = 1;

    return dl_list;
}


List_element_t* ListDataInit(size_t capacity) {
    if (capacity <= 0) {
        PrintErrorMessage("Incorrect value, it must be greater than 0", capacity);
        return NULL;
    }

    List_element_t* data = (List_element_t*)calloc(capacity + 1, sizeof(List_element_t)); // +1 because of 0 element
    if (data == NULL) {
        PrintErrorMessage("Failed to allocate memory", data);
        return NULL;
    }

    data[0].value = POISON;
    data[0].next_index = 0;
    data[0].prev_index = 0;

    for (int i = 1; i < capacity; i++) { // dont forgot about +1 in calloc
        data[i].value = POISON;
        data[i].next_index = i + 1;
        data[i].prev_index = INCORRECT_INDEX;
    }

    data[capacity].value = POISON;
    data[capacity].next_index = 0;
    data[capacity].prev_index = INCORRECT_INDEX;

    return data;
}


void ListDestroy(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    free(dl_list->data); // STUB ful with POISON
    free(dl_list);
}


List_err_t ListVerify(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line) {
    assert(dl_list != NULL); assert(calling_function_name != NULL); // konechnost spiska
    ONDEB(assert(List_log_file != NULL);)

    ONDEB(fprintf(List_log_file, ">>>>>\n");)

    (void)ListDiagnoseError(dl_list, calling_function_name, from_line);

    ONDEB(
        fprintf(List_log_file, "\n");
        ListDump(dl_list);
    )

    ONDEB(fprintf(List_log_file, "<<<<<\n\n");)

    return dl_list->error;
}


List_err_t ListDiagnoseError(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line) {
    assert(dl_list != NULL); assert(calling_function_name != NULL);
    ONDEB(assert(List_log_file != NULL);)

    dl_list->error = LIST_NORMAL;

    if (dl_list->data == NULL) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - data is lost, may be it did not allocated or was free\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_DATA_ERROR);
    }
    if (dl_list->capacity < 1) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid dl_list capacity, it must be positive\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_CAPACITY_ERROR);
    }
    if (dl_list->size > dl_list->capacity) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid dl_list size, it must not exceed capacity\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_SIZE_ERROR);
    }
    if (dl_list->size < 0) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid dl_list size, it can not be negative\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_SIZE_ERROR);
    }
    if ((HEAD_INDEX < 0) || (HEAD_INDEX > dl_list->capacity)) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid head index %ld\n", calling_function_name, from_line, HEAD_INDEX);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_HEAD_INDEX_ERROR);
    }
    if (dl_list->data[HEAD_INDEX].next_index != 0) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - wrong head content\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_HEAD_INDEX_ERROR);
    }
    if ((TAIL_INDEX < 0) || (TAIL_INDEX > dl_list->capacity)) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid tail index %ld\n", calling_function_name, from_line, TAIL_INDEX);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_TAIL_INDEX_ERROR);
    }
    if (dl_list->data[TAIL_INDEX].prev_index != 0) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - wrong tail content\n", calling_function_name, from_line);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_TAIL_INDEX_ERROR);
    }
    if ((dl_list->FREE_INDEX < 0) || (dl_list->FREE_INDEX > dl_list->capacity)) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid free index %ld\n", calling_function_name, from_line, dl_list->FREE_INDEX);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_FREE_INDEX_ERROR);
    }

    if (dl_list->error == LIST_NORMAL) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: No fatal damage\n", calling_function_name, from_line);)
    }

    return dl_list->error;
}


void ListDump(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    fprintf(List_log_file, "index   ");
    for (int i = 0; i <= dl_list->capacity; i++)
        fprintf(List_log_file, "%-2d ", i);
    fprintf(List_log_file, "\nvalue   ");
    for (int i = 0; i <= dl_list->capacity; i++)
        fprintf(List_log_file, "%-2d ", dl_list->data[i].value);
    fprintf(List_log_file, "\nnext    ");
    for (int i = 0; i <= dl_list->capacity; i++)
        fprintf(List_log_file, "%-2ld ", dl_list->data[i].next_index);
    fprintf(List_log_file, "\nprev    ");
    for (int i = 0; i <= dl_list->capacity; i++)
        fprintf(List_log_file, "%-2ld ", dl_list->data[i].prev_index);
    fprintf(List_log_file, "\nstatus  ");
    fprintf(List_log_file, "%-2.2s ", "Z");
    for (int i = 1; i <= dl_list->capacity; i++) {
        if (dl_list->data[i].prev_index == INCORRECT_INDEX)
            fprintf(List_log_file, "%-2.2s ", "F");
        else
            fprintf(List_log_file, "%-2.2s ", "D");
    }
    fprintf(List_log_file, "\n        ");
    for (int i = 0; i <= dl_list->capacity; i++) {
        if (i == TAIL_INDEX)
            fprintf(List_log_file, "%-2.2s ", "DT");
        else if (i == dl_list->FREE_INDEX)
            fprintf(List_log_file, "%-2.2s ", "FT");
        else
            fprintf(List_log_file, "%-2.2s ", "  ");
    }
    fprintf(List_log_file, "\n        ");
    for (int i = 0; i <= dl_list->capacity; i++) {
        if (i == HEAD_INDEX)
            fprintf(List_log_file, "%-2.2s ", "DH");
        else
            fprintf(List_log_file, "%-2.2s ", "  ");
    }
    fprintf(List_log_file, "\n\n");
}


List_report_t ListAddHead(Doubly_linked_list_t* dl_list, List_type_t value) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    size_t new_index = GetFreeIndex(dl_list);
    if (dl_list->last_function_success != SUCCESS)
        return dl_list->last_function_success;

    dl_list->data[new_index].value = value;
    dl_list->data[new_index].next_index = 0;
    dl_list->data[new_index].prev_index = HEAD_INDEX;

    dl_list->data[HEAD_INDEX].next_index = new_index;
    HEAD_INDEX = new_index;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_report_t ListAddTail(Doubly_linked_list_t* dl_list, List_type_t value) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    size_t new_index = GetFreeIndex(dl_list);
    if (dl_list->last_function_success != SUCCESS)
        return dl_list->last_function_success;

    dl_list->data[new_index].value = value;
    dl_list->data[new_index].next_index = TAIL_INDEX;
    dl_list->data[new_index].prev_index = 0;

    dl_list->data[TAIL_INDEX].prev_index = new_index;
    TAIL_INDEX = new_index;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_report_t ListAddIndex(Doubly_linked_list_t* dl_list, size_t index, List_type_t value) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if ((dl_list->data[index].prev_index == INCORRECT_INDEX) || (index <= 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %d is out of range or not owned by list, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return dl_list->last_function_success = INCORRECT_INPUT;
    }

    size_t new_index = GetFreeIndex(dl_list);
    if (dl_list->last_function_success != SUCCESS)
        return dl_list->last_function_success;

    dl_list->data[new_index].value = value;
    dl_list->data[new_index].next_index = dl_list->data[index].next_index;
    dl_list->data[new_index].prev_index = index;

    dl_list->data[dl_list->data[index].next_index].prev_index = new_index;
    dl_list->data[index].next_index = new_index;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_report_t ListDelHead(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if (dl_list->size == 0) {
        printf("note from %s(), line %d: list is already empty, %s was cancelled\n", __func__, __LINE__, __func__);
        return dl_list->last_function_success = VACUUM;
    }

    size_t old_index = HEAD_INDEX;

    dl_list->data[dl_list->data[HEAD_INDEX].prev_index].next_index = 0;
    HEAD_INDEX = dl_list->data[HEAD_INDEX].prev_index;

    if (PutFreeIndex(dl_list, old_index) != SUCCESS)
        return dl_list->last_function_success;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_report_t ListDelTail(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if (dl_list->size == 0) {
        printf("note from %s(), line %d: list is already empty, %s was cancelled\n", __func__, __LINE__, __func__);
        return dl_list->last_function_success = VACUUM;
    }

    size_t old_index = TAIL_INDEX;

    dl_list->data[dl_list->data[TAIL_INDEX].next_index].prev_index = 0;
    TAIL_INDEX = dl_list->data[TAIL_INDEX].next_index;

    if (PutFreeIndex(dl_list, old_index) != SUCCESS)
        return dl_list->last_function_success;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_report_t ListDelIndex(Doubly_linked_list_t* dl_list, size_t index) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if (dl_list->size == 0) {
        printf("note from %s(), line %d: list is already empty, %s was cancelled\n", __func__, __LINE__, __func__);
        return dl_list->last_function_success = VACUUM;
    }

    if ((dl_list->data[index].prev_index == INCORRECT_INDEX) || (index <= 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %d is out of range or not owned by list, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return dl_list->last_function_success = INCORRECT_INPUT;
    }

    dl_list->data[dl_list->data[index].next_index].prev_index = dl_list->data[index].prev_index;
    dl_list->data[dl_list->data[index].prev_index].next_index = dl_list->data[index].next_index;

    if (PutFreeIndex(dl_list, index) != SUCCESS)
        return dl_list->last_function_success;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


size_t GetFreeIndex(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL) {
        dl_list->last_function_success = FATAL_ERROR;
        return INCORRECT_INDEX;
    }

    if (dl_list->size >= dl_list->capacity) {
        printf("note from %s(), line %d: list is overflow, %s was cancelled\n", __func__, __LINE__, __func__);
        dl_list->last_function_success = OVERFLOW;
        return INCORRECT_INDEX;
    }

    size_t free_index = dl_list->FREE_INDEX;

    dl_list->FREE_INDEX = dl_list->data[dl_list->FREE_INDEX].next_index;

    dl_list->size++;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL) {
        dl_list->last_function_success = FATAL_ERROR;
        return INCORRECT_INDEX;
    }

    dl_list->last_function_success = SUCCESS;

    return free_index;
}


List_report_t PutFreeIndex(Doubly_linked_list_t* dl_list, size_t index) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if ((dl_list->data[index].prev_index == INCORRECT_INDEX) || (index <= 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %d is out of range or not owned by list, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return dl_list->last_function_success = INCORRECT_INPUT;
    }

    dl_list->data[index].value = POISON;
    dl_list->data[index].next_index = dl_list->FREE_INDEX;
    dl_list->data[index].prev_index = INCORRECT_INDEX;

    dl_list->FREE_INDEX = index;

    dl_list->size--;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}
