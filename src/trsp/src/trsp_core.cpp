#include "GraphDefinition.h"
#include "utils.h"

#undef PGR_LOGGER_ON
#define PGR_LOGGER_ON
#include "pgr_logger.h"

#include "pgr_time.h"


int trsp_node_wrapper(
    edge_t *edges,
    unsigned int edge_count,
    restrict_t *restricts,
    int restrict_count,
    int start_vertex,
    int end_vertex,
    bool directed,
    bool has_reverse_cost,
    path_element_t **path,
    int *path_count,
    char **err_msg
    )
{
    try {

        std::vector<PDVI> ruleTable;

        double t0 = pgr_gettime();
        int i, j;
        ruleTable.clear();
        for (i=0; i<restrict_count; i++) {
            std::vector<int> seq;
            seq.clear();
            seq.push_back(restricts[i].target_id);
            for(j = 0; j<MAX_RULE_LENGTH && restricts[i].via[j]>-1; j++)
            {
                seq.push_back(restricts[i].via[j]);
            }
            ruleTable.push_back(make_pair(restricts[i].to_cost, seq));
        }

        t0 = pgr_gettime() - t0;
        PGR_LOGF("TIME to build ruleTable for restrictions: %f\n", t0);

        GraphDefinition gdef;
        int res = gdef.my_dijkstra(edges, edge_count, start_vertex, end_vertex, directed, has_reverse_cost, path, path_count, err_msg, ruleTable);


        if (res < 0)
            return res;
        else
            return EXIT_SUCCESS;
    }
    catch(std::exception& e) {
        *err_msg = (char *) e.what();
        return -1;
    }
    catch(...) {
        *err_msg = (char *) "Caught unknown exception!";
        return -1;
    }
}

int trsp_edge_wrapper(
    edge_t *edges,
    unsigned int edge_count,
    restrict_t *restricts,
    int restrict_count,
    int start_edge,
    double start_pos,
    int end_edge,
    double end_pos,
    bool directed,
    bool has_reverse_cost,
    path_element_t **path,
    int *path_count,
    char **err_msg
    )
{
    try {

        std::vector<PDVI> ruleTable;

        double t0 = pgr_gettime();
        int i, j;
        ruleTable.clear();
        for (i=0; i<restrict_count; i++) {
            std::vector<int> seq;
            seq.clear();
            seq.push_back(restricts[i].target_id);
            for(j = 0; j<MAX_RULE_LENGTH && restricts[i].via[j]>-1; j++)
            {
                seq.push_back(restricts[i].via[j]);
            }
            ruleTable.push_back(make_pair(restricts[i].to_cost, seq));
        }

        t0 = pgr_gettime() - t0;
        PGR_LOGF("TIME to build ruleTable for restrictions: %f\n", t0);

        GraphDefinition gdef;
        int res = gdef.my_dijkstra(edges, edge_count, start_edge, start_pos, end_edge, end_pos, directed, has_reverse_cost, path, path_count, err_msg, ruleTable);


        if (res < 0)
            return res;
        else
            return EXIT_SUCCESS;
    }
    catch(std::exception& e) {
        *err_msg = (char *) e.what();
        return -1;
    }
    catch(...) {
        *err_msg = (char *) "Caught unknown exception!";
        return -1;
    }
}
