#include "refine.h"
#include "graph_ops.h"
#include "adj_ops.h"
#include <float.h>
#include <stdio.h>

static struct ints mark_fes(struct graph efs, struct ints bfs) __attribute__((noinline));
static struct ss compute_split_quals(struct ints ecss, struct rgraph evs,
    struct graph efs, struct rgraph fvs, struct xs xs) __attribute__((noinline));
static struct ints compute_best_indset(struct ints ecss, struct graph ees,
    struct ss eqs) __attribute__((noinline));
static struct ints mark_split_faces(struct ints ewss, struct rgraph fes) __attribute__((noinline));
static struct xs split_edges(struct xs xs,
    struct ints ewss, struct rgraph evs) __attribute__((noinline));
static struct rgraph split_faces(struct rgraph fvs,
    struct ints fwss, struct ints ewss, struct rgraph evs,
    struct graph efs, int nv) __attribute__((noinline));

static struct ints mark_fes(struct graph efs, struct ints bfs)
{
  struct ints ecss = ints_new(efs.nverts);
  ints_zero(ecss);
  struct adj ef = adj_new(efs.max_deg);
  for (int i = 0; i < efs.nverts; ++i) { // loop over edges
    graph_get(efs, i, &ef); // get faces bound by edge
    for (int j = 0; j < ef.n; ++j)
      if (bfs.i[ef.e[j]]) { // if face is marked
        ecss.i[i] = 1;      // then mark the edge for splitting
        break;              // don't need to check the other face bound by this edge
      }
  }
  adj_free(ef);
  return ecss;
}

static struct ss compute_split_quals(struct ints ecss, struct rgraph evs,
    struct graph efs, struct rgraph fvs, struct xs xs)
{
  struct ss eqs = ss_new(efs.nverts);
  struct adj ef = adj_new(2);
  for (int i = 0; i < ecss.n; ++i) {    // loop over the marked edges
    if (!ecss.i[i])                     // skip edge if not marked
      continue;
    int ev[2];
    rgraph_get(evs, i, ev);             // get vertices bounding edge
    struct x ex[2];
    xs_get(xs, ev, 2, ex);              // get coordinates of verts
    struct x mid = x_avg(ex[0], ex[1]); // get mid point of edge
    graph_get(efs, i, &ef);             // get faces bound by edge
    double wq = DBL_MAX;
    for (int j = 0; j < ef.n; ++j) {    // loop over bound faces
      int f = ef.e[j];
      int fv[3];
      rgraph_get(fvs, f, fv);           // get vertices of face
      struct x fx[3];
      for (int k = 0; k < 2; ++k) {
        xs_get(xs, fv, 3, fx);          // get coordinates of face vertices
        for (int l = 0; l < 3; ++l)
          if (fv[l] == ev[k])           // find the marked edge
            fx[l] = mid;                // change the coordinate of the vertex to the mid point
        double q = fx_qual(fx);         // measure quality
        if (q < wq)                     // if this is the smallest quality measured
          wq = q;                       // store it as 'wq', worst quality
      }
    }
    eqs.s[i] = wq;                      // associate wq with the edge
  }
  adj_free(ef);
  return eqs;
}

static struct ints compute_best_indset(struct ints ecss, struct graph ees,
    struct ss eqs)
{
  enum { WONT_SPLIT = 0, WILL_SPLIT = 1, COULD_SPLIT = 2 };
  struct ints ewss = ints_new(ecss.n);
  struct ints ewss_old = ints_new(ecss.n);
  for (int i = 0; i < ecss.n; ++i) {
    if (ecss.i[i])
      ewss.i[i] = COULD_SPLIT;
    else
      ewss.i[i] = WONT_SPLIT;
  }
  struct adj ee = adj_new(ees.max_deg);
  int done = 0;
  int iter;
  for (iter = 0; !done; ++iter) {
    done = 1;
    ints_from_dat(ewss_old, ewss.i);              // copy ewss to ewss_old
    for (int i = 0; i < ecss.n; ++i) {            // loop over edges that could be split
      if (ewss_old.i[i] != COULD_SPLIT)
        continue;
      double q = eqs.s[i];
      graph_get(ees, i, &ee);                     // get second adj edges via faces
      for (int j = 0; j < ee.n; ++j)
        if (ewss_old.i[ee.e[j]] == WILL_SPLIT)
          ewss.i[i] = WONT_SPLIT;                 // if any of adj edge is marked for splitting
                                                  //  then this edge won't be split
      if (ewss.i[i] != COULD_SPLIT)               // if the edge was just marked as won't split
        continue;                                 //  then skip it
      int local_max = 1;
      for (int j = 0; j < ee.n; ++j) {            // loop over second adjacent edges and find
        if (ewss_old.i[ee.e[j]] == WONT_SPLIT)    //  the one with the highest quality to split
          continue;
        assert(ee.e[j] != i);
        double oq = eqs.s[ee.e[j]];
        if (oq == q) {
          if (ee.e[j] < i)
            local_max = 0;
        } else {
          if (q < oq)
            local_max = 0;
        }
      }
      if (local_max)
        ewss.i[i] = WILL_SPLIT;
      else
        done = 0;
    }
  }
  adj_free(ee);
  ints_free(ewss_old);
  return ewss;
}

static struct ints mark_split_faces(struct ints ewss, struct rgraph fes)
{
  struct ints fwss = ints_new(fes.nverts);
  struct adj fe = adj_new_rgraph(fes);
  for (int i = 0; i < fes.nverts; ++i) {
    fwss.i[i] = 0;
    rgraph_get(fes, i, fe.e);
    for (int j = 0; j < fe.n; ++j)
      if (ewss.i[fe.e[j]])
        fwss.i[i] = 1;
  }
  adj_free(fe);
  return fwss;
}

static struct xs split_edges(struct xs xs,
    struct ints ewss, struct rgraph evs)
{
  struct ints eos = ints_exscan(ewss);
  int nse = eos.i[ewss.n];                  // nse: number of split edges
  struct xs xs2 = xs_new(xs.n + nse);       // xs2: new coordinates sized to hold old and new verts
  for (int i = 0; i < xs.n; ++i)
    xs2.x[i] = xs.x[i];
  for (int i = 0; i < ewss.n; ++i)          // loop over edges
    if (ewss.i[i]) {                        // if an edge to split
      int ev[2];
      rgraph_get(evs, i, ev);               // ev: vertices of edge
      struct x ex[2];
      xs_get(xs, ev, 2, ex);                // ex: coordinates of vertices
      struct x mid = x_avg(ex[0], ex[1]);   // mid: midpoint between vertices
      xs2.x[xs.n + eos.i[i]] = mid;         // set the new coordinate
    }
  ints_free(eos);
  return xs2;
}

static struct rgraph split_faces(struct rgraph fvs,
    struct ints fwss, struct ints ewss, struct rgraph evs,
    struct graph efs, int nv)
{
  struct ints fos = ints_exscan(fwss);
  struct ints eos = ints_exscan(ewss);                   // eos: number of edges split
  int nsf = fos.i[fwss.n];                               // nsf: number of faces split
  struct rgraph fvs2 = rgraph_new(fvs.nverts + nsf, 3);  // fvs2: faces to vertices sized to hold old + new faces
  for (int i = 0; i < fwss.n; ++i)                       // loop to copy fvs to fvs2
    if (!fwss.i[i]) {
      int fv[3];
      rgraph_get(fvs, i, fv);
      rgraph_set(fvs2, i, fv);
    }
  struct adj ef = adj_new_graph(efs);
  for (int i = 0; i < ewss.n; ++i)
    if (ewss.i[i]) {                                     // if an edge to split
      int ev[2];
      rgraph_get(evs, i, ev);                            // ev: vertices of split edge
      graph_get(efs, i, &ef);                            // ef: faces of split edge
      int sv = nv + eos.i[i];                            // sv: index to new vertex
      for (int j = 0; j < ef.n; ++j) {                   // loop over faces of split edge
        int f = ef.e[j];                                 // f: jth face
        int sf[2];
        sf[0] = f;                                       // sf[0]: id of original face
        sf[1] = fvs.nverts + fos.i[f];                   // sf[1]: id of the new face
        int fv[3];
        for (int k = 0; k < 2; ++k) {
          rgraph_get(fvs, f, fv);                        // get vertices of original face
          for (int l = 0; l < 3; ++l)                    // loop over the three vertices of the face
            if (fv[l] == ev[k])                          // if the vtx in the original face was bounding a split edge
              fv[l] = sv;                                // replace the vertex with the new vertex
          rgraph_set(fvs2, sf[k], fv);                   // write the modified list of face vertices
        }
      }
    }
  adj_free(ef);
  ints_free(eos);
  ints_free(fos);
  return fvs2;
}

void refine(struct rgraph fvs, struct xs xs, struct ss dss,
    struct rgraph* pfvs2, struct xs* pxs2)
{
  struct graph vfs = rgraph_invert(fvs); // vfs: verts to faces
  struct graph vvs = graph_rgraph_transit(vfs, fvs); // vvs: verts to verts via faces
  graph_free(vfs);
  struct rgraph evs = graph_bridge(vvs); // evs: edges to vertices
  graph_free(vvs);
  struct graph ves = rgraph_invert(evs); // ves: verts to edges
  struct rgraph fes = compute_fes(fvs, ves); // fes: stores the edges bounding a face in a specific order
  graph_free(ves);
  struct graph efs = rgraph_invert(fes); // efs: edges to faces
  struct ss as = compute_areas(xs, fvs); // as: area of each triangle
  struct ints bfs = ss_gt(as, dss); // bfs: mask of which triangles have area greater than the size field
  ss_free(as);
  struct ints ecss = mark_fes(efs, bfs); // ecss: edges that could be split
  ints_free(bfs);
  struct ss eqs = compute_split_quals(ecss, evs, efs, fvs, xs); // eqs: edge qualities
  struct graph ees = graph_rgraph_transit(efs, fes); // ees: edge-to-edge via faces
  struct ints ewss = compute_best_indset(ecss, ees, eqs); // ewss: edges that will be split
  graph_free(ees);
  ss_free(eqs);
  ints_free(ecss);
  struct ints fwss = mark_split_faces(ewss, fes); // fwss: faces that will be split
  rgraph_free(fes);
  struct xs xs2 = split_edges(xs, ewss, evs);  // xs2: old coordinates with new midpoints appended
  *pxs2 = xs2;
  struct rgraph fvs2 = split_faces(fvs, fwss, ewss, evs, efs, xs.n);
  *pfvs2 = fvs2;
  ints_free(fwss);
  ints_free(ewss);
  rgraph_free(evs);
  graph_free(efs);
}

