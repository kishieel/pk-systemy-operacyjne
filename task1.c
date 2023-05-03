#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 4

int main()
{
    int A[N][N] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
    int sum = 0;

    for (int i = 0; i < N; i++)
    {
        int pid = fork();
        if (pid == 0) {
            int partial = 0;
            for (int j = 0; j < N; j++) partial += A[i][j];

            printf("Proces %d: Suma elementów %d wiersza: %d\n", getpid(), i + 1, partial);
            exit(partial);  
        } else if (pid == -1) {
            perror("Błąd!");
            exit(-1);
        }
    }

    for (int i = 0; i < N; i++)
    {
        int status;
        wait(&status);
        sum += WEXITSTATUS(status);
    }

    printf("Suma elementów tablicy: %d\n", sum);
    return 0;
}