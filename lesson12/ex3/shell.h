int sh_start();
int sh_read_command(char** command_out);
char** sh_parse_command(char* command);
int sh_launch(char **args);
int sh_perfom_cmd(char** args);