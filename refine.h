#ifndef REFINE_H
#define REFINE_H

#include "space.h"

/**
 * \brief refine the mesh to satisfy the given satisfy
 * \parameter fvs (in) face-to-vertices graph structure
 * \parameter xs (in) coordinates of mesh vertices
 * \parameter dss (in) size field
 * \parameter fvs2 (in/out) refined face-to-vertices graph structure
 * \parameter xs2 (in/out) mesh coordinates associated with fvs2
 */
void refine(struct rgraph fvs, struct xs xs, struct ss dss,
    struct rgraph* fvs2, struct xs* xs2);

#endif
