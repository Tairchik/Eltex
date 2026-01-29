struct subscriber
{
    char name[10];
    char second_name[10];
    char tel[10];
};

struct list
{
    struct elementList* head; 
    struct elementList* tail;
    long size;
};

struct elementList
{   
    struct subscriber data;
    struct elementList* next;
    struct elementList* prev;    
};

void createList(struct list* collection);
void add(struct list* collection, struct subscriber data); // Добавить в список
int pop(struct list* collection, char tel[]); // Удалить из списка по номеру телефона
struct subscriber* find(struct list* collection, char tel[]); // Поиск элемента по номеру телефона
void freeAll(struct list* collection); // Освободить всю память
