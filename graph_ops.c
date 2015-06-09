#include "graph_ops.h"
#include <stdlib.h>
#include <stdio.h>

struct graph rgraph_invert(struct rgraph rg)
{
  int nverts = rgraph_max_adj(rg) + 1;
  struct graph_spec s = graph_spec_new(nverts);
  ints_zero(s.deg);
  #pragma omp parallel
  {
    struct adj a = adj_new_rgraph(rg);
    a.n = rg.degree;
    #pragma omp for
    for (int i = 0; i < rg.nverts; ++i) {
      rgraph_get(rg, i, a.e);
      for (int j = 0; j < a.n; ++j) {
        int* p = &(s.deg.i[a.e[j]]);
        #pragma omp atomic update
        ++(*p);
      }
    }
    adj_free(a);
  }
  struct graph g = graph_new(s);
  struct ints at = ints_new(nverts);
  ints_from_dat(at, g.off.i);
  #pragma omp parallel
  {
    struct adj a = adj_new_rgraph(rg);
    a.n = rg.degree;
    #pragma omp for
    for (int i = 0; i < rg.nverts; ++i) {
      rgraph_get(rg, i, a.e);
      for (int j = 0; j < a.n; ++j) {
        int av = a.e[j];
        int* p = &(at.i[av]);
        int o;
        #pragma omp atomic capture
        o = (*p)++;
        g.adj.i[o] = i;
      }
    }
    adj_free(a);
  }
  ints_free(at);
  return g;
}

struct graph graph_rgraph_transit(struct graph g, struct rgraph rg)
{
  struct graph_spec s = graph_spec_new(g.nverts);
  ints_zero(s.deg);
  #pragma omp parallel
  {
    struct adj ga = adj_new_graph(g);
    struct adj ra = adj_new_rgraph(rg);
    struct adj ta = adj_new(ga.c * ra.c);
    #pragma omp for
    for (int i = 0; i < g.nverts; ++i) {
      graph_get(g, i, &ga);
      rgraph_get(rg, ga.e[0], ta.e);
      ta.n = rg.degree;
      for (int j = 1; j < ga.n; ++j) {
        rgraph_get(rg, ga.e[j], ra.e);
        adj_unite(&ta, ra);
      }
      s.deg.i[i] = ta.n - 1;
    }
    adj_free(ga);
    adj_free(ra);
    adj_free(ta);
  }
  struct graph tg = graph_new(s);
  #pragma omp parallel
  {
    struct adj ga = adj_new_graph(g);
    struct adj ra = adj_new_rgraph(rg);
    struct adj ta = adj_new(ga.c * ra.c);
    #pragma omp for
    for (int i = 0; i < g.nverts; ++i) {
      graph_get(g, i, &ga);
      rgraph_get(rg, ga.e[0], ta.e);
      ta.n = rg.degree;
      for (int j = 1; j < ga.n; ++j) {
        rgraph_get(rg, ga.e[j], ra.e);
        adj_unite(&ta, ra);
      }
      adj_remove(&ta, i);
      graph_set(tg, i, ta);
    }
    adj_free(ga);
    adj_free(ra);
    adj_free(ta);
  }
  return tg;
}

struct rgraph graph_bridge(struct graph g)
{
  int nhe = graph_nedges(g);
  assert(nhe % 2 == 0);
  int ne = nhe / 2;
  struct rgraph rg = rgraph_new(ne, 2);
  struct ints naes = ints_new(g.nverts);
  #pragma omp parallel
  {
    struct adj a = adj_new_graph(g);
    #pragma omp for
    for (int i = 0; i < g.nverts; ++i) {
      graph_get(g, i, &a);
      int nae = 0;
      for (int j = 0; j < a.n; ++j)
        if (i < a.e[j])
          nae++;
      naes.i[i] = nae;
    }
    adj_free(a);
  }
  struct ints os = ints_exscan(naes);
  ints_free(naes);
  #pragma omp parallel
  {
    struct adj a = adj_new_graph(g);
    #pragma omp for
    for (int i = 0; i < g.nverts; ++i) {
      int ra[2];
      ra[0] = i;
      graph_get(g, i, &a);
      int k = os.i[i];
      for (int j = 0; j < a.n; ++j)
        if (i < a.e[j]) {
          ra[1] = a.e[j];
          rgraph_set(rg, k++, ra);
        }
    }
    adj_free(a);
  }
  ints_free(os);
  return rg;
}
