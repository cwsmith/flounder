#include "pl_graph_ops.h"
#include <stdlib.h>
#include <stdio.h>

void pl_rgraph_invert(int& rg_nverts, int& rg_degree,
    int& rg_n, int& rg_i,
    int& g_nverts, int& g_max_deg,
    int& g_off_n, int& g_off_i,
    int& g_adj_n, int& g_adj_i)
{
  #pragma HLS INTERFACE m_axi port=rg_nverts bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_degree bundle=gmem1 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_adj_n bundle=gmem2 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_adj_i bundle=gmem3 offset=slave
  #pragma HLS INTERFACE m_axi port=g_nverts bundle=gmem4 offset=slave
  #pragma HLS INTERFACE m_axi port=g_max_deg bundle=gmem5 offset=slave
  #pragma HLS INTERFACE m_axi port=g_off_n bundle=gmem6 offset=slave
  #pragma HLS INTERFACE m_axi port=g_off_i bundle=gmem7 offset=slave
  #pragma HLS INTERFACE m_axi port=g_adj_n bundle=gmem8 offset=slave
  #pragma HLS INTERFACE m_axi port=g_adj_i bundle=gmem9 offset=slave
  #pragma HLS INTERFACE s_axilite port=rg_nverts
  #pragma HLS INTERFACE s_axilite port=rg_degree
  #pragma HLS INTERFACE s_axilite port=rg_adj_n
  #pragma HLS INTERFACE s_axilite port=rg_adj_i
  #pragma HLS INTERFACE s_axilite port=g_nverts
  #pragma HLS INTERFACE s_axilite port=g_max_deg
  #pragma HLS INTERFACE s_axilite port=g_off_n
  #pragma HLS INTERFACE s_axilite port=g_off_i
  #pragma HLS INTERFACE s_axilite port=g_adj_n
  #pragma HLS INTERFACE s_axilite port=g_adj_i
  printf("hi from rgraph_invert_pipelined rg_nverts %d\n", rg_nverts);
}
