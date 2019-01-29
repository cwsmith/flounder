#ifndef PL_GRAPH_OPS_H 
#define PL_GRAPH_OPS_H 

#ifdef __cplusplus
extern "C" {
#endif
void pl_rgraph_invert(int* rg_nverts, int* rg_degree,
    int* g_nverts, int* g_max_deg);
#ifdef __cplusplus
}
#endif

#endif
