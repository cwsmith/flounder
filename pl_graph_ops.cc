#include "pl_graph_ops.h"
#include <stdlib.h>
#include <stdio.h>

void pl_rgraph_invert(
    int* rg_nverts, int* rg_degree,
    int* g_nverts, int* g_max_deg)
{
  #pragma HLS INTERFACE m_axi port=rg_nverts bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_degree bundle=gmem1 offset=slave
  #pragma HLS INTERFACE m_axi port=g_nverts bundle=gmem2 offset=slave
  #pragma HLS INTERFACE m_axi port=g_max_deg bundle=gmem3 offset=slave
  #pragma HLS INTERFACE s_axilite port=rg_nverts
  #pragma HLS INTERFACE s_axilite port=rg_degree
  #pragma HLS INTERFACE s_axilite port=g_nverts
  #pragma HLS INTERFACE s_axilite port=g_max_deg
  printf("hi from rgraph_invert_pipelined rg_nverts %d\n", *rg_nverts);
}
