//
// Created by administrator on 09.04.17.
//

#ifndef ZAD1_PIPES_H
#define ZAD1_PIPES_H

#define PROGRAMS_LIMIT 100
#define ARGUMENTS_LIMIT 500

int line_to_programs(char *line, char **programs);
int program_to_args(char *program, char **args);
void swap_write_read_pipes(int **pipe1, int **pipe2);




#endif //ZAD1_PIPES_H
