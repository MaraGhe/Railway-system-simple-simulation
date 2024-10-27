#ifndef RAILWAY_SIM_H
#define RAILWAY_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max_city_length 50
#define max_cities 100
#define max_section 30
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

typedef struct node_struct
{
    int index;
    char *name;
}*node;

typedef struct list_struct
{
    // the index of the route that starts from the i node (adj_list[i])
    int route_index;
    node city;
    struct list_struct *next, *prev;
}*list;

typedef struct graph_struct
{
	int nr_of_cities;
	list *adj_list;
}graph;


// initialises the undirected graph that represents the railway system
graph* init_graph() {
    graph *g = (graph *)malloc(sizeof(graph));
    g->nr_of_cities = 0;
    g->adj_list = (list *)malloc(max_cities * sizeof(list));
    int i = 0;
    for (i = 0; i < max_cities; i++) {
        g->adj_list[i] = (list)malloc(sizeof(struct list_struct));
        g->adj_list[i]->next = NULL;
        g->adj_list[i]->prev = NULL;
        g->adj_list[i]->city = (node)malloc(sizeof(struct node_struct));
        g->adj_list[i]->city->name = (char *)malloc(max_city_length * sizeof(char));
    }
    return g;
}

/*
    - allocates memory for an array matrix where an array at the coordinates (i, j) represents
    the degrees of wear on each section of the route between the i and j nodes.
    - the first element of the array stores the number of sections on that route.
    - returns the matrix
*/
float ***init_section() {
    float ***dw = (float ***)malloc(max_cities * sizeof(float **));
    int i, j;
    for (i = 0; i < max_cities; i++) {
        // sections on routes which contain the i node
        dw[i] = (float **)malloc(max_cities * sizeof(float *));
        for (j = 0; j < max_cities; j++)
            // sections on the i <-> j route
            dw[i][j] = (float *)malloc(max_section * sizeof(float));
    }
    return dw;
}

// initialises and returns the cities list that ties each name to an index
node *init_cities_list() {
    node *cities_list = (node*)malloc(max_cities * sizeof(node));
    int  i = 0;
    for (i = 0; i < max_cities; i++) {
        cities_list[i] = (node)malloc(sizeof(struct node_struct));
        cities_list[i]->name = (char *)malloc(max_city_length * sizeof(char));
    }
    return cities_list;
}

// returns 1 if there is an existing route between cities c1 and c2, 0 otherwise
int exists_route(graph *g, node c1, node c2) {
    int i = c1->index;
    list nod = g->adj_list[i];
    while (nod != NULL) {
        if (nod->city == c2)
            return 1;
        nod = nod->next;
    }
    return 0;
}

// returns 1 if city "c" has been added to the railway system yet, 0 otherwise
int exists_city(graph *g, node c) {
    int i = c->index;
    if (g->adj_list[i]->next != NULL)
        return 1;
    return 0;
}

/*
    - adds city "c" to the adjacency list of another city
    - writes the route's index
    - returns the updated list
*/
list add_city(FILE *out, list l, node c, int route_index) {
    list aux = l, previous = NULL;
    list nod = (list)malloc(sizeof(struct list_struct));
    nod->city = (node)malloc(sizeof(struct node_struct));
    nod->city = c;
    nod->next = NULL;
    while (l != NULL) {
        previous = l;
        l = l->next;
    }
    l = nod;
    nod->prev = previous;
    previous->next = l;
    nod->route_index = route_index;
    return aux;
}

// - removes city "c" from the adjacency list of another city
// - returns the updated list
list remove_city(list l, node c) {
    if(l == NULL) {
		return NULL;
	} else {
		list aux = l, prev;
		if(aux->city == c) {
			l = l->next;
			free(aux);
			l->prev = NULL;
			return l;
		} else {
			prev = aux;
			aux = aux->next;
		}
		while(aux != NULL) {
			if(aux->city == c) {
				prev->next = aux->next;
				if (aux->next != NULL)
					aux->next->prev = prev;
				free(aux);
				return l;
			}
			prev = aux;
			aux = aux->next;
		}
		return l;
	}
}

// - adds a route between cities c1 and c2
// - returns the updated graph
graph* add_route(FILE *out, graph *g, node c1, node c2, int route_index) {
    int i1, i2;
    list nod;
    if (!exists_route(g, c1, c2)) {
        i1 = c1->index;
        g->adj_list[i1] = add_city(out, g->adj_list[i1], c2, route_index);
        i2 = c2->index;
        g->adj_list[i2] = add_city(out, g->adj_list[i2], c1, 0);
    }
    return g;
}

// - removes the routes between cities c1 and c2
// - returns the updated graph
graph* remove_route(graph *g, node c1, node c2) {
    if (exists_route(g, c1, c2)) {
        int i1 = c1->index, i2 = c2->index;
        g->adj_list[i1] = remove_city(g->adj_list[i1], c2);
        g->adj_list[i2] = remove_city(g->adj_list[i2], c1);
    }
    return g;
}

/*
    - reads the number of sections between cities c1 and c2 and the degree of wear for each one of them
    - dw[i][j][0] stores the number of sections on the i <-> j route
    - returns the updated matrix of arrays for the degrees of wear and tear
*/
float ***read_sections(FILE *in, float ***dw, int i1, int i2) {
    float nr_sections, value;
    int i, len;
    fscanf(in, "%f", &nr_sections);
    len = (int)nr_sections;

    // the first eleme nt of the array is the number of sections
    dw[i1][i2][0] = nr_sections;
    dw[i2][i1][0] = nr_sections;
    for (i = 1; i <= nr_sections; i++) {
        fscanf(in, "%f", &value);
        dw[i1][i2][i] = value;
        dw[i2][i1][len - i + 1] = value;
    }
    return dw;
}

// deletes the cities list
void free_cities_list(node *cities_list) {
    int i = 0;
    for (i = 0; i < max_cities; i++) {
        free(cities_list[i]->name);
        free(cities_list[i]);
    }
    free (cities_list);
}

// deletes the matrix that stores the degrees of wear
void free_section(float ***dw) {
    int i, j;
    for (i = 0; i < max_cities; i++) {
        for (j = 0; j < max_cities; j++)
            free(dw[i][j]);
        free(dw[i]);
    }
    free (dw);
}

// deletes the railway system
void free_graph(graph *g) {
    int i = 0;
    for (i = 0; i < max_cities; i++) {
        free(g->adj_list[i]->city->name);
        free(g->adj_list[i]->city);
        free(g->adj_list[i]);        
    }
    free(g->adj_list);
    free(g);
}

// - computes the degradation of the railway system afetr a year
// - returns the upgraded metrix of degrees of wear
float ***degradation(FILE *out, graph *g, float ***dw, node *cities_list) {
    int i, j, k, idx, len, left, right;
    float value, ***degraded;
    list nod;

    // the updated matrix
    degraded = init_section();
    
    for (i = 0; i < g->nr_of_cities; i++) {
        for (j = i+1; j < g->nr_of_cities; j++) {
            degraded[i][j][0] = dw[i][j][0];
            degraded[j][i][0] = degraded[i][j][0];
            len = (int)dw[i][j][0];
            if (exists_route(g, cities_list[i], cities_list[j])) {
                for (k = 1; k <= len; k++) {
                    value = 0;
                    if (dw[i][j][k] != 0)
                        value = dw[i][j][k] * 2;
                    else {
                        if (k == 1) {
                            for (left = 0; left < g->nr_of_cities; left++)
                                if (left != i && left != j && exists_route(g, cities_list[i], cities_list[left]))
                                    value = max(value, dw[i][left][1]);
                        }
                        else value = max(value, dw[i][j][k-1]);

                        if (k == len) {
                            for (right = 0; right < g->nr_of_cities; right++)
                                if (right != i && right != j && exists_route(g, cities_list[j], cities_list[right]))
                                        value = max(value, dw[j][right][1]);
                        }
                        else value = max(value, dw[i][j][k+1]);

                        value = min(value / (float)2, 100);
                    }
                    degraded[i][j][k] = min(value, 100);
                    degraded[j][i][len - k + 1] = degraded[i][j][k];
                }
            }
        }
    }

    free_section(dw);
    return degraded;
}

// prints the indexed routes between cities
void print_graph(FILE  *out, graph *g, node *cities_list) {
	list nod;
	int i;
	for (i = 0; i < g->nr_of_cities; i++) {
		fprintf(out, "Node (%d, %s): \n", i, cities_list[i]->name);
		nod = g->adj_list[i];
		while (nod != NULL) {
			fprintf(out, "    route with the index %d towards (%d, %s)\n", nod->route_index, nod->city->index, nod->city->name);
			nod = nod->next;
		}
	}
}

// prints of degrees of wear on each route
void print_section(FILE *out, float ***dw, int cities) {
    int i, j, k, len;
    for (i = 0; i < cities; i++) {
        for (j = 0; j < cities; j++)
            if (dw[i][j][0]) {
                len = dw[i][j][0];
                fprintf(out, "On the route from %d to %d (length = %d) the sections are: ", i, j, (int)len);
                for (k = 1; k <= len; k++)
                    fprintf(out, "%.2f ", dw[i][j][k]);
                fprintf(out, "\n");
            }
        fprintf(out, "\n");
    }
}

// prints the railway system
void print_railway(FILE *out, graph *g, float ***t, node *cities_list, int routes) {
    int i = 0, j = 0, k = 1, len = 0, r = 1, found = 0;
    list nod = g->adj_list[0];
    for (r = 1; r <= routes; r++) {
        found = 0;
        for (i = 0; i < g->nr_of_cities && !found; i++) {
            nod = g->adj_list[i];
            while(nod && !found) {
                if (nod->route_index == r) {
                    found = 1;
                    j = nod->city->index;
                    len = (int)t[i][j][0];
                    fprintf(out, "%s %s %d", cities_list[i]->name, nod->city->name, len);
                    for (k = 1; k <= len; k++)
                        fprintf(out, " %.2f", t[i][j][k]);

                    if (r != routes)   // do not print \n when it's the last route
                        fprintf(out, "\n");
                }
                nod = nod->next;
            }
        }
    }
}

// returns the index of the route between the city with the adjacency 
// list "l" and another city "c", or (-1) if the route doesn't exist
int find_route_index (list l, node c) {
    if (!l)
        return -1;
    
    while (l) {
        if (l->city == c)
            return l->route_index;
        l = l->next;
    }

    return -1;
}

// calculates the mean degradation value of a route
float find_degradation_of_route(graph *g, float ***dw, int route) {
    int i = 0, j = 0, k = 0, len = 0;
    float s = 0;
    list nod;
    // finds what nodes form the route
    for (i = 0; i < g->nr_of_cities; i++) {
        nod = g->adj_list[i];
        while (nod) {
            if (nod->route_index == route) {
                len = (int)dw[i][nod->city->index][0];
                for (k = 1; k <= len; k++)
                    s += dw[i][nod->city->index][k];
                s /= (float)len;
                return s;
            }
            nod = nod->next;
        }
    }
    return s;
}

// - adds the city named city_name to the list and assigns an index to it
// - returns the new cities list
node *add_city_name_index (node *cities_list, char *city_name, int memorized_cities) {
    cities_list[memorized_cities] = (node)malloc(sizeof(struct node_struct));
    cities_list[memorized_cities]->name = (char *)malloc((strlen(city_name)+1) * sizeof (char));
    strcpy(cities_list[memorized_cities]->name, city_name);
    cities_list[memorized_cities]->index = memorized_cities;
    return cities_list;
}

// returns the index of a city or (-1) if the city is not in the list
int get_city_index (node *cities_list, char *city_name, int memorized_cities) {
    int i = 0;
    for (i = 0; i < memorized_cities; i++)
        if (strcmp(cities_list[i]->name, city_name) == 0)
            return i;
    return -1;
}

#endif
