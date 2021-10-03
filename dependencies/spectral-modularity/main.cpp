/*
 *  Graph clustering with modularity optimization by leading eigenvectors
 * */

#include <igraph.h>
#include <math.h>
#include <omp.h>
#include <string>

int main(int argc, char *argv[]) {
    igraph_t graph;
    igraph_vector_t membership;
    igraph_vector_init(&membership, 0);

    std::string infile(argv[1]);
    std::string outfile(argv[2]);
    FILE *fin;
    FILE *fout;
    fin = fopen(infile.c_str(), "r");
    fout = fopen(outfile.c_str(), "w");

    double t0, t1;
    
    fprintf(fout, "File: %s\n", infile.c_str());

    t0 = omp_get_wtime();
    igraph_read_graph_ncol(&graph, fin, NULL, IGRAPH_DIRECTED, IGRAPH_ADD_WEIGHTS_IF_PRESENT, IGRAPH_UNDIRECTED);
    t1 = omp_get_wtime();
    fprintf(fout, "Read: %lf seconds\n", t1-t0);
    fclose(fout);
    fout = fopen(outfile.c_str(), "a");

    long int n_edges = (long int ) ( igraph_ecount(&graph) );
    long int n_vertices = (long int) ( igraph_vcount(&graph) );
    fprintf(fout, "Vertices: %ld\n", n_vertices);
    fprintf(fout, "Edges: %ld\n", n_edges);

    igraph_vector_init_seq(&membership, 0, n_vertices-1); // Singleton clusters
    igraph_arpack_options_t options;
    igraph_arpack_options_init(&options);

    t0 = omp_get_wtime();
    igraph_real_t modularity;
    igraph_integer_t nb_cluster;
    igraph_community_leading_eigenvector(
            &graph, // graph
            NULL, // edge weights
            NULL, // merges
            &membership, // membership vector
            n_vertices, // steps
            &options, // options
            &modularity, // modularity
            0, // start
            NULL, // eigenvalues
            NULL, //eigenvectors
            NULL, // history
            NULL, // callback
            NULL // extra callback parameter
            );
    t1 = omp_get_wtime();
    fprintf(fout, "Compute: %lf seconds\n", t1-t0);
    fprintf(fout, "Modularity: %g\n", modularity);
    fprintf(fout, "Communities: %ld\n", (long int)(igraph_vector_max(&membership)+1) );

    long int i, j;
    for (j = 0; j < igraph_vector_size(&membership); j++) {
        fprintf(fout, "%ld\n", (long int) VECTOR(membership)[j]);
    }
    fprintf(fout, "\n");

    fclose(fin);
    fclose(fout);
    
    /* Free memory */
    igraph_vector_destroy(&membership);
    igraph_destroy(&graph);

    return 0;
}
