#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#define FILE_PATH_START_COUNT 4
#define MAX_THREADS_COUNT 100

typedef char Path[PATH_MAX];
Path *file_paths;
int file_paths_count = 0;
int file_paths_max_count = FILE_PATH_START_COUNT;

const char *word;

void get_files_paths(const char *directory);

void process_files(int n);

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Invalid arguments.\n");
        fprintf(stderr, "Command format: %s directory word n.\n", argv[0]);
        fprintf(stderr, "directory: start directory to check.\n");
        fprintf(stderr, "word: word to count.\n");
        fprintf(stderr, "n: max count of threads in one moment.\n");
        return EXIT_FAILURE;
    }

    char *endptr;
    long n = strtol(argv[3], &endptr, 10);

    if (endptr == argv[3] || *endptr != '\0' || n <= 0 || n > MAX_THREADS_COUNT) {
        fprintf(stderr, "Max threads count cannot be less or equal 0 and more than %i!\n", MAX_THREADS_COUNT);
        return EXIT_FAILURE;
    }

    word = argv[2];

    file_paths = calloc(FILE_PATH_START_COUNT, sizeof(Path));
    if (!file_paths) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    get_files_paths(argv[1]);
    process_files((int) n);
    free(file_paths);

    return 0;
}

void add_file_path(const char *file_path) {
    if (file_paths_count == file_paths_max_count) {
        file_paths_max_count *= 2;
        Path *temp = realloc(file_paths, file_paths_max_count * sizeof(Path));
        if (!temp) {
            perror("realloc");
            free(file_paths);
            exit(EXIT_FAILURE);
        }
        file_paths = temp;
    }
    strcpy(file_paths[file_paths_count++], file_path);
}

void get_files_paths(const char *directory) {
    DIR *current_dir = opendir(directory);
    if (!current_dir) {
        perror("opendir");
        return;
    }

    struct dirent *d;

    while ((d = readdir(current_dir))) {
        Path filepath;
        sprintf(filepath, "%s/%s", directory, d->d_name);

        if (d->d_type == DT_REG) {
            add_file_path(filepath);
        } else if (d->d_type == DT_DIR && strcmp(".", d->d_name) && strcmp("..", d->d_name)) {
            get_files_paths(filepath);
        }
    }

    if (closedir(current_dir)) {
        perror("closedir");
        return;
    }
}

void *thread_func(void *index);

void process_file(const char *file_path, const char *word);

sem_t arg_semaphore;
sem_t main_semaphore;

void process_files(const int n) {
    if (sem_init(&arg_semaphore, 0, 1)) {
        perror("sem_init");
        return;
    }

    if (sem_init(&main_semaphore, 0, n)) {
        perror("sem_init");
        sem_destroy(&arg_semaphore);
        return;
    }

    printf("%-16s | %11s | %11s | %s\n", "Thread ID", "Bytes count", "Words count", "Path");

    for (int i = 0; i < file_paths_count; ++i) {
        sem_wait(&main_semaphore);

        pthread_t thread;
        sem_wait(&arg_semaphore);
        if (pthread_create(&thread, NULL, thread_func, (void *) &i)) {
            perror("pthread_create");
            sem_post(&arg_semaphore);
            sem_post(&main_semaphore);
            continue;
        }
    }

    for (int i = 0; i < n; ++i) {
        sem_wait(&main_semaphore);
    }

    sem_destroy(&arg_semaphore);
    sem_destroy(&main_semaphore);
}

void *thread_func(void *index) {
    int i = *(int *) index;
    sem_post(&arg_semaphore);

    process_file(file_paths[i], word);

    sem_post(&main_semaphore);
}

void process_file(const char *file_path, const char *word) {
    int file = open(file_path, O_RDONLY);
    if (file == -1) {
        //printf("Error in [%s]\n", file_path);
        perror("open");
        return;
    }

    int wordLength = strlen(word);
    char c;

    int count = 0;
    int current = 0;
    char canBeWord = 1;
    long countBytes = 0;
    char isCorrentWord = 0;

    while (read(file, &c, 1) == 1) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (isCorrentWord) {
                count++;
            }
            current = 0;
            canBeWord = 1;
            isCorrentWord = 0;
        } else if (canBeWord) {
            if (isCorrentWord) {
                canBeWord = 0;
                isCorrentWord = 0;
            } else {
                if (word[current] == c) {
                    current++;

                    if (current == wordLength) {
                        isCorrentWord = 1;
                    }
                } else {
                    current = 0;
                    canBeWord = 0;
                    isCorrentWord = 0;
                }
            }
        }

        countBytes++;
    }
    printf("%-16lu | %11ld | %11d | %s\n",
           pthread_self(), countBytes, count, file_path);

    if (close(file) == -1) {
        perror("close");
        return;
    }
}
