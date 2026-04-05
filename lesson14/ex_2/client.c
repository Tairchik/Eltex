#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "shared_m.h"
#include "client.h"

static Message messages[MAX_MESSAGES];
static int message_count = 0;
static int chat_scroll = 0;

static char clients[MAX_CLIENTS][32];
static int clients_pids[MAX_CLIENTS] = {0};

static int client_count = 0;
static int client_scroll = 0;

WINDOW *chat_win, *clients_win, *input_win;

char name[MAX_NAME];

int shm_fd;
int end_of_ptr = 0;

sem_t *sem_write;

Message *shm_ptr;

pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;
int my_pid;
// UI DRAW

void draw_chat()
{
    werase(chat_win);
    box(chat_win, 0, 0);

    int h, w;
    getmaxyx(chat_win, h, w);

    int start = message_count - (h - 2) - chat_scroll;
    if (start < 0)
        start = 0;

    int line = 1;
    for (int i = start; i < message_count && line < h - 1; i++)
    {
        mvwprintw(chat_win, line++, 1, "%s: %s",
                  messages[i].name,
                  messages[i].text);
    }

    wrefresh(chat_win);
}

void draw_clients()
{
    werase(clients_win);
    box(clients_win, 0, 0);

    int h, w;
    getmaxyx(clients_win, h, w);

    int start = client_scroll;
    if (start < 0)
        start = 0;

    int line = 1;

    for (int i = start; i < client_count && line < h - 1; i++)
    {
        mvwprintw(clients_win, line++, 1, "%s", clients[i]);
    }

    wrefresh(clients_win);
}

void draw_input(const char *buffer)
{
    werase(input_win);
    box(input_win, 0, 0);

    wmove(input_win, 1, 1 + strlen(buffer));

    mvwprintw(input_win, 1, 1, "%s", buffer);

    wrefresh(input_win);
}

// API

void ui_add_message(const char *name, const char *text)
{
    if (message_count < MAX_MESSAGES)
    {
        strncpy(messages[message_count].name, name, 31);
        messages[message_count].name[31] = '\0';

        strncpy(messages[message_count].text, text, MAX_TEXT - 1);
        messages[message_count].text[MAX_TEXT - 1] = '\0';

        message_count++;
    }

    draw_chat();
}

void ui_add_client(const char *name)
{
    if (client_count < MAX_CLIENTS)
    {
        strncpy(clients[client_count], name, 31);
        clients[client_count][31] = '\0';
        client_count++;
    }

    draw_clients();
}

void ui_remove_client(const char *name)
{
    for (int i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i], name) == 0)
        {
            for (int j = i; j < client_count - 1; j++)
            {
                strcpy(clients[j], clients[j + 1]);
            }
            client_count--;
            break;
        }
    }

    draw_clients();
}

// INIT

void ui_init()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    keypad(input_win, TRUE);

    clear();
    refresh();

    int h, w;
    getmaxyx(stdscr, h, w);

    int input_h = 3;
    int clients_w = w / 4;

    chat_win = newwin(h - input_h, w - clients_w, 0, 0);
    clients_win = newwin(h - input_h, clients_w, 0, w - clients_w);
    input_win = newwin(input_h, w, h - input_h, 0);

    scrollok(chat_win, TRUE);

    draw_chat();
    draw_clients();
    draw_input("");

    wrefresh(chat_win);
    wrefresh(clients_win);
    wrefresh(input_win);
}

// LOOP

void ui_loop()
{
    char buffer[MAX_TEXT] = {0};
    int pos = 0;

    while (1)
    {
        pthread_mutex_lock(&ncurses_mutex);
        draw_input(buffer);
        pthread_mutex_unlock(&ncurses_mutex);

        int ch = getch();

        pthread_mutex_lock(&ncurses_mutex);
        // ENTER
        if (ch == '\n')
        {
            if (strlen(buffer) > 0)
            {
                send_message(buffer, name);
                buffer[0] = 0;
                pos = 0;
            }
        }

        // BACKSPACE
        else if (ch == KEY_BACKSPACE || ch == 127)
        {
            if (pos > 0)
                buffer[--pos] = 0;
        }

        // SCROLL CHAT
        else if (ch == KEY_UP || ch == KEY_DOWN)
        {
            int h, w;
            getmaxyx(chat_win, h, w);

            int visible = h - 2;
            int max_scroll = message_count > visible ? message_count - visible : 0;

            if (ch == KEY_UP)
            {
                if (chat_scroll < max_scroll)
                    chat_scroll++;
            }
            else if (ch == KEY_DOWN)
            {
                if (chat_scroll > 0)
                    chat_scroll--;
            }

            draw_chat();
        }

        // SCROLL CLIENTS
        else if (ch == KEY_PPAGE) // PageUp
        {
            if (client_scroll > 0)
                client_scroll--;
            draw_clients();
        }
        else if (ch == KEY_NPAGE) // PageDown
        {
            int h, w;
            getmaxyx(clients_win, h, w);

            int visible = h - 2;
            int max_scroll = client_count > visible ? client_count - visible : 0;

            if (client_scroll < max_scroll)
                client_scroll++;

            draw_clients();
        }

        // INPUT (только печатные символы)
        else if (ch >= 32 && ch <= 126)
        {
            if (pos < MAX_TEXT - 1)
            {
                buffer[pos++] = ch;
                buffer[pos] = 0;
            }
        }

        pthread_mutex_unlock(&ncurses_mutex);
    }
}

int msgIsNull(Message* msg)
{
    char* ptr = (char*) msg;
    int size = sizeof(Message);

    int i = 0;
    while (size > 0)
    {
        if (ptr[i] != 0) return 0; 
        i++;
    }
    return 1;
}


void connect_to_server(const char *name)
{
    shm_fd = shm_open(PATHNAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running\n");
        else
            perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, BUFSIZE) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shm_ptr = mmap(NULL, BUFSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Семафоры
    sem_write = sem_open(SEMWRITE, 0);
    if (sem_write == SEM_FAILED)
    {
        perror("sem_open sem_write");
        exit(EXIT_FAILURE);
    }

    sem_t *notify_join = sem_open(SEMCOUNTCLIENTS, 0);
    if (notify_join == SEM_FAILED)
    {
        sem_close(sem_write);
        perror("sem_open notify_join");
        exit(EXIT_FAILURE);
    }
    sem_post(notify_join);
    sem_close(notify_join);

    Message msg;
    msg.status = MSG_LOGIN;
    my_pid = getpid();
    msg.pid_client = my_pid;
    strncpy(msg.name, name, MAX_NAME);
    msg.name[MAX_NAME - 1] = '\0';

    sem_wait(sem_write);
    int i = 0;

    while (shm_ptr[i].pid_client != 0)
    {
        i++;
    }

    shm_ptr[i] = msg;

    memset(&shm_ptr[i + 1], 0, sizeof(Message));
    sem_post(sem_write);
}

void send_message(const char *text, const char *name)
{
    Message msg;
    msg.status = MSG_TEXT;
    msg.pid_client = my_pid;

    strncpy(msg.name, name, MAX_NAME);
    strncpy(msg.text, text, MAX_TEXT);

    sem_wait(sem_write);
    int i = end_of_ptr;

    while (msgIsNull(&shm_ptr[i]) != 0)
    {
        i++;
    }
    shm_ptr[i] = msg;

    memset(&shm_ptr[i + 1], 0, sizeof(Message));
    sem_post(sem_write);
}

void *reader_thread(void *arg)
{
    while (1)
    {
        sem_wait(sem_write);

        if (shm_ptr[end_of_ptr].pid_client != 0)
        {
            pthread_mutex_lock(&ncurses_mutex); 
            while (shm_ptr[end_of_ptr].pid_client != 0)
            {
                add_new_client(shm_ptr[end_of_ptr].pid_client, shm_ptr[end_of_ptr].name);
                if (shm_ptr[end_of_ptr].status != MSG_LOGIN)
                    ui_add_message(shm_ptr[end_of_ptr].name, shm_ptr[end_of_ptr].text);
                end_of_ptr++;
            }
            pthread_mutex_unlock(&ncurses_mutex);
        }

        sem_post(sem_write);

    }
    return NULL;
}

void add_new_client(int pid, char *name)
{
    int i = 0;
    while (i < MAX_CLIENTS && clients_pids[i] != 0)
    {
        if (clients_pids[i] == pid)
        {
            return;
        }
        i++;
    }

    if (i != MAX_CLIENTS)
    {
        clients_pids[i] = pid;
        ui_add_client(name);
    }
}

int main()
{
    printf("Enter name: ");
    scanf("%s", name);

    connect_to_server(name);

    pthread_t tid;
    pthread_create(&tid, NULL, reader_thread, NULL);

    ui_init();
    ui_loop();

    endwin();
    return 0;
}