/* Two-dimensional Upwind/Donor-cell code - taken from Hirsch II */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>

#include "boost/multi_array.hpp"


#define USE_HDF5 1

#ifdef HAVE_LIBMPICH
#include "mpi.h"
#endif

#define TWODIM

#include "tnt.h"
using namespace TNT;

using namespace std;
#define DEBUG 1
#ifndef GLOBAL_VARIABLES
#define GLOBAL_VARIABLES 1

#include "sgn.h"
#include "eigenvectors.h"
#include "hlld.h"
#include "roe.h"
#include "vanleer.h"
extern int nx;
extern int ny;
extern int ne;
extern double delta_x;
extern double gammag;
extern double pmin;

#define _MASS .array[0]
#define _MOMX .array[1]
#define _MOMY .array[2]
#define _MOMZ .array[3]
#define _ENER .array[4]
#define _B_X .array[5]
#define _B_Y .array[6]
#define _B_Z .array[7]
#define _COOLING .temperature

typedef struct {
  double array[8];
  double temperature;
  double cooling;
} zone;

typedef boost::multi_array<double, 2> array_2d;
#endif //  GLOBAL_VARIABLES
