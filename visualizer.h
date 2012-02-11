#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <list>
#include <set>

#include <cairo.h>

#include "astar.h"

using namespace std;

void visualize( list< Edge >, list< Edge >, vector< vector< bool > >, Point );

#endif
