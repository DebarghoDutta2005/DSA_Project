#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants ---
#define MAX_AIRPORTS 20
#define MAX_NAME_LEN 50
#define INF 99999

// --- Global Variables ---
int graph[MAX_AIRPORTS][MAX_AIRPORTS];
char airports[MAX_AIRPORTS][MAX_NAME_LEN];
int airportCount = 0;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int getInt(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        int result = scanf("%d", &value);

        if (result == EOF) {
            printf("\nEnd of input detected. Exiting.\n");
            exit(0); 
        }

        clearInputBuffer(); 

        if (result != 1) {
            printf("Invalid input. Please enter a number.\n");
        } else if (value < min || value > max) {
            printf("Input out of range. Must be between %d and %d.\n", min, max);
        } else {
            return value; 
        }
    }
}


void getString(const char* prompt, char* buffer, int size) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, size, stdin) == NULL) {
            printf("\nEnd of input detected. Exiting.\n");
            exit(0); 
        }

        
        buffer[strcspn(buffer, "\n")] = 0;

        if (buffer[0] == '\0') {
            printf("Input cannot be empty. Please try again.\n");
        } else {
            return; 
        }
    }
}

// --- Core Graph Functions ---

void initGraph() {
    for (int i = 0; i < airportCount; i++) {
        for (int j = 0; j < airportCount; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }
}

void addFlight(int src, int dest, int cost, int twoWay) {
    graph[src][dest] = cost;
    if (twoWay) {
        graph[dest][src] = cost;
    }
}

void displayGraph() {
    printf("\nAvailable Flight Routes:\n");
    int found = 0;
    for (int i = 0; i < airportCount; i++) {
        for (int j = 0; j < airportCount; j++) {
            if (graph[i][j] != INF && i != j) {
                printf("  %s -> %s : %d km\n", airports[i], airports[j], graph[i][j]);
                found = 1;
            }
        }
    }
    if (!found) {
        printf("  No flights have been added yet.\n");
    }
}

int getAirportIndex(const char* name) {
    for (int i = 0; i < airportCount; i++) {
        if (strcmp(airports[i], name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

// --- Dijkstra's Algorithm ---

void dijkstra(int src, int dest) {
    int dist[MAX_AIRPORTS];
    int visited[MAX_AIRPORTS];
    int parent[MAX_AIRPORTS];

    for (int i = 0; i < airportCount; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < airportCount - 1; count++) {
        int min = INF, u = -1;

        for (int i = 0; i < airportCount; i++) {
            if (!visited[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }

        if (u == -1) break; // No path or all reachable nodes visited
        visited[u] = 1;

        for (int v = 0; v < airportCount; v++) {
            if (!visited[v] && graph[u][v] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    if (dist[dest] == INF) {
        printf("No route exists from %s to %s\n", airports[src], airports[dest]);
        return;
    }

    printf("\nShortest Route from %s to %s: %d km\n", airports[src], airports[dest], dist[dest]);
    printf("  Path: ");

    int path[MAX_AIRPORTS];
    int len = 0;
    for (int v = dest; v != -1; v = parent[v]) {
        path[len++] = v;
    }

    for (int i = len - 1; i >= 0; i--) {
        printf("%s", airports[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

// --- Main Program Logic ---


int showMenu() {
    printf("\nMenu:\n"
           "1. Add Flight\n"
           "2. Display All Flights\n"
           "3. Find Shortest Route\n"
           "4. Exit\n");
    return getInt("Enter choice (1-4): ", 1, 4);
}


void findRoute() {
    char src[MAX_NAME_LEN], dest[MAX_NAME_LEN];
    int s, d;

    getString("Enter source airport: ", src, MAX_NAME_LEN);
    s = getAirportIndex(src);
    if (s == -1) {
        printf("Airport '%s' not found.\n", src);
        return;
    }

    getString("Enter destination airport: ", dest, MAX_NAME_LEN);
    d = getAirportIndex(dest);
    if (d == -1) {
        printf("Airport '%s' not found.\n", dest);
        return;
    }

    dijkstra(s, d);
}


void addNewFlight() {
    char src[MAX_NAME_LEN], dest[MAX_NAME_LEN];
    int cost, twoWay, s, d;

    getString("Enter source airport: ", src, MAX_NAME_LEN);
    s = getAirportIndex(src);
    if (s == -1) {
        printf("Airport '%s' not found.\n", src);
        return;
    }

    getString("Enter destination airport: ", dest, MAX_NAME_LEN);
    d = getAirportIndex(dest);
    if (d == -1) {
        printf("Airport '%s' not found.\n", dest);
        return;
    }

    if (s == d) {
        printf("Source and destination cannot be the same.\n");
        return;
    }

    cost = getInt("Enter distance (km): ", 1, INF);
    twoWay = getInt("Is this route two-way? (1=Yes, 0=No): ", 0, 1);

    addFlight(s, d, cost, twoWay);
    printf("Flight added successfully!\n");
}

int main() {
    printf("Flight Route Planner\n");

    char prompt[100];
    sprintf(prompt, "Enter number of airports (1-%d): ", MAX_AIRPORTS);
    airportCount = getInt(prompt, 1, MAX_AIRPORTS);

    initGraph();

    printf("--- Enter Airport Names ---\n");
    for (int i = 0; i < airportCount; i++) {
        char namePrompt[100];
        sprintf(namePrompt, "Airport %d name: ", i + 1);
        getString(namePrompt, airports[i], MAX_NAME_LEN);
    }
    printf("--- Airport setup complete ---\n");


    while (1) {
        int choice = showMenu();

        switch (choice) {
            case 1:
                addNewFlight();
                break;
            case 2:
                displayGraph();
                break;
            case 3:
                findRoute();
                break;
            case 4:
                printf("Exiting Flight Planner. Goodbye!\n");
                return 0; 
        }
    }
    
    return 0;
}