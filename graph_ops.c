#include "graph_ops.h"
#include <stdlib.h>
#include <stdio.h>

struct graph rgraph_invert(struct rgraph rg)
{
  int nverts = rgraph_max_adj(rg) + 1;           // get the max number of adjacencies for a single entry
  struct graph_spec s = graph_spec_new(nverts);
  ints_zero(s.deg);
  struct adj a = adj_new_rgraph(rg);
  a.n = rg.degree;
  // build the count of how many time each adjacency in the rgraph appears
  //  this count is the basis of the CSR graph offset array
  for (int i = 0; i < rg.nverts; ++i) {          // loop over the 'parent' rgraph entities
    rgraph_get(rg, i, a.e);                      // get the 'child' entities adjacent to a parent
    for (int j = 0; j < a.n; ++j)                // loop over the children
      s.deg.i[a.e[j]]++;                         // count how many times each child entity appears
  }
  struct graph g = graph_new(s);                 // the child->parent graph
  struct ints at = ints_new(nverts);
  ints_from_dat(at, g.off.i);
  // run the same loop as before and store the parents for each child
  for (int i = 0; i < rg.nverts; ++i) {
    rgraph_get(rg, i, a.e);
    for (int j = 0; j < a.n; ++j) {
      int av = a.e[j];
      g.adj.i[at.i[av]] = i;
      at.i[av]++;
    }
  }
  ints_free(at);
  adj_free(a);
  return g;
}

// compute second adjacencies from g to rg
// for example, if g is edges to faces and rg is faces to edges, then
// the output is edges-to-edges via faces
struct graph graph_rgraph_transit(struct graph g, struct rgraph rg)
{
  struct graph_spec s = graph_spec_new(g.nverts);
  ints_zero(s.deg);
  struct adj ga = adj_new_graph(g);
  struct adj ra = adj_new_rgraph(rg);
  struct adj ta = adj_new(ga.c * ra.c);
  for (int i = 0; i < g.nverts; ++i) {      // loop over edges
    graph_get(g, i, &ga);                   // store faces adj to i'th edge in 'ga'
    rgraph_get(rg, ga.e[0], ta.e);          // store edges adj to the first face in 'ta'
    ta.n = rg.degree;                       // set the count of 'ta' to 3 (edges bouding a face) ?
    for (int j = 1; j < ga.n; ++j) {        // loop over remaining faces adj to i'th edge
      rgraph_get(rg, ga.e[j], ra.e);        // store edges adj to the j'th face in 'ra'
      adj_unite(&ta, ra);                   // add edges from ra' that are not in 'ta'
    }
    s.deg.i[i] = ta.n - 1;
  }
  struct graph tg = graph_new(s);
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
  return tg;
}

// this is only called from refine with a vertex-to-vertex via face graph
struct rgraph graph_bridge(struct graph g)
{
  int nhe = graph_nedges(g);
  assert(nhe % 2 == 0);
  int ne = nhe / 2;                          // each edge appears exactly twice in g
  struct rgraph rg = rgraph_new(ne, 2);      // the edge-to-vertex rgraph is downward with deg=2
  struct adj a = adj_new_graph(g);           // a large enough adj struct for any adj of g
  struct ints naes = ints_new(g.nverts);     // naes: number of adjacent ents ?
  for (int i = 0; i < g.nverts; ++i) {
    graph_get(g, i, &a);                     // vertices adjacent to parent vtx i
    int nae = 0;
    for (int j = 0; j < a.n; ++j)            // loop over adjacent verts
      if (i < a.e[j])                        // count each edge once
        nae++;
    naes.i[i] = nae;
  }
  struct ints os = ints_exscan(naes);
  ints_free(naes);
  int ra[2];                                 // the two vertices for each edge
  for (int i = 0; i < g.nverts; ++i) {
    ra[0] = i;                               // the parent vtx is the first in the edge
    graph_get(g, i, &a);
    int k = os.i[i];                         // the edge we are creating in rg
    for (int j = 0; j < a.n; ++j)
      if (i < a.e[j]) {                      // count each edge once
        ra[1] = a.e[j];                      // child vertex is second in the edge
        rgraph_set(rg, k++, ra);             // write the edge into the rg
      }
  }
  ints_free(os);
  adj_free(a);
  return rg;
}
