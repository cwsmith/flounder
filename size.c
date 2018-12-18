#include "size.h"

struct ss gen_size_field(struct rgraph fvs, struct xs xs,
    double (*fun)(struct x))
{
  struct ss dss = ss_new(fvs.nverts);
  // loop over faces
  for (int i = 0; i < dss.n; ++i) {
    int fv[3]; // vertices of the face
    rgraph_get(fvs, i, fv);
    struct x fx[3]; //vertex coordinates
    xs_get(xs, fv, 3, fx);
    struct x c = fx_center(fx);
    dss.s[i] = fun(c);
  }
  return dss;
}
