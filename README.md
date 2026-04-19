# Simulation of a railway system and its degradation over time using data structures
Mara Gheorghe, Politehnica University of Bucharest

Description: Each route between two cities consists of a number of connected sections and each section has a degree of wear and tear. Each year, all degrees greater than 0 double in value. Those with a 0 value take half the value of the highest degree of wear among its neighbouring sections.

Usage: Make sure you have a version of the GCC compiler installed on your Linux machine. Enter the "railway_sim" folder, write you input in the railway_sim.in file and run the "make" command. Then execute the resulting binary file: "./railway_sim".

Input: In the first line of the  railway_sim.in file, write the number of routes, the number of years passing and the maximum acceptable mean degree of wear and tear for a route. Next write each route on a separate line: the name of the two cities it connects, the number of sections and the degree of wear and tear on each section.

Output: The  railway_sim.out file contains the degree of wear on all sections in the railway system and the indexes of all the routes where the mean degree of wear are lower than the maximum acceptable degree of wear.

Example of input and output:
railway_sim.in
4
3
40
Munich Prague 2 0 0
Paris Berlin 3 10 15 0
Berlin Pamplona 3 0 0 0
Pamplona Lyon 4 0 20 40 0

railway_sim.out
Munich Prague 2 0.00 0.00
Paris Berlin 3 80.00 100.00 30.00
Berlin Pamplona 3 7.50 2.50 10.00
Pamplona Lyon 4 40.00 100.00 100.00 80.00
1 3

The following data structures were dinamically allocated:
 - adjacency lists to represent the undirected graph that holds all information about the system besides degrees of wear
 - array of pairs (name, index) for cities to easily access a city's name knowing its index
 - matrix of arrays to store the degrees of wear and tear for all sections

