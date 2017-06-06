/*
 * args_reader.c
 * Author: Josh Leath
 * Last Updated: 6/5/17
 */
#include "../include/args_reader.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

/*
 * A helper function for reading the arguments to the csim program.
 * Returns 0 if the args were not entered properly, else 1.
 */
int get_args(program_args* args, int argc, char** argv)
{
    int op;
    while ((op = getopt(argc, argv, OPT_STR)) != -1) {
        switch (op) {
            case 'h':
                printf("%s\n", USAGE_STR);
                exit(EXIT_SUCCESS);
            case 'v':
                args->verbose = true;
                break;
            case 's':
                args->s = atoi(optarg);
                break;
            case 'E':
                args->E = atoi(optarg);
                break;
            case 'b':
                args->b = atoi(optarg);
                break;
            case 't':
                args->ref_filename = optarg;
                break;
        }
    }
    if (args->ref_filename == NULL || args->b <= 0 || args->s <= 0
            || args->E <= 0)
        return 0;
    return 1;
}

/*
 * Prints usage information to the user.
 */
void report_failure(void)
{
    printf("./csim: missing required arguments\n");
    printf("%s\n", USAGE_STR);
    printf("Options:\n-h\t\tPrint this help message.\n");
    printf("-v\t\tOptional verbose flag.\n");
    printf("-s <num>\tNumber of set index bits.\n");
    printf("-E <num>\tNumber of lines per set.\n");
    printf("-b <num>\tNumber of block offset bits.\n");
    printf("-t <file>\tTrace file.\n");
}
