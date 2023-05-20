// proj2.h
// Riesenie IOS_project2
// Datum odovzdania: 2.5.2022
// Autor: Matej Keznikl
// Fakulta: Fakulta informačných technológií VUT v Brne (FIT VUT)
// Prelozene: GCC 7.5.0
// Testované na zariadeniach s operačnými systémami: Ubuntu 20.04, Debian 10,...
// Popis: Hlavičkový súbor pre proj.c

#ifndef PROJ2_H
#define PROJ2_H

// Standartné kniznice
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

// Kniznice pre zdielanu pamat a semafory
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#define OUTPUT_FILENAME "proj2.out"

#define MAX_TI 1000
#define MAX_TB 1000

#define NUM_ARGS 5

// Struktura pre argumenty
typedef struct
{
    int NO;
    int NH;
    int TI;
    int TB;
} args_t;

// Struktura pre zdielanu pamat
typedef struct
{
    FILE *fp;
    args_t *args;

    int line_count;
    int oxygens;
    int hydrogens;
    int molecules;

    int oxygen_atm;
    int hydro_atm_1;
    int hydro_atm_2;

    sem_t sem_print;
    sem_t sem_oxygen;
    sem_t sem_hydrogen;
    sem_t sem_molecule_ox;
    sem_t sem_molecule_hy;
    sem_t sem_molecule;
    sem_t sem_hydrogen_queued;
    sem_t sem_end;

} shared_memory_t;

/*      Prototypy funkcii       */

shared_memory_t *init();
void destroy(shared_memory_t *shared);

void sleep_process(int start, int end);

void print(FILE *stream, const char *fmt, ...);
void atom_print(char atom, int index, shared_memory_t *shared, int message);

void oxygen(char atom, int index, shared_memory_t *shared, int molecule_max);
void hydrogen(char atom, int index, shared_memory_t *shared, int molecule_max);

args_t *arguments_create(const char *oxygens_, const char *hydrogens_, const char *TI_, const char *TB_);
bool arguments_check(const char *oxygens, const char *hydrogens, const char *TI, const char *TB);
void arguments_free(args_t *args);

#endif
