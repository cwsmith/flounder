#ifndef PL_GRAPH_OPS_H
#define PL_GRAPH_OPS_H

#ifdef __cplusplus
extern "C" {
#endif
void pl_rgraph_invert(int* rg_nverts, int* rg_degree,
    int* rg_n, int* rg_i,
    int* g_nverts, int* g_max_deg,
    int* g_off_n, int* g_off_i,
    int* g_adj_n, int* g_adj_i);
#ifdef __cplusplus
}
#endif

#endif
