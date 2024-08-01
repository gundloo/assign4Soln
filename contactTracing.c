/* Created by Muhammad Mohiuddin on 3/23/2021 and revised on 11/11/21. Several functions related to Dijkstra algorithm
 are taken from Program P7.3 from Kalicharan and their full implementation is given below and these MUST NOT BE CHANGED!
For rest of the functions follow the TO DO list.
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "contactTracing.h"
GEdgePtr newGEdge(int, int);
void initSingleSource(Graph G, int s) {
    for (int i = 1; i <= G->numV; i++) {
        G -> vertex[i].cost = INT_MAX;
        G -> vertex[i].parent = 0;
    }
    G -> vertex[s].cost = 0;
} //end initSingleSource
void followPath(Graph G, int c) {
    if (c != 0) {
        followPath(G, G -> vertex[c].parent);
        if (G -> vertex[c].parent != 0) printf(" -> "); //do not print -> for source
        printf("%d ", G -> vertex[c].id);
    }
} //end followPath
void siftUp(Graph G, int heap[], int n, int heapLoc[]) {
    //sifts up heap[n] so that heap[1..n] contains a heap based on cost
    int siftItem = heap[n];
    int child = n;
    int parent = child / 2;
    while (parent > 0) {
        if (G->vertex[siftItem].cost >= G->vertex[heap[parent]].cost) break;
        heap[child] = heap[parent]; //move down parent
        heapLoc[heap[parent]] = child;
        child = parent;
        parent = child / 2;
    }
    heap[child] = siftItem;
    heapLoc[siftItem] = child;
} //end siftUp
void siftDown(Graph G, int key, int heap[], int root, int last, int heapLoc[]) {
    int smaller = 2 * root;
    while (smaller <= last) { //while there is at least one child
        if (smaller < last) //there is a right child as well; find the smaller
            if (G->vertex[heap[smaller+1]].cost < G->vertex[heap[smaller]].cost)
                smaller++;
        //'smaller' holds the index of the smaller child
        if (G -> vertex[key].cost <= G -> vertex[heap[smaller]].cost) break;
        //cost[key] is bigger; promote heap[smaller]
        heap[root] = heap[smaller];
        heapLoc[heap[smaller]] = root;
        root = smaller;
        smaller = 2 * root;
    } //end while
    heap[root] = key;
    heapLoc[key] = root;
} //end siftDown
Graph newGraph(int n) {
    if (n > NUM_VERTICES) {
        printf("\nToo big. Only %d vertices allowed.\n", NUM_VERTICES);
        exit(1);
    }
    Graph p = malloc(sizeof(*p));
    p -> numV = n;
    return p;
} //end newGraph

GVertex newGVertex(int id) {
    GVertex temp;
    temp.id = id;
    temp.firstEdge = NULL;
    return temp;
}
void addEdge(int srcID, int destID, int weight, Graph G) {
    //add an edge srcID -> destID with a given weight
    int h, k;
    //find srcID in the list of nodes; its location is h
    for (h = 1; h <= G->numV; h++) if (srcID == G->vertex[h].id) break;
    //find destID in the list of nodes; its location is k
    for (k = 1; k <= G->numV; k++) if (destID == G->vertex[k].id) break;
    if (h > G->numV || k > G->numV) {
        printf("No such edge: %d -> %d\n", srcID, destID);
        exit(1);
    }
    GEdgePtr ep = newGEdge(k, weight); //create edge vertex
    // add it to the list of edges, possible empty, from X;
    // it is added so that the list is in order by vertex id
    GEdgePtr prev, curr;
    prev = curr = G->vertex[h].firstEdge;
    while (curr != NULL && destID > G->vertex[curr->child].id)  {
        prev = curr;
        curr = curr->nextEdge;
    }
    if (prev == curr) {
        ep->nextEdge = G->vertex[h].firstEdge;
        G->vertex[h].firstEdge = ep;
    } else {
        ep->nextEdge = curr;
        prev->nextEdge = ep;
    }
} //end addEdge
GEdgePtr newGEdge(int c, int w) {
    //return a pointer to a new GEdge node
    GEdgePtr p = (GEdgePtr) malloc(sizeof(GEdge));
    p->child = c;
    p->weight = w;
    p->nextEdge = NULL;
    return p;}
/**
 *
 * @param G
 */
void buildGraphRandom(Graph G) {
    int numEdges;
    for (int h = 1; h <= G-> numV; h++) {
        G -> vertex[h] = newGVertex(h);      //create a vertex node
        //G -> vertex[h].id = h; //copy id to the graph
    }
    for (int h = 1; h <= G -> numV; h++) {
        numEdges = rand() % (MAX_NUM_EDGES + 1); // number of persons contacted
        if (!numEdges) { // if number of edges are zero then go to the next vertex
            continue;
        }
        /*destID = nodeID;
        if ( numEdges == 1) { // if there is only one edge
            while (destID == nodeID) {
                destID = rand() % G->numV;
            }
            addEdge(nodeID, destID, 1, G);
            continue;
        }*/
        int arr[numEdges];
        generateRandomYetSortedIntegers(arr, numEdges, G->numV - 1, 1, 1, h); // remember valid IDs vary from 1 to G->numV
        for (int k = 0; k < numEdges; k++) {
            addEdge(h, arr[k], 1, G); // since weight is always 1 for contact tracing
        }
    }
}//buildGraphRandom ends
void writeGraphToFile(FILE* in, Graph G){
    fprintf(in, "%d\n", G->numV);
    for ( int dex = 1 ;dex <= G->numV; dex++){
        fprintf(in, "%d ", G->vertex[dex].id);
    }
    fprintf(in, "%s", "\n");
    for ( int dex = 1 ;dex <= G->numV; dex++){
        fprintf(in, "%d ", G->vertex[dex].id);
        GEdgePtr p = G -> vertex[dex].firstEdge;
        while (p != NULL) {
            fprintf(in, "%d ", G -> vertex[p -> child].id);
            p = p -> nextEdge;
        }
        fprintf(in, "%s", "\n");
    }
}
int DijkstraContactTracing(Graph G, int s, int d) {
    // checking direct contact between source and destination first
    GEdgePtr p = G -> vertex[s].firstEdge;
    GEdgePtr q = G -> vertex[d].firstEdge;
    while (p != NULL ) { // checking for direct contact from source to destination
        if (G->vertex[p->child].id == d) return 0;
        p = p->nextEdge;
    }
    while (q != NULL ){ // checking for direct contact from destination to source
        if(G -> vertex[q -> child].id == s) return 0;
        q = q -> nextEdge;
    } // while loop to check for direct contact ends
    // if no direct contact between source and destination is found
    int heap[NUM_VERTICES + 1];    //heapLoc[i] gives the position in heap of vertex i
    int heapLoc[NUM_VERTICES + 1]; //if heapLoc[i] = k, then heap[k] contains i
    initSingleSource(G, s);
    for (int i = 1; i <= G -> numV; i++) heap[i] = heapLoc[i] = i;
    heap[1] = s; heap[s] = 1; heapLoc[s] = 1; heapLoc[1] = s;
    int heapSize = G -> numV;
    while (heapSize > 0) {
        int u = heap[1];
        if (u == d) return G->vertex[u].cost - 1;
        if (G -> vertex[u].cost == INT_MAX) break; //no paths to other vertices
        //reorganize heap after removing top item
        siftDown(G, heap[heapSize], heap, 1, heapSize-1, heapLoc);
        GEdgePtr p = G -> vertex[u].firstEdge;
        while (p != NULL) {
            if (G -> vertex[u].cost + p -> weight < G -> vertex[p -> child].cost) {
                G -> vertex[p -> child].cost = G -> vertex[u].cost + p -> weight;
                G -> vertex[p -> child].parent = u;
                siftUp(G, heap, heapLoc[p -> child], heapLoc);
            }
            p = p -> nextEdge;
        }
        --heapSize;
    } //end while
    return INT_MAX - 1; // if there is no path between the source and the destination
} //end DijkstraContactTracing
void generateRandomYetSortedIntegers(int arr[], int cnt, int range, int offset, int distinct, int exclude) {
    range =  cnt > range ? cnt : range;
    for (int dex = 0; dex < cnt; dex++ ) {
        int randInt = exclude;
        while (randInt == exclude ) {
            randInt = rand() % (range + 1) + offset;
        }
        arr[dex] = randInt;
        if (distinct) {
            for (int ctr = 0; ctr < dex; ctr++) {
                if (arr[dex] == arr[ctr]) {
                    dex--;
                    break;
                }
            }
        }
    }
    //quickSort(arr, 0, cnt - 1); // optionally you may provide code for quickSort and call it to sort the array
    int min = 0, temp = 0;

    for (int i = 0; i < cnt ; i++)
    {
        min = i;  // record the position of the smallest
        for (int j = i + 1; j < cnt; j++)
        {
            // update min when finding a smaller element
            if (arr[j] < arr[min])
                min = j;
        }
        // put the smallest element at position i
        temp = arr[i];
        arr[i] = arr[min];
        arr[min] = temp;
    } // array sorting ends here
} // function generateRandomYetSortedIntegers ends

void printGraph(Graph G, int region) {
    printf("-----------------Contact tracing data for %s-----------------------\n", regions[region]);
    puts("Person's\t# of direct\tIDs of people");
    puts("ID\t\tcontacts\tcontacted directly");
    for (int h = 1; h <= G->numV; h++) {
        printf("%d\t\t", G->vertex[h].id);
        GEdgePtr p = G->vertex[h].firstEdge;
        int ctr = 0;
        int tempArr[MAX_NUM_EDGES];
        while (p != NULL) {
            tempArr[ctr++] = G->vertex[p->child].id;
            p = p->nextEdge;
        }
        printf("%d\t\t", ctr);
        for (int dex = 0; dex < ctr; dex++) {
            printf("%d ", tempArr[dex]);
        }
        printf("\n");
    }
    printf("\n");
} //end printGraph
