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
static FILE* List_graphviz_file = NULL;
#define LIST_GRAPHVIZ_FILE_NAME "List_graphviz.dot"
static FILE* List_html_log_file = NULL;
#define LIST_HTML_LOG_FILE_NAME "List_log.html"

#define BACK_COLOR "#34252c"

#define ZERO_LINE_COLOR "#260a18"
#define ZERO_FILL_COLOR "#77606b"
#define ZERO_FONT_COLOR "#260a18"

#define DATA_LINE_COLOR "#430523"
#define DATA_FILL_COLOR "#94285c"
#define DATA_FONT_COLOR "#260a18"

#define HEAD_LINE_COLOR "#260a18"
#define HEAD_FILL_COLOR "#591837"
#define HEAD_FONT_COLOR "#ca80a2"

#define TAIL_LINE_COLOR "#430523"
#define TAIL_FILL_COLOR "#ca80a2"
#define TAIL_FONT_COLOR "#3f1229"

#define NEXT_LINE_COLOR "#063a2e"
#define NEXT_FILL_COLOR "#06a07c"
#define NEXT_FONT_COLOR "#0a2920"

#define FREE_LINE_COLOR "#03261e"
#define FREE_FILL_COLOR "#037f62"
#define FREE_FONT_COLOR "#05120e"

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

typedef enum {ZERO = 0, FREE = -1, DATA = 1} Status_t;

#define INCORRECT_INDEX -1

#define MAX_FILE_NAME_SIZE 100
#define MAX_COMMAND_SIZE 200

struct Node_colors_t {
    const char* line_color;
    const char* fill_color;
    const char* font_color;
};

struct List_element_t {
    List_type_t value;
    size_t next_index;
    size_t prev_index;
};

struct Doubly_linked_list_t {
    List_element_t* data;

    size_t capacity;
    size_t size;

    size_t free_index;

    int graphic_dump_number;

    List_err_t error;
    List_report_t last_function_success;
};

#define PrintErrorMessage(message, object)                                                         \
    fprintf(stderr, "%s: %s(): %s - %s, line %d\n", __FILE__, __func__, message, #object, __LINE__);

#define LIST_VERIFY(dl_list) ListVerify(dl_list, __func__, __LINE__)

Doubly_linked_list_t* ListInit(size_t capacity);

List_element_t* ListDataInit(size_t capacity);

void ListDestroy(Doubly_linked_list_t* dl_list); // REVIEW return err

List_report_t ListAddHead(Doubly_linked_list_t* dl_list, List_type_t value); // REVIEW return err/success

List_report_t ListAddTail(Doubly_linked_list_t* dl_list, List_type_t value);

List_report_t ListAddIndex(Doubly_linked_list_t* dl_list, size_t index, List_type_t value);

List_report_t ListDelHead(Doubly_linked_list_t* dl_list);

List_report_t ListDelTail(Doubly_linked_list_t* dl_list);

List_report_t ListDelIndex(Doubly_linked_list_t* dl_list, size_t index);

size_t GetFreeIndex(Doubly_linked_list_t* dl_list);

List_report_t PutFreeIndex(Doubly_linked_list_t* dl_list, size_t index);

List_err_t ListVerify(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line);

List_err_t ListDiagnoseError(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line);

void ListTechnicalDump(Doubly_linked_list_t* dl_list);

void ListGraphicDump(Doubly_linked_list_t* dl_list, const char* message);

void ListStartLogging(Doubly_linked_list_t* dl_list);

void ListCreateInformationTable(Doubly_linked_list_t* dl_list);

void ListCreateDataTable(Doubly_linked_list_t* dl_list);

const char* ListGetStatus(Doubly_linked_list_t* dl_list, size_t index);

void ListCreateGraphic(Doubly_linked_list_t* dl_list);

void ListCreatePng(Doubly_linked_list_t* dl_list);

void ListCreateNodes(Doubly_linked_list_t* dl_list);

void SetNodeColors(Node_colors_t* node_colors, const char* line_color, const char* fill_color, const char* font_color);

void ListCreateNode(Doubly_linked_list_t* dl_list, size_t index, Node_colors_t* node_colors);

void ListCreateMarkers(Doubly_linked_list_t* dl_list);

void ListCreateMarker(Doubly_linked_list_t* dl_list, size_t marker_index, const char* marker_name, Node_colors_t* node_colors);

void ListCreateLinks(Doubly_linked_list_t* dl_list);

void ListCreateLink(Doubly_linked_list_t* dl_list, size_t index, const char* prev_color, const char* next_color);


int main() {
    ONDEB(
        List_log_file = fopen(LIST_LOG_FILE_NAME, "w");
        if (List_log_file == NULL) {
            printf("Cant open %s with \"w\" permission\n", LIST_LOG_FILE_NAME);
            return 1;
        }
    )
    List_html_log_file = fopen(LIST_HTML_LOG_FILE_NAME, "w");
    if (List_html_log_file == NULL) {
        printf("Cant open %s with \"w\" permission\n", LIST_GRAPHVIZ_FILE_NAME);
        return 1;
    }

    Doubly_linked_list_t* dllst_1 = ListInit(5);

    ListGraphicDump(dllst_1, "sweetie_dump1");

    ListAddHead(dllst_1, 3);
    ListAddTail(dllst_1, 3);

    ListDelHead(dllst_1);
    ListDelHead(dllst_1);

    ListAddTail(dllst_1, 7);
    ListAddHead(dllst_1, 6);
    ListAddHead(dllst_1, 56);
    ListAddHead(dllst_1, 12);

    ListGraphicDump(dllst_1, "sweetie_dump2");

    ListDestroy(dllst_1);
    fclose(List_log_file);
    fclose(List_graphviz_file);

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

    dl_list->capacity = capacity;
    dl_list->size = 0;

    dl_list->free_index = 1;

    dl_list->graphic_dump_number = 1;

    dl_list->error = LIST_NORMAL;
    dl_list->last_function_success = SUCCESS;

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


List_report_t ListAddHead(Doubly_linked_list_t* dl_list, List_type_t value) {
    assert(dl_list != NULL);

    return ListAddIndex(dl_list, HEAD_INDEX, value);
}


List_report_t ListAddTail(Doubly_linked_list_t* dl_list, List_type_t value) {
    assert(dl_list != NULL);

    return ListAddIndex(dl_list, 0, value);
}


List_report_t ListAddIndex(Doubly_linked_list_t* dl_list, size_t index, List_type_t value) {
    assert(dl_list != NULL);

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    if ((dl_list->data[index].prev_index == INCORRECT_INDEX) || (index < 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %ld is out of range or not owned by list, %s was cancelled\n",
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

    return ListDelIndex(dl_list, HEAD_INDEX);
}


List_report_t ListDelTail(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    return ListDelIndex(dl_list, TAIL_INDEX);
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
        printf("note from %s(), line %d: index %ld is out of range or not owned by list, %s was cancelled\n",
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

    size_t free_index = dl_list->free_index;

    dl_list->free_index = dl_list->data[dl_list->free_index].next_index;

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
        printf("note from %s(), line %d: index %ld is out of range or not owned by list, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return dl_list->last_function_success = INCORRECT_INPUT;
    }

    dl_list->data[index].value = POISON;
    dl_list->data[index].next_index = dl_list->free_index;
    dl_list->data[index].prev_index = INCORRECT_INDEX;

    dl_list->free_index = index;

    dl_list->size--;

    if (LIST_VERIFY(dl_list) != LIST_NORMAL)
        return dl_list->last_function_success = FATAL_ERROR;

    return dl_list->last_function_success = SUCCESS;
}


List_err_t ListVerify(Doubly_linked_list_t* dl_list, const char* calling_function_name, int from_line) { // FIXME obhod na cicli
    assert(dl_list != NULL); assert(calling_function_name != NULL); // konechnost spiska
    ONDEB(assert(List_log_file != NULL);)

    (void)ListDiagnoseError(dl_list, calling_function_name, from_line);

    ONDEB(ListGraphicDump(dl_list);)

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
    if ((dl_list->free_index < 0) || (dl_list->free_index > dl_list->capacity)) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: Fatal error - invalid free index %ld\n", calling_function_name, from_line, dl_list->free_index);)
        dl_list->error = (List_err_t)(dl_list->error | LIST_FREE_INDEX_ERROR);
    }

    if (dl_list->error == LIST_NORMAL) {
        ONDEB(fprintf(List_log_file, "Diagnostic from %s(), line %d: No fatal damage\n", calling_function_name, from_line);)
    }

    return dl_list->error;
}


void ListGraphicDump(Doubly_linked_list_t* dl_list, const char* message) { // FIXME krasnie strelochki esli cto-to ne tak
    assert(dl_list != NULL); assert(message != NULL);

    if (List_html_log_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_HTML_LOG_FILE_NAME, __func__);
        return;
    }

    if (dl_list->graphic_dump_number == 1)
        ListStartLogging(dl_list);

    fprintf(List_html_log_file, "<h2>%s</h2>\n\n", message);

    ListCreateInformationTable(dl_list);

    ListCreateDataTable(dl_list);

    ListCreateGraphic(dl_list);

    fprintf(List_html_log_file, "<hr>\n\n");

    dl_list->graphic_dump_number++;
}


void ListStartLogging(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_html_log_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_HTML_LOG_FILE_NAME, __func__);
        return;
    }

    fprintf(List_html_log_file,
        "<meta charset=\"UTF-8\">\n\n"
        "<style>\n"
        "\tbody {background-color: %s; font-family: sans-serif; margin: 20px;}\n"
        "\th1 {text-align: center; color: %s;}\n"
        "\th2 {text-align: center; color: %s;font-size: 20px; margin-top: 20px; margin-bottom: 20px;}\n"
        "\th3 {color: %s; font-size: 18px; margin-top: 40px; margin-bottom: 10px;}\n"
        "\thr {border: none; height: 3px; background-color: %s; margin: 20px 0;}\n"
        "\ttable {border-collapse: collapse; max-width: fit-content;}\n"
        "\ttd {border: 2px solid %s; padding: 10px; text-align: center; font-weight: bold; color: %s}\n"
        "\t.data_table tr:first-child td {background-color: %s; font-size: 14px; font-weight: bold; color: %s;}\n"
        "\t.list_information_table td:first-child {background-color: %s; font-size: 14px; font-weight: bold; color: %s;}\n"
        "</style>\n\n"
        "<h1>Log</h1>\n\n"
        "<hr>\n\n",
        BACK_COLOR, HEAD_FONT_COLOR, HEAD_FONT_COLOR, HEAD_FONT_COLOR, HEAD_FONT_COLOR, HEAD_LINE_COLOR,
        NEXT_FILL_COLOR, HEAD_FILL_COLOR, HEAD_FONT_COLOR, HEAD_FILL_COLOR, HEAD_FONT_COLOR);
}


void ListCreateInformationTable(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_html_log_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_HTML_LOG_FILE_NAME, __func__);
        return;
    }

    fprintf(List_html_log_file,
        "<h3>Information</h3>\n"
        "<table class = \"list_information_table\">\n"
        "\t<tr><td>size</td><td>%ld</td></tr>\n"
        "\t<tr><td>capacity</td><td>%ld</td></tr>\n"
        "\t<tr><td>head index</td><td>%ld</td></tr>\n"
        "\t<tr><td>tall index</td><td>%ld</td></tr>\n"
        "\t<tr><td>free index</td><td>%ld</td></tr>\n"
        "\t<tr><td>errors</td><td>no errors</td></tr>\n"  // FIXME errors
        "</table>\n\n",
        dl_list->size, dl_list->capacity, HEAD_INDEX, TAIL_INDEX, dl_list->free_index);
}


void ListCreateDataTable(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_html_log_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_HTML_LOG_FILE_NAME, __func__);
        return;
    }

    fprintf(List_html_log_file, "<h3>Data</h3>\n<p style=\"font-size: 16px; font-weight: bold; color: %s; margin: 16px 0;\">\n tail -> ",
           HEAD_FONT_COLOR);
    for (size_t index = TAIL_INDEX; index != 0; index = dl_list->data[index].next_index)
        fprintf(List_html_log_file, LIST_TYPE_SPECIFIER " -> ", dl_list->data[index].value);
    fprintf(List_html_log_file, "head\n</p>\n");

    fprintf(List_html_log_file, "<table class = \"data_table\">\n");
    fprintf(List_html_log_file, "\t<tr><td>index</td><td>value</td><td>prev</td><td>next</td><td>status</td></tr>\n");
    for (size_t index = 0; index <= dl_list->capacity; index++)
        fprintf(List_html_log_file, "\t<tr><td>%ld</td><td>" LIST_TYPE_SPECIFIER "</td><td>%ld</td><td>%ld</td><td>%s</td></tr>\n",
                index, dl_list->data[index].value, dl_list->data[index].prev_index, dl_list->data[index].next_index, ListGetStatus(dl_list, index));
    fprintf(List_html_log_file, "</table>\n\n");
}


const char* ListGetStatus(Doubly_linked_list_t* dl_list, size_t index) {
    assert(dl_list != NULL);

    if ((index < 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %ld is out of range, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return "";
    }

    if (index == 0)
        return "ZERO";
    if (dl_list->data[index].prev_index == INCORRECT_INDEX)
        return "FREE";
    else
        return "DATA";
}


void ListCreateGraphic(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    List_graphviz_file = fopen(LIST_GRAPHVIZ_FILE_NAME, "w");
    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    ListCreatePng(dl_list);

    fclose(List_graphviz_file);
    List_graphviz_file = NULL;

    char dump_file_name[MAX_FILE_NAME_SIZE] = {};
    sprintf(dump_file_name, "dump%d.png", dl_list->graphic_dump_number);

    char command[MAX_COMMAND_SIZE] = {};
    sprintf(command, "dot -Tpng %s -o %s", LIST_GRAPHVIZ_FILE_NAME, dump_file_name);

    system(command);

    fprintf(List_html_log_file, "<h3>Graphic</h3>\n<img src=\"%s\" alt=\"%s\">\n\n",
            dump_file_name, dump_file_name);
}


void ListCreatePng(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    fprintf(List_graphviz_file, "digraph G {\n\n");
    fprintf(List_graphviz_file, "bgcolor = \"%s\"\n", BACK_COLOR);
    fprintf(List_graphviz_file, "node [shape = none];\n\n");

    ListCreateNodes(dl_list);

    ListCreateMarkers(dl_list);

    ListCreateLinks(dl_list);

    fprintf(List_graphviz_file, "}\n");
}


void ListCreateNodes(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    for (size_t index = 0; index <= dl_list->capacity; index++) {
        Node_colors_t node_colors = {};

        if (index == 0)
            SetNodeColors(&node_colors, ZERO_LINE_COLOR, ZERO_FILL_COLOR, ZERO_FONT_COLOR);
        else if (index == dl_list->free_index)
            SetNodeColors(&node_colors, NEXT_LINE_COLOR, NEXT_FILL_COLOR, NEXT_FONT_COLOR);
        else if (dl_list->data[index].prev_index == INCORRECT_INDEX)
            SetNodeColors(&node_colors, FREE_LINE_COLOR, FREE_FILL_COLOR, FREE_FONT_COLOR);
        else if (index == HEAD_INDEX)
            SetNodeColors(&node_colors, HEAD_LINE_COLOR, HEAD_FILL_COLOR, HEAD_FONT_COLOR);
        else if (index == TAIL_INDEX)
            SetNodeColors(&node_colors, TAIL_LINE_COLOR, TAIL_FILL_COLOR, TAIL_FONT_COLOR);
        else
            SetNodeColors(&node_colors, DATA_LINE_COLOR, DATA_FILL_COLOR, DATA_FONT_COLOR);

        ListCreateNode(dl_list, index, &node_colors);
    }

    fprintf(List_graphviz_file, "{rank = same;");
    for (size_t index = 0; index <= dl_list->capacity; index++)
        fprintf(List_graphviz_file, "node%ld;%s", index, (index != dl_list->capacity) ? " " : "");
    fprintf(List_graphviz_file, "}\n\n");

    for (size_t index = 0; index < dl_list->capacity; index++)
        fprintf(List_graphviz_file, "node%ld -> node%ld [dir = forward, style = invis];\n", index, index + 1);
    fprintf(List_graphviz_file, "\n");
}


void SetNodeColors(Node_colors_t* node_colors, const char* line_color, const char* fill_color, const char* font_color) {
    assert(node_colors != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    node_colors->line_color = line_color;
    node_colors->fill_color = fill_color;
    node_colors->font_color = font_color;
}


void ListCreateNode(Doubly_linked_list_t* dl_list, size_t index, Node_colors_t* node_colors) {
    assert(dl_list != NULL); assert(node_colors != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    if ((index < 0) || (index > dl_list->capacity)) {
        printf("note from %s(), line %d: index %ld is out of range, %s was cancelled\n",
               __func__, __LINE__, index, __func__);
        return;
    }

    fprintf(List_graphviz_file,
        "node%ld [label=<\n"
            "\t<TABLE BORDER = \"1\" CELLBORDER = \"0\" CELLSPACING = \"0\" CELLPADDING=\"4\" STYLE = \"ROUNDED\"\n"
            "\tCOLOR = \"%s\"\n"
            "\tBGCOLOR = \"%s\">\n"
            "\t<TR><TD COLSPAN = \"2\"><FONT COLOR = \"%s\">index = %ld</FONT></TD></TR><HR/>\n"
            "\t<TR><TD COLSPAN = \"2\"><FONT COLOR = \"%s\">value = " LIST_TYPE_SPECIFIER "</FONT></TD></TR><HR/>\n"
            "\t<TR><TD><FONT COLOR = \"%s\">prev = %ld</FONT></TD><VR/><TD><FONT COLOR = \"%s\">next = %ld</FONT></TD></TR>\n"
            "\t</TABLE>\n"
        ">];\n\n",
        index, node_colors->line_color, node_colors->fill_color,
        node_colors->font_color, index,
        node_colors->font_color, dl_list->data[index].value,
        node_colors->font_color, dl_list->data[index].prev_index,
        node_colors->font_color, dl_list->data[index].next_index);
}


void ListCreateMarkers(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    Node_colors_t node_colors = {};

    SetNodeColors(&node_colors, NEXT_LINE_COLOR, NEXT_FILL_COLOR, NEXT_FONT_COLOR);
    ListCreateMarker(dl_list, dl_list->free_index, "free", &node_colors);

    SetNodeColors(&node_colors, HEAD_LINE_COLOR, HEAD_FILL_COLOR, HEAD_FONT_COLOR);
    ListCreateMarker(dl_list, HEAD_INDEX, "head", &node_colors);

    SetNodeColors(&node_colors, TAIL_LINE_COLOR, TAIL_FILL_COLOR, TAIL_FONT_COLOR);
    ListCreateMarker(dl_list, TAIL_INDEX, "tail", &node_colors);
}


void ListCreateMarker(Doubly_linked_list_t* dl_list, size_t marker_index, const char* marker_name, Node_colors_t* node_colors) {
    assert(dl_list != NULL); assert(marker_name != NULL); assert(node_colors != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    fprintf(List_graphviz_file,
        "%s [\n"
	        "\tlabel = \"%s\", shape = box, style = \"rounded, filled\",\n"
	        "\tcolor = \"%s\",\n"
	        "\tfillcolor = \"%s\",\n"
	        "\tfontcolor = \"%s\",\n"
	        "\tgroup = \"g1\"\n"
        "];\n\n",
        marker_name, marker_name,
        node_colors->line_color, node_colors->fill_color, node_colors->font_color);

    fprintf(List_graphviz_file, "%s -> node%ld [dir = forward, style = invis, penwidth=2, color = \"%s\"];\n\n",
            marker_name, marker_index, node_colors->fill_color);
}


void ListCreateLinks(Doubly_linked_list_t* dl_list) {
    assert(dl_list != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    for (size_t index = 1; index <= dl_list->capacity; index++) {
        Node_colors_t node_colors = {};

        if (dl_list->data[index].prev_index == INCORRECT_INDEX) {
            ListCreateLink(dl_list, index, NULL, NEXT_FILL_COLOR);
        }
        else
            ListCreateLink(dl_list, index, HEAD_FILL_COLOR, TAIL_FILL_COLOR);
    }
}


void ListCreateLink(Doubly_linked_list_t* dl_list, size_t index, const char* prev_color, const char* next_color) {
    assert(dl_list != NULL);

    if (List_graphviz_file == NULL) {
        printf("note from %s(), line %d: %s isn't open, %s was cancelled\n",
               __func__, __LINE__, LIST_GRAPHVIZ_FILE_NAME, __func__);
        return;
    }

    if (prev_color != NULL) {
        if (dl_list->data[index].prev_index != 0)
            fprintf(List_graphviz_file, "node%ld -> node%ld [dir = forward, penwidth=2, color = \"%s\", constraint = false];\n",
                    index, dl_list->data[index].prev_index, prev_color);
    }

    if (next_color != NULL) {
        if (dl_list->data[index].next_index != 0)
            fprintf(List_graphviz_file, "node%ld -> node%ld [dir = forward, penwidth=2, color = \"%s\", constraint = false];\n",
                    index, dl_list->data[index].next_index, next_color);
    }

    fprintf(List_graphviz_file, "\n");
}
