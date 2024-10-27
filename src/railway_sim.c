#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "railway_sim.h"

int main(int argc, char *argv[]) {
    FILE *in, *out;
    in = fopen("railway_sim.in", "r");
    if (in == NULL) {
        printf ("ERROR when opening input file\n");
        return 0;
    }
    out = fopen("railway_sim.out", "w");
    if (out == NULL) {
        printf ("ERROR when opening output file\n");
        return 0;
    }

    graph *g = init_graph();
    float ***sections = init_section(), L;

    // the cities list needed to tie each read name to an index
    node *cities_list = init_cities_list();
    int R, K, i, j, x;
    char *name1, *name2;
    name1 = (char *)malloc(max_city_length * sizeof(char));
    name2 = (char *)malloc(max_city_length * sizeof(char));

    // read the number of routes, the number of years passing 
    // and the maximum acceptable mean degree of wear and tear for a route
    fscanf(in, "%d %d %f", &R, &K, &L);
    int i1, i2, route_index = 1, nr_sections, year;

    // read each pair of cities, update the cities list and the graph
    // read the degrees of wear and update the matrix of degrees
    for (i = 0; i < R; i++) {
        fscanf (in, "%s %s", name1, name2);
        i1 = get_city_index(cities_list, name1, g->nr_of_cities);
        i2 = get_city_index(cities_list, name2, g->nr_of_cities);

        if (i1 == -1) {
            cities_list = add_city_name_index(cities_list, name1, g->nr_of_cities);
            i1 = g->nr_of_cities;
            g->nr_of_cities++;
        }

        if (i2 == -1) {
            cities_list = add_city_name_index(cities_list, name2, g->nr_of_cities);
            i2 = g->nr_of_cities;
            g->nr_of_cities++;
        }

        g = add_route(out, g, cities_list[i1], cities_list[i2], route_index);
        route_index++;

        sections = read_sections(in, sections, i1, i2);
    }

    // simulate the degradation process
    for (year = 1; year <= K; year++) {
        sections = degradation(out, g, sections, cities_list);
    }

    print_railway(out, g, sections, cities_list, route_index - 1);

    // find and print the index of the routes whose
    // mean degradation value is below the given limit
    int route, newline_cnt = 0;
    for (route = 1; route <= route_index - 1; route++)
        if (find_degradation_of_route(g, sections, route) < L) {
            if (!newline_cnt) {
                fprintf(out, "\n%d", route);
                newline_cnt = 1;
            }
            else 
                fprintf(out, " %d\n", route);
        }

    // freeing up the used memory
    free_cities_list(cities_list);
    free_section(sections);
    free_graph(g);
    free(name1);
    free(name2);

    fclose(in);
    fclose(out);
    return 0;
}
