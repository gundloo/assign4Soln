/*
 * Assignment submitted by (name of student):
 * on (date):
 * assign4DijkstraContactTracingTester.c written by M. Mohiuddin. It randomly generates the contact tracing database for
 * a population and then uses Dijkstra algorithm to find the contact history of the population and can find the contact
 * trace of any pair of persons.
 * The project has following other files:
 * 1- contactTracing.c
 * 2- contactTracing.h
 * 3- dijkstra.in is the data file that optionally holds the graph
 * Revised on Nov 14, 2021.
 * The revision corrected the algorithm to include reverse direct contact between source and destination and merging of
 * multiple header and implementation files into one header and one implementation file.
 * Revised further on Nov 20, 2021 to add a function isADirectContact to check direct contact between source and
 * destination in either direction.
 * Revised on Nov 22, 2021 to
 * 1- completely remove the need for an array of indices!
 * 2- include isADirectContact functionality within DijkstraContactTracing function.
 * THIS CLIENT APPLICATION MUST NOT BE CHANGED except writing of name of the student above
 */
#include <stdio.h>
#include <stdlib.h>
#include "contactTracing.h"
int main() {
    puts("-----------------------This application helps in contact tracing to monitor Covid-19 spread.-----------------------"
         "\n-----It displays contact database of the population and determines if any two persons have been in contact.-----");
    int num = -2;
    while ( num != 0) {
        puts("Select the population dataset");
        puts("Enter 1 for Durham region");
        puts("Enter 2 for Peel region");
        puts("Enter 3 for York region");
        puts("Enter 4 or higher integer for Greater Toronto Area");
        int region = 4;
        scanf("%d", &region);
        printf("Enter total number of people to be traced/tracked (it must be an integer between 2 to %d) or enter '0' to end\n", NUM_VERTICES);
        scanf("%d", &num);
        if ( num == 0) {
            puts("Thanks");
            break;
        }
        if (num < 2 || num > NUM_VERTICES) {
            puts("Invalid value. Try again.");
            continue;
        }
        Graph G = newGraph(num);
        buildGraphRandom(G);
        if ( num <= 5000) {
            FILE *in = fopen("..//dijkstra.in", "w");
            writeGraphToFile(in, G);
            fclose(in);
            printGraph(G, region);
        }
        int src = 0;
        int flag = 0;
        while (1) {  // loop to continue contact tracing of one population
            src = 0;
            puts("---------Query for contact tracing between two individuals:---------");
            while (src < 1 || src > num) { // loop to obtain valid source ID
                printf("Enter source person's ID (1 to %d)\n", num);
                scanf("%d", &src);
                if (src < 1 || src > num) {
                    puts("Invalid data, try again");
                }
            } // end of while loop for getting source person ID
            int target = 0;
            while (target == src || target < 1 || target > num) { // loop to obtain valid target ID
                printf("Enter target person's ID (1 to %d excluding %d) or 0 to change the population data set\n",
                       num, src);
                scanf("%d", &target);
                if (target == 0) {
                    flag = 1;
                    break;
                }
                if (target == src || target < 1 || target > num) {
                    puts("Invalid data, try again");
                }
            } // end of while loop to obtain valid target ID
            if (flag == 1) {
                flag = 0;
                continue;
            }
            int count = DijkstraContactTracing(G, src, target);
                if (count == 0) {
                    printf("person ID %d and person ID %d have been in direct contact\n", src, target);
                }
                else if (count == INT_MAX - 1) {
                    printf("Target person ID %d has not been in direct or indirect contact with the source person ID %d\n",
                           target, src);
                }
                else {
                    printf("Contact trace from person ID %d to person ID %d \n", src, target);
                    followPath(G, target);
                }
            int options = 1;
            puts("\nNext query:");
            puts("Enter 1 to check for another pair of persons or 2 to change the population data set or 0 to exit");
            scanf("%d", &options);
            if(options == 0) {
                puts("Thank you");
                exit(0);
            }
            else if (options == 1){
                continue;
            }
            else
                break;
        } // while(1) loop to work on one population dataset for contact tracing ends here
    } // outer while loop ends
    return 0;
} // end main