#include "pl_graph_ops.h"
#include <stdlib.h>
#include <stdio.h>
#include "ints.h"
#include "graph.h"
#include "rgraph.h"
#include "graph.c"
#include "rgraph.c"
#include "ints.c"

void pl_rgraph_invert(int& rg_nverts, int& rg_degree,
    int& rg_n, int* rg_i,
    int& g_nverts, int& g_max_deg,
    int& g_off_n, int* g_off_i,
    int& g_adj_n, int* g_adj_i)
{
  #pragma HLS INTERFACE m_axi port=rg_nverts bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_degree bundle=gmem1 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_n bundle=gmem2 offset=slave
  #pragma HLS INTERFACE m_axi port=rg_i bundle=gmem3 offset=slave
  #pragma HLS INTERFACE m_axi port=g_nverts bundle=gmem4 offset=slave
  #pragma HLS INTERFACE m_axi port=g_max_deg bundle=gmem5 offset=slave
  #pragma HLS INTERFACE m_axi port=g_off_n bundle=gmem6 offset=slave
  #pragma HLS INTERFACE m_axi port=g_off_i bundle=gmem7 offset=slave
  #pragma HLS INTERFACE m_axi port=g_adj_n bundle=gmem8 offset=slave
  #pragma HLS INTERFACE m_axi port=g_adj_i bundle=gmem9 offset=slave
  #pragma HLS INTERFACE s_axilite port=rg_nverts
  #pragma HLS INTERFACE s_axilite port=rg_degree
  #pragma HLS INTERFACE s_axilite port=rg_n
  #pragma HLS INTERFACE s_axilite port=rg_i
  #pragma HLS INTERFACE s_axilite port=g_nverts
  #pragma HLS INTERFACE s_axilite port=g_max_deg
  #pragma HLS INTERFACE s_axilite port=g_off_n
  #pragma HLS INTERFACE s_axilite port=g_off_i
  #pragma HLS INTERFACE s_axilite port=g_adj_n
  #pragma HLS INTERFACE s_axilite port=g_adj_i
  printf("hi from rgraph_invert_pipelined rg_nverts %d\n", rg_nverts);

  #include "rgraph.h"       
  //Create an ints struct for adj in rgraph   
  ints rg_adj;
  rg_adj.n=rg_n; 
  rg_adj.i=rg_i;
  //Create rgraph struct
  rgraph rg;
  rg.nverts=rg_nverts;
  rg.degree=rg_degree;
  rg.adj=rg_adj;
  //printf("");
  #include "graph.h"
  //ints strcut for adj
  ints g_adj;
  g_adj.n=g_adj_n;
  g_adj.i=g_adj_i;
  //ints struct for offset
  ints g_off2;
  g_off2.n=g_off_n;
  g_off2.i=g_off_i;
  //Create graph struct
  graph g;
  g.nverts=g_nverts;
  g.max_deg=g_max_deg;
  g.off=g_off2;
  g.adj=g_adj;
  //Try rgraph invert
  //int nverts=rgraph_max_adj(rg)+1;
  struct graph_spec s;
  s.nverts=rgraph_max_adj(rg)+1;
  //statically allocate
  static int degs_g[s.nverts];
  s.deg=degs_g;
  ints_zero(s.deg);
}

pl_rgraph_invert_0(int& s_nvert,)

void read(int const *in, Stream<int> &stream){
  //Allocate space for graph_spec
  //for 
}
