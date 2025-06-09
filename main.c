#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sudoku.h"

#define N 9
#define POP_SIZE 500
#define NUM_GENERATIONS 10000
#define MUTATION_RATE 0.2
#define CROSSOVER_RATE 0.7

typedef struct {
    int grid[N][N];
    int fitness;
} Individual;

int puzzle[MAX_N][MAX_N], solution[MAX_N][MAX_N], fixed[MAX_N][MAX_N];

void generate_fixed() {
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            fixed[i][j] = puzzle[i][j] ? 1 : 0;
}

void init_individual(Individual *ind) {
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            ind->grid[i][j]=puzzle[i][j];
    for(int block=0; block<N; block++) {
        int br=(block/3)*3, bc=(block%3)*3;
        int used[10]={0}, vals[9], cnt=0;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++)
            if(ind->grid[br+i][bc+j]) used[ind->grid[br+i][bc+j]]=1;
        for(int v=1;v<=9;v++) if(!used[v]) vals[cnt++]=v;
        for(int i=cnt-1;i>0;i--) {
            int j=rand()%(i+1);
            int t=vals[i]; vals[i]=vals[j]; vals[j]=t;
        }
        int idx=0;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++)
            if(!ind->grid[br+i][bc+j]) ind->grid[br+i][bc+j]=vals[idx++];
    }
}

int calculate_fitness(Individual *ind) {
    int conf = 0;
    for(int r=0;r<N;r++) {
        int count[10]={0};
        for(int c=0;c<N;c++) count[ind->grid[r][c]]++;
        for(int v=1;v<=9;v++) if(count[v]>1) conf+=count[v]-1;
    }
    for(int c=0;c<N;c++) {
        int count[10]={0};
        for(int r=0;r<N;r++) count[ind->grid[r][c]]++;
        for(int v=1;v<=9;v++) if(count[v]>1) conf+=count[v]-1;
    }
    ind->fitness=conf;
    return conf;
}

void initializePopulation(Individual pop[POP_SIZE]) {
    for (int i=0; i<POP_SIZE; i++) {
        init_individual(&pop[i]);
        calculate_fitness(&pop[i]);
    }
}

Individual selectParent(Individual pop[POP_SIZE]) {
    int best = rand()%POP_SIZE;
    for(int i=0;i<3;i++) {
        int n=rand()%POP_SIZE;
        if(pop[n].fitness<pop[best].fitness) best=n;
    }
    return pop[best];
}

void crossover(Individual p1, Individual p2, Individual *child) {
    for(int block=0;block<N;block++) {
        int take_from = rand()%2;
        int br=(block/3)*3, bc=(block%3)*3;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++) {
            int r=br+i, c=bc+j;
            if (fixed[r][c]) child->grid[r][c]=puzzle[r][c];
            else child->grid[r][c]=(take_from?p1.grid[r][c]:p2.grid[r][c]);
        }
    }
}

void mutate(Individual *ind) {
    int block=rand()%N, br=(block/3)*3, bc=(block%3)*3;
    int e[9][2], cnt=0;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++)
        if(!fixed[br+i][bc+j]){e[cnt][0]=br+i;e[cnt][1]=bc+j;cnt++;}
    if(cnt>=2) {
        int i1=rand()%cnt, i2=rand()%cnt;
        int tmp = ind->grid[e[i1][0]][e[i1][1]];
        ind->grid[e[i1][0]][e[i1][1]] = ind->grid[e[i2][0]][e[i2][1]];
        ind->grid[e[i2][0]][e[i2][1]] = tmp;
    }
}

void print_grid(int grid[N][N]) {
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            printf("%d ",grid[i][j]);
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    int hints = 30;
    generateSudoku(puzzle, solution, hints, N);

    printf("Wygenerowana plansza Sudoku:\n");
    int tmp_puz[N][N], tmp_sol[N][N];
    for(int i=0;i<N;i++) for(int j=0;j<N;j++) {
        tmp_puz[i][j] = puzzle[i][j];
        tmp_sol[i][j] = solution[i][j];
    }
    print_grid(tmp_puz);
    printf("\nRozwiazanie Sudoku:\n");
    print_grid(tmp_sol);

    generate_fixed();

    Individual population[POP_SIZE], next_population[POP_SIZE];
    initializePopulation(population);

    Individual best;
    best.fitness=10000;
    int rozwiazanie_znalezione = 0;

    for(int generation=0; generation<NUM_GENERATIONS; generation++) {
        Individual current_best = population[0];
        for (int i=1;i<POP_SIZE;i++)
            if (population[i].fitness < current_best.fitness)
                current_best = population[i];
        if(current_best.fitness < best.fitness) {
            best = current_best;
            printf("Gen %d: found fitness=%d\n", generation, best.fitness);
            if (best.fitness==0) {
                printf("\nROZWIAZANIE SUDOKU ZNALEZIONE W GENERACJI %d:\n", generation);
                print_grid(best.grid);
                printf("Fitness=%d\n", best.fitness);
                rozwiazanie_znalezione = 1;
                break;
            }
        }
        next_population[0]=best;
        int idx=1;
        while(idx<POP_SIZE) {
            Individual p1 = selectParent(population);
            Individual p2 = selectParent(population);
            Individual c1, c2;
            if(((double)rand()/RAND_MAX)<CROSSOVER_RATE) {
                crossover(p1, p2, &c1);
                crossover(p2, p1, &c2);
            } else {
                c1 = p1; c2 = p2;
            }
            if(((double)rand()/RAND_MAX)<MUTATION_RATE) mutate(&c1);
            if(((double)rand()/RAND_MAX)<MUTATION_RATE) mutate(&c2);
            calculate_fitness(&c1);
            calculate_fitness(&c2);
            next_population[idx++]=c1;
            if(idx<POP_SIZE) next_population[idx++]=c2;
        }
        for(int i=0;i<POP_SIZE;i++) population[i]=next_population[i];
    }

    if(!rozwiazanie_znalezione) {
        printf("\nNIE ZNALEZIONO IDEALNEGO ROZWIAZANIA.\nNajlepszy znaleziony wynik:\n");
        print_grid(best.grid);
        printf("Fitness=%d\n", best.fitness);
    }

    printf("Nacisnij ENTER aby zakonczyc...");
    getchar();
    getchar();
    return 0;
}