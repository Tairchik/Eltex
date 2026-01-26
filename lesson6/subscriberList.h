struct subscriber
{
    char name[10];
    char second_name[10];
    char tel[10];
};

struct list
{
    struct subscriber data;
    struct list* next;
    struct list* prev;    
};

struct list* createList(struct subscriber data); // Создать список
void add(struct list* head, struct subscriber data); // Добавить в список
void pop(char tel); // Удалить из списка по номеру телефона
void find(char tel); // Поиск элемента по номеру телефона
void freeAll(struct subscriber data); // Освободить всю память
