#include <stdio.h>
#include <limits.h>

#define MAX_NODES 100
#define MAX_DIST 999999

int cost_matrix[MAX_NODES][MAX_NODES];

int next_matrix[MAX_NODES][MAX_NODES];

void init(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
                cost_matrix[i][j] = 0;
            else
                cost_matrix[i][j] = MAX_DIST;
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            next_matrix[i][j] = j;
        }
}

void add_edge(int source, int dest, int weight)
{
    cost_matrix[source][dest] = weight;
}

void bellman_ford(int n)
{
    int i, j, k;

    // Step 1: Relax all edges |V| - 1 times. A simple shortest path from src to any other vertex can have at most |V| - 1 edges
    for (int round = 0; round < (n - 1); round++)
    {
        for (int i = 0; i < n; i++) // select source
        {
            for (int j = 0; j < n; j++)
            {
                for (int k = 0; k < n; k++)
                {

                    if (cost_matrix[i][j] > cost_matrix[i][k] + cost_matrix[k][j])
                    {
                        cost_matrix[i][j] = cost_matrix[i][k] + cost_matrix[k][j];
                        next_matrix[i][j] = next_matrix[i][k];
                    }
                }
            }
        }
    }
}

void print_matrix(int n)
{
    printf(" | ");
    for (int i = 0; i < n; i++)
        printf("%d ", i);

    printf("\n");
    for (int i = 0; i < n + 3; i++)
        printf("- ");

    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%d| ", i);
        for (int j = 0; j < n; j++)
        {
            // printf("(%d,%d) ", cost_matrix[i][j] == MAX_DIST ? -1 : cost_matrix[i][j], next_matrix[i][j]);
            printf("%d ", cost_matrix[i][j] == MAX_DIST ? -1 : cost_matrix[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int n, m, source, dest, weight, src;
    printf("Enter the number of nodes: ");
    scanf("%d", &n);

    printf("Enter the number of edges: ");
    scanf("%d", &m);

    init(n);

    for (int i = 0; i < m; i++)
    {
        printf("Enter edge %d (source, destination, weight): ", i + 1);
        scanf("%d %d %d", &source, &dest, &weight);
        add_edge(source, dest, weight);
        add_edge(dest, source, weight);
    }

    bellman_ford(n);

    print_matrix(n);

    return 0;
}
