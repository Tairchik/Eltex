#define MAX_MESSAGES 1000
#define MAX_CLIENTS 100

void draw_chat();
void draw_clients();
void draw_input(const char *buffer);
void ui_add_message(const char *name, const char *text);
void ui_add_client(const char *name);
void ui_remove_client(const char *name);
void ui_init();
void ui_loop();
void *reader_thread(void *arg);
void send_message(const char *text, const char *name);
void connect_to_server(const char *name);
void add_new_client(int pid, char* name);