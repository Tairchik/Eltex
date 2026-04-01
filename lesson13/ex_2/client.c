#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "client.h"

static Message messages[MAX_MESSAGES];
static int message_count = 0;
static int chat_scroll = 0;

static char clients[MAX_CLIENTS][32];
static int client_count = 0;
static int client_scroll = 0;

WINDOW *chat_win, *clients_win, *input_win;

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
        draw_input(buffer);

        int ch = getch();

        // ENTER
        if (ch == '\n')
        {
            if (strlen(buffer) > 0)
            {
                ui_add_message("me", buffer);
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
    }
}

int main()
{
    ui_init();

    ui_add_client("Alice");
    ui_add_client("Bob");

    ui_add_message("Alice", "Hello!");
    ui_add_message("Bob", "Hi!");

    ui_loop();

    endwin();
    return 0;
}