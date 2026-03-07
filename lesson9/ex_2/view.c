#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include <stdlib.h>
#include "file_manager.h"
#include <time.h>

#define main_w 120    // Ширина основного окна
#define main_h 30     // Высота основного окна
#define sub_w 60      // Ширина под-окон
#define sub_h 30      // Высота под-окон

typedef struct {
    Files* data;      // Список файлов в этой панели
    WINDOW* wnd;      // Указатель на окно (левое или правое)
    int selected;     // Индекс файла, на котором стоит курсор
    int offset;       // Смещение прокрутки (какой файл отображается самым верхним)
} PanelState;

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

void init_windows(WINDOW** wnd_main, WINDOW** subwnd_left, WINDOW** subwnd_right)
{
    initscr();
    if (has_colors()) 
    {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    }

    signal(SIGWINCH, sig_winch);
    signal(SIGWINCH, sig_winch);
    cbreak();
    curs_set(0);
    refresh();
    WINDOW* border_wnd_left;
    WINDOW* border_wnd_right;
    
    *wnd_main = newwin(main_h, main_w, 0, 0);

    border_wnd_left = derwin(*wnd_main, sub_h, sub_w, 0, 0);
    *subwnd_left = derwin(border_wnd_left, sub_h - 2, sub_w - 2, 1, 1);
    box(border_wnd_left, '|', '-');

    border_wnd_right = derwin(*wnd_main, sub_h, sub_w, 0, sub_w);
    *subwnd_right = derwin(border_wnd_right, sub_h - 2, sub_w - 2, 1, 1);
    box(border_wnd_right, '|', '-');

    wrefresh(*wnd_main);

    refresh();
}

void addFilesIntoWindow(PanelState* panel, int is_active)
{
    if (!panel->data || !panel->wnd) return;

    int max_y, max_x;
    getmaxyx(panel->wnd, max_y, max_x);
    werase(panel->wnd);

    // Рисуем заголовки
    wattron(panel->wnd, A_UNDERLINE | A_BOLD);
    mvwprintw(panel->wnd, 0, 0, "%-20s %-10s %-18s", "Name", "Size", "Last Modified");
    wattroff(panel->wnd, A_UNDERLINE | A_BOLD);

    // Количество строк, доступных для файлов (вычитаем 1 строку под заголовок)
    int visible_lines = max_y - 1;

    // Если курсор ушел выше видимой области, двигаем камеру вверх
    if (panel->selected < panel->offset) 
    {
        panel->offset = panel->selected;
    }
    // Если курсор ушел ниже видимой области, двигаем камеру вниз
    if (panel->selected >= panel->offset + visible_lines) 
    {
        panel->offset = panel->selected - visible_lines + 1;
    }

    for (int i = 0; i < visible_lines; i++)
    {
        int file_idx = i + panel->offset; // Реальный индекс файла в массиве

        // Если вышли за пределы массива файлов — прерываем цикл
        if (file_idx >= panel->data->size_arr) break;

        char* name = panel->data->array[file_idx].dirent->d_name;
        long long size = (long long)panel->data->array[file_idx].stat->st_size;
        
        char time_str[20];
        struct tm *tm_info = localtime(&panel->data->array[file_idx].stat->st_mtime);
        strftime(time_str, sizeof(time_str), "%d.%m.%y %H:%M", tm_info);

        // Подсветка курсора (только если панель активна)
        if (file_idx == panel->selected && is_active) 
        {
            wattron(panel->wnd, A_REVERSE);
        } 
        // Для неактивной панели тусклое выделение
        else if (file_idx == panel->selected && !is_active) 
        {
            wattron(panel->wnd, A_DIM);
        } 
        else 
        {
            // Обычные цвета для файлов и папок
            if (panel->data->array[file_idx].dirent->d_type == 4) 
            {
                wattron(panel->wnd, COLOR_PAIR(1) | A_BOLD);
            } 
            else 
            {
                wattron(panel->wnd, COLOR_PAIR(2));
            }
        }

        // Рисуем строку (i + 1, т.к. нулевая строка — это заголовок)
        mvwprintw(panel->wnd, i + 1, 0, "%-20.20s %10lld %18s", name, size, time_str);

        wattrset(panel->wnd, A_NORMAL);
    }

    wrefresh(panel->wnd);
}


int main() 
{
    WINDOW *wnd, *subwnd_left, *subwnd_right;
    init_windows(&wnd, &subwnd_left, &subwnd_right);
    keypad(stdscr, TRUE);

    PanelState panels[2];
    
    // Левая панель
    panels[0].data = getFilesInfo("."); 
    panels[0].wnd = subwnd_left;
    panels[0].selected = 0;
    panels[0].offset = 0;

    // Правая панель
    panels[1].data = getFilesInfo("/"); 
    panels[1].wnd = subwnd_right;
    panels[1].selected = 0;
    panels[1].offset = 0;

    int active_panel = 0; // 0 - левая, 1 - правая
    int ch;

    while (1) 
    {
        // Отрисовываем обе панели
        addFilesIntoWindow(&panels[0], active_panel == 0);
        addFilesIntoWindow(&panels[1], active_panel == 1);
        
        ch = getch();

        if (ch == 'q' || ch == 'Q') break; // Выход

        if (ch == '\t') // Клавиша TAB для переключения
        { 
            active_panel = 1 - active_panel;
            continue;
        }

        // Получаем указатель на активную панель
        PanelState* cur = &panels[active_panel];

        switch(ch) 
        {
            case KEY_UP:
                if (cur->selected > 0) cur->selected--;
                break;
            case KEY_DOWN:
                if (cur->selected < cur->data->size_arr - 1) cur->selected++;
                break;
            case '\n': // Enter
                if (cur->data->array[cur->selected].dirent->d_type == 4) // Если выбранный файл - папка
                {
                    char* target = cur->data->array[cur->selected].dirent->d_name;
                    if (changeDir(&cur->data, target) == 0) 
                    {
                        cur->selected = 0; // Сбрасываем курсор
                        cur->offset = 0;   // Сбрасываем прокрутку
                    }
                }
                break;
        }
    }
    
    delwin(subwnd_left);
    delwin(subwnd_right);
    delwin(wnd);

    freeFiles(panels[0].data);
    freeFiles(panels[1].data);

    endwin();
    return 0;
}