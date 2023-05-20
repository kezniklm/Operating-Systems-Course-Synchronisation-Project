// proj2.c
// Riesenie IOS_project2
// Datum odovzdania: 2.5.2022
// Autor: Matej Keznikl
// Fakulta: Fakulta informačných technológií VUT v Brne (FIT VUT)
// Prelozene: GCC 7.5.0
// Testované na zariadeniach s operačnými systémami: Ubuntu 20.04, Debian 10,...
// Popis: Projekt 2 - riešenie synchronizačného problému - H2O - inšpirované knihou od Allen B. Downey, The Little Book of Semaphores

#include "proj2.h"

int OX_ID = 0;
int H_ID = 0;

void sleep_process(int start, int end)
{
    float sleep_time = (rand() % (end - start + 1)) + start;

    usleep(sleep_time * 1000);
}

shared_memory_t *init()
{
    shared_memory_t *shared = mmap(NULL, sizeof(shared_memory_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (shared == NULL)
    {
        return shared;
    }

    shared->fp = fopen(OUTPUT_FILENAME, "w");

    shared->line_count = 1;
    shared->oxygens = 0;
    shared->hydrogens = 0;
    shared->molecules = 1;

    if (sem_init(&(shared->sem_oxygen), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre Kyslik\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_hydrogen), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre Vodik\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_print), 1, 1) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre printovanie\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_molecule_ox), 1, 1) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre tvorenie molekuly\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_molecule_hy), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre tvorenie molekuly\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_end), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre tvorenie molekuly\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_molecule), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre tvorenie molekuly\n");
        return NULL;
    }

    if (sem_init(&(shared->sem_hydrogen_queued), 1, 0) == -1)
    {
        print(stderr, "Nebolo možné otvoriť semafor pre tvorenie molekuly\n");
        return NULL;
    }

    return shared;
}

void destroy(shared_memory_t *shared)
{
    sem_destroy(&(shared->sem_oxygen));
    sem_destroy(&(shared->sem_hydrogen));
    sem_destroy(&(shared->sem_print));
    sem_destroy(&(shared->sem_molecule_ox));
    sem_destroy(&(shared->sem_molecule_hy));
    sem_destroy(&(shared->sem_molecule));
    sem_destroy(&(shared->sem_end));
    sem_destroy(&shared->sem_hydrogen_queued);
}

void print(FILE *stream, const char *fmt, ...)
{
    va_list arguments;
    va_start(arguments, fmt);
    vfprintf(stream, fmt, arguments);
    fflush(stream);
    va_end(arguments);
}

void atom_print(char atom, int index, shared_memory_t *shared, int message)
{
    sem_wait(&(shared->sem_print));

    if (message == 0)
    {
        print(shared->fp, "%d: %c %d: started\n", shared->line_count++, atom, index);
    }

    if (message == 1)
    {
        print(shared->fp, "%d: %c %d: going to queue\n", shared->line_count++, atom, index);
    }

    if (message == 2)
    {
        print(shared->fp, "%d: %c %d: creating molecule %d\n", shared->line_count++, atom, index, shared->molecules);
    }

    if (message == 3)
    {
        print(shared->fp, "%d: %c %d: molecule %d created\n", shared->line_count++, 'O', shared->oxygen_atm, shared->molecules);
        print(shared->fp, "%d: %c %d: molecule %d created\n", shared->line_count++, 'H', shared->hydro_atm_1, shared->molecules);
        print(shared->fp, "%d: %c %d: molecule %d created\n", shared->line_count++, 'H', shared->hydro_atm_2, shared->molecules);
        shared->oxygen_atm = 0;
        shared->hydro_atm_1 = 0;
        shared->hydro_atm_2 = 0;
    }

    if (message == 4)
    {
        print(shared->fp, "%d: %c %d: not enough O or H\n", shared->line_count++, atom, index);
    }

    if (message == 5)
    {
        print(shared->fp, "%d: %c %d: not enough H\n", shared->line_count++, atom, index);
    }

    sem_post(&(shared->sem_print));
    return;
}

void oxygen(char atom, int index, shared_memory_t *shared, int molecule_max)
{
    atom_print(atom, index, shared, 0);
    sleep_process(0, shared->args->TI);
    atom_print(atom, index, shared, 1);
    sem_wait(&shared->sem_molecule_ox);
    shared->oxygen_atm = index;

    if (shared->molecules > molecule_max)
    {
        for (int i = molecule_max * 2; i < shared->args->NH; i++)
        {
            sem_post(&shared->sem_molecule_hy);
        }
        atom_print(atom, index, shared, 5);
        sem_post(&shared->sem_molecule_ox);
        exit(EXIT_SUCCESS);
    }
    sem_post(&shared->sem_molecule_hy);
    sem_post(&shared->sem_molecule_hy);

    sem_wait(&shared->sem_hydrogen_queued);
    sem_wait(&shared->sem_hydrogen_queued);

    sem_post(&shared->sem_molecule);
    sem_post(&shared->sem_molecule);

    sem_wait(&shared->sem_hydrogen);
    sem_wait(&shared->sem_hydrogen);

    atom_print(atom, index, shared, 2);

    sleep_process(0, shared->args->TB);

    atom_print(0, 0, shared, 3);
    shared->oxygen_atm = 0;
    shared->hydro_atm_1 = 0;
    shared->hydro_atm_2 = 0;

    shared->molecules++;
    if (shared->molecules > molecule_max)
    {
        sem_post(&shared->sem_end);
    }
    sem_post(&shared->sem_molecule_ox);
    return;
}

void hydrogen(char atom, int index, shared_memory_t *shared, int molecule_max)
{
    atom_print(atom, index, shared, 0);
    sleep_process(0, shared->args->TI);
    atom_print(atom, index, shared, 1);

    sem_wait(&shared->sem_molecule_hy);

    if (shared->molecules > molecule_max)
    {
        atom_print(atom, index, shared, 4);
        sem_post(&shared->sem_end);
        exit(EXIT_SUCCESS);
    }

    if (shared->hydro_atm_1 == 0)
    {
        shared->hydro_atm_1 = index;
    }
    else
    {
        shared->hydro_atm_2 = index;
    }

    sem_post(&shared->sem_hydrogen_queued);

    sem_wait(&shared->sem_molecule);

    atom_print(atom, index, shared, 2);
    sem_post(&shared->sem_hydrogen);
    return;
}

args_t *arguments_create(const char *oxygens_, const char *hydrogens_, const char *TI_, const char *TB_)
{
    args_t *args = (args_t *)malloc(sizeof(args_t));
    if (arguments_check(oxygens_, hydrogens_, TI_, TB_))
    {
        arguments_free(args);
        return NULL;
    }
    args->NO = atoi(oxygens_);
    args->NH = atoi(hydrogens_);
    args->TB = atoi(TB_);
    args->TI = atoi(TI_);

    return args;
}

bool arguments_check(const char *oxygens, const char *hydrogens, const char *TI, const char *TB)
{
    if (*oxygens == '\0')
    {
        print(stderr, "NO, NH, TI, TB musia byt cisla\n");
        return 1;
    }

    if (*hydrogens == '\0')
    {
        print(stderr, "NO, NH, TI, TB musia byt cisla\n");
        return 1;
    }

    if (*TI == '\0')
    {
        print(stderr, "NO, NH, TI, TB musia byt cisla\n");
        return 1;
    }

    if (*TB == '\0')
    {
        print(stderr, "NO, NH, TI, TB musia byt cisla\n");
        return 1;
    }

    char *NO_ptr = NULL;
    long NO = strtol(oxygens, &NO_ptr, 10);

    char *NH_ptr = NULL;
    long NH = strtol(hydrogens, &NH_ptr, 10);

    char *TI_ptr = NULL;
    long TI_n = strtol(TI, &TI_ptr, 10);

    char *TB_ptr = NULL;
    long TB_n = strtol(TB, &TB_ptr, 10);

    if ((*NO_ptr != '\0') || (*NH_ptr != '\0') || (*TI_ptr != '\0') || (*TB_ptr != '\0'))
    {
        print(stderr, "NO, NH, TI, TB musia byt cisla\n");
        return 1;
    }

    if (NO <= 0)
    {
        print(stderr, "NO musi byt vacsie ako 0\n");
        return 1;
    }

    if (NH <= 0)
    {
        print(stderr, "NH musi byt vacsie ako 0\n");
        return 1;
    }

    if (TI_n < 0 || TI_n > MAX_TI)
    {
        print(stderr, "TI musi byt vacsie ako 0 a zaroven mensie ako 1001\n");
        return 1;
    }

    if (TB_n < 0 || TB_n > MAX_TB)
    {
        print(stderr, "TB musi byt vacsie ako 0 a zaroven mensie ako 1001\n");
        return 1;
    }

    return 0;
}

void arguments_free(args_t *args)
{
    free(args);
}

int main(int argc, char **argv)
{
    shared_memory_t *shared = init();

    if (argc != NUM_ARGS)
    {
        print(stderr, "Pocet argumentov musi byt 4\n");
        exit(EXIT_FAILURE);
    }

    args_t *args = arguments_create(argv[1], argv[2], argv[3], argv[4]);
    if (args == NULL)
    {
        exit(EXIT_FAILURE);
    }

    shared->args = args;
    int molecule_max = ((args->NH / 2) < args->NO) ? (args->NH / 2) : args->NO;

    for (int i = 0; i < shared->args->NO; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            print(stderr, "Fork nebol uspesny\n");
            arguments_free(args);
            destroy(shared);
            exit(EXIT_FAILURE);
        }
        OX_ID++;
        if (pid == 0)
        {
            oxygen('O', OX_ID, shared, molecule_max);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < shared->args->NH; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            print(stderr, "Fork nebol uspesny\n");
            arguments_free(args);
            destroy(shared);
            exit(EXIT_FAILURE);
        }
        H_ID++;
        if (pid == 0)
        {
            hydrogen('H', H_ID, shared, molecule_max);
            exit(EXIT_SUCCESS);
        }
    }
    sem_wait(&shared->sem_end);

    if (args->NO * 2 < args->NH)
    {
        for (int i = molecule_max * 2; i < shared->args->NH; i++)
        {
            sem_post(&shared->sem_molecule_hy);
        }
    }

    while (wait(NULL) > 0)
        ;

    arguments_free(args);
    destroy(shared);

    exit(EXIT_SUCCESS);
}
