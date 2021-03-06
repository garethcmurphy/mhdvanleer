#include "update.h"
#define ROE
#undef COOLING
#undef  VANLEER
//#define STAGGER_MESH 
//#define  POWELL
int monopole(double *source_term, const double *old, double divb);
int Updater::update(Array3D<zone> NewGrid,
                    Array3D<zone> oldg,
                    Array3D<zone> xflux,
                    Array3D<zone> yflux,
                    double delta,
                    int ii,
                    int jj) {

  double rho, rhoi;

  int idir = 1;

  //      double fx2[ne],fx1[ne];
//      double fy2[ne],fy1[ne];
  double d[2];


  if (idir == 1) {
    d[0] = 1;
    d[1] = 0;
  } else if (idir == 2) {
    d[0] = 0;
    d[1] = 1;
  }

#ifdef TWODIM
  for (jj = 2; jj < ny - 2; jj++)
#endif /* TWODIM */
  {
    for (ii = 2; ii < nx - 2; ii++) {
      int kk = 0;

//      cout << "upd: " << jj << endl;
      double *fx1;
      double *fx2;
      double *fy1;
      double *fy2;
#ifdef ROE
      fx2 = xflux[ii + 1][jj][kk].array;
      fx1 = xflux[ii][jj][kk].array;
#ifdef TWODIM
      fy2 = yflux[ii][jj + 1][kk].array;
      fy1 = yflux[ii][jj][kk].array;
#endif /* TWODIM */

      if (std::isnan(oldg[ii][jj][kk]_MASS)) {
        cout << "upd:Unphysical density" << endl;
        exit(0);
      }
      if (std::isnan(oldg[ii][jj][kk]_ENER)) {
        cout << "upd:Unphysical energy" << endl;
        exit(0);
      }

      int hh = 0;
      for (hh = 0; hh < ne; hh++) {
        NewGrid[ii][jj][kk].array[hh] = oldg[ii][jj][kk].array[hh]
            - delta * (fx2[hh] - fx1[hh]);
#ifdef TWODIM
        NewGrid[ii][jj][kk].array[hh] = oldg[ii][jj][kk].array[hh]
            - delta * (fx2[hh] - fx1[hh])
            - delta * (fy2[hh] - fy1[hh]);
#endif
      }
      if (false && oldg[ii][jj][kk]_MOMX != oldg[ii + 1][jj][kk]_MOMX) {
        cout << NewGrid[ii][jj][kk]_MASS
             << " " << oldg[ii][jj][kk]_MASS
             << " " << delta << " " << fx2[0] << " " << fx1[0] << endl;
      }

      if (std::isnan(NewGrid[ii][jj][kk]_MASS)) {
        cout << "upd:Unphysical density" << endl;
        exit(0);
      }
      if (std::isnan(NewGrid[ii][jj][kk]_ENER)) {
        cout << "upd:newg:Unphysical energy" << endl;
        cout
            << " " << NewGrid[ii][jj][kk]_ENER
            << " " << delta
            << " " << oldg[ii][jj][kk]_ENER
            << " " << fx2[4]
            << " " << fx1[4]
            << endl;
        exit(0);
      }

      // Can add a cooling source term here
#ifdef COOLING
      double coolvar[ne];
  double source_term[ne];
  double Lcooling;

      coolvar[0] = oldg[ii][jj][kk] _MASS;
      coolvar[1] = oldg[ii][jj][kk] _MOMX;
      coolvar[2] = oldg[ii][jj][kk] _MOMY;
      coolvar[3] = oldg[ii][jj][kk] _ENER;

  int status = 0;
      RadiativeCooling cool;
      status = cool.cooling (coolvar, &Lcooling, dt);
  assert(status == 0);
      NewGrid[ii][jj][kk] _ENER = NewGrid[ii][jj][kk] _ENER + Lcooling;
      NewGrid[ii][jj][kk] _COOLING = Lcooling;
#endif /* COOLING */

#endif       /*ROE*/
#ifdef VANLEER_XXX
      /*
               idir = 1;



            status = flux (oldg, fp,  fn,  ii+1, jj,timestep, idir);
            assert(status == 0);
            status = flux (oldg, fp1, fn1, ii  , jj,timestep, idir);
            assert(status == 0);
            status = flux (oldg, fp2, fn2, ii-1, jj,timestep, idir);
            assert(status == 0);

               if ( jj==5)
               {
             for (hh = 0; hh < 1; hh++)
                   {
               cout
                << "[" << ii << "," << jj << "]="
                   << fp[hh] << " " << fp1[hh] << " " << fp2[hh] << endl;
                  }
               }


              for (hh = 0; hh < ne; hh++)
                 {
             NewGrid[ii][jj][hh] = oldg[ii][jj][hh]
                   - delta *(fp1[hh]- fp2[hh])
                   - delta *(fn1[hh]-  fn[hh]);
             if (NewGrid[ii][jj][kk]_MASS < 0.0 )
               {
                               cout << "upd:"
                                        << "[" << ii << "," << jj << "]="
                                        <<  endl;
                  cout  << "[" << ii+1  << "," << jj << "]="
                          << " " << oldg[ii+1][jj][kk]_MASS
                          << " " << oldg[ii+1][jj][kk]_MOMX
                          << " " << oldg[ii+1][jj][kk]_MOMY
                          << " " << oldg[ii+1][jj][kk]_ENER
                          << endl;
                  cout  << "[" << ii-1  << "," << jj << "]="
                          << " " << oldg[ii-1][jj][kk]_MASS
                          << " " << oldg[ii-1][jj][kk]_MOMX
                          << " " << oldg[ii-1][jj][kk]_MOMY
                          << " " << oldg[ii-1][jj][kk]_ENER
                          << endl;
                  cout  << "[" << ii  << "," << jj << "]="
                          << " " << NewGrid[ii][jj][kk]_MASS
                          << " " << NewGrid[ii][jj][kk]_MOMX
                          << " " << NewGrid[ii][jj][kk]_MOMY
                          << " " << NewGrid[ii][jj][kk]_ENER
                          << endl;
                                       exit(0);
               }

                   }
      */
#endif
      rho = NewGrid[ii][jj][kk]_MASS;
      double px;
      double py;
      double pz;
      double ke;
      px = NewGrid[ii][jj][kk]_MOMX;
      py = NewGrid[ii][jj][kk]_MOMY;
      pz = NewGrid[ii][jj][kk]_MOMZ;
      double energy = NewGrid[ii][jj][kk]_ENER;
      double bx = NewGrid[ii][jj][kk]_B_X;
      double by = NewGrid[ii][jj][kk]_B_Y;
      double bz = NewGrid[ii][jj][kk]_B_Z;

      rhoi = 1.0 / rho;
      double vx = px * rhoi;
      double vy = py * rhoi;
      double vz = pz * rhoi;
      double v2;
      v2 = vx * vx + vy * vy + vz * vz;
      double b2 = bx * bx + by * by + bz * bz;
      ke = 0.5 * rho * v2;
      double eint = energy - ke - 0.5 * b2;
      double pressure = eint * (gammag - 1);

      NewGrid = check_negative_pressure(NewGrid, oldg, ii, jj, hh, kk, ke, fx1, fx2, fy1, fy2, b2, pressure);

    }
  }
#ifdef STAGGER_MESH


  // Use resolved state to calculate electric field at cell faces
  // At this point we have all we need for the
  // emf at cell faces
  // They are simply -E3 = B2 x-flux
  // +E 3 = B1 y-flux
  // E 3 = u1 B2 - B2 u1
  // See Jim Stone's Athena source code notes
  // xFlux[i][j].getvar(6) i+0.5,j
  // yFlux[i][j].getvar(5) i,j+05


/*
  Array2D < double >emf (nx, ny);
  Array2D < double >Bxpos (nx, ny);
  Array2D < double >Bypos (nx, ny);
  Array2D < double >Bxneg (nx, ny);
  Array2D < double >Byneg (nx, ny);
*/



  multi_array<double, 2>  emf(extents[nx][ny]);
  multi_array<double, 2>  Bxpos(extents[nx][ny]);
  multi_array<double, 2>  Bypos(extents[nx][ny]);
  multi_array<double, 2>  Bxneg(extents[nx][ny]);
  multi_array<double, 2>  Byneg(extents[nx][ny]);


  double bx, by, bz, b2;

  // Determine emf at cell corners
  // Average emf onto cell corners
  for (int i = 1; i < nx - 1; ++i)
  {
    for (int j = 1; j < ny - 1; ++j)
      {
    emf[i][j] = 0.25 * (-xflux[i][j][0].array[6]
                - xflux[i][j - 1][0].array[6]
                + yflux[i][j][0].array[5]
                + yflux[i - 1][j][0].array[5]);
      }
      }


  // Update B field at cell edges

  double dtodx = dt/delta_x;
  for (int i = 2; i < nx - 2; ++i)
      {
    for (int j = 2; j < ny - 2; ++j)
      {
// Check divergence of resolved state
//
//
    double a = xResState[i + 1][j][0].array[5];
    double b = xResState[i][j][0].array[5];
    double c = yResState[i][j + 1][0].array[6];
    double d = yResState[i][j][0].array[6];

    if ( (a -b+c-d) !=0)
    {
        cout << "stag: Div Res State " << a-b+c-d  << " " ;
        cout << i << " " << j << endl;
    }

    Bxpos[i][j] = xResState[i + 1][j][0].array[5] - dtodx * (emf[i + 1][j] - emf[i + 1][j + 1]);
    Bxneg[i][j] = xResState[i][j][0].array[5] - dtodx * (emf[i][j] - emf[i][j + 1]);
    Bypos[i][j] = yResState[i][j + 1][0].array[6] + dtodx * (emf[i][j + 1] - emf[i + 1][j + 1]);
    Byneg[i][j] = yResState[i][j][0].array[6] + dtodx * (emf[i][j] - emf[i + 1][j]);

    a = -(emf[i + 1][j] - emf[i + 1][j + 1]);
    b = -(emf[i][j] - emf[i][j + 1]);
    c = (emf[i][j + 1] - emf[i + 1][j + 1]);
    d = (emf[i][j] - emf[i + 1][j]);


   if ( (a -b+c-d) !=0)
   {
      cout << "stag: Div Res Change " << a-b+c-d  << " " ;
      cout << i << " " << j << endl;
   }


#ifdef DEBUG_STAGGER
    cout
      << " " << Bxpos[i][j]
      << " " << Bxneg[i][j]
      << " " << Bypos[i][j] << " " << Byneg[i][j] << endl;
#endif /* DEBUG_STAGGER */
      }
      }


  // Average B field onto cell centres
  for (int i = 4; i < nx - 4; ++i)
      {
    for (int j = 4; j < ny - 4; ++j)
      {
    et = NewGrid[i][j][0].array[4];
    bx = NewGrid[i][j][0].array[5];
    by = NewGrid[i][j][0].array[6];
    bz = NewGrid[i][j][0].array[7];
    b2 = bx * bx + by * by + bz * bz;
    b2 = 0.5 * b2;
    et = et - b2;
    bx = NewGrid[i][j][0].array[5] = 0.5 * (Bxpos[i][j] + Bxneg[i][j]);
    by = NewGrid[i][j][0].array[6] = 0.5 * (Bypos[i][j] + Byneg[i][j]);

    NewGrid[i][j][0].array[4] = et + 0.5 * (bx * bx + by * by + bz * bz);

      }
      }


#endif /*STAGGER_MESH */

  return 0;
}
Array3D<zone> &Updater::check_negative_pressure(Array3D<zone> &NewGrid,
                                                const Array3D<zone> &oldg,
                                                int ii,
                                                int jj,
                                                int hh,
                                                int kk,
                                                double ke,
                                                const double *fx1,
                                                const double *fx2,
                                                const double *fy1,
                                                const double *fy2,
                                                double b2,
                                                double pressure) const {
  if (pressure < 0.0) {
    pressure = 0.001;
    NewGrid[ii][jj][kk]_ENER = ke + b2 + pressure / (gammag - 1);
  }
  if (pressure < 0.0) {
    cout << "Exiting on Negative pressure " << NewGrid[ii][jj][kk]
    _MASS << endl;
    cout << "hh fx2 fx1 fy2 fy1 " << endl;
    for (hh = 0; hh < ne; hh++) {

      cout << "upd: " << hh
           << " " << fx2[hh]
           << " " << fx1[hh]
           << " " << fy2[hh] << " " << fy1[hh] << endl;
    }
    cout << "upd:" << "[" << ii << "," << jj << "]=" << endl;
    cout << setiosflags(ios_base::fixed);
    cout << "[" << ii + 1 << "," << jj << "]="
         << " " << oldg[ii + 1][jj][kk]_MASS
         << " " << oldg[ii + 1][jj][kk]_MOMX
         << " " << oldg[ii + 1][jj][kk]_MOMY
         << " " << oldg[ii + 1][jj][kk]_MOMZ
         << " " << oldg[ii + 1][jj][kk]_ENER
         << " " << oldg[ii + 1][jj][kk]_B_X
         << " " << oldg[ii + 1][jj][kk]_B_Y
         << " " << oldg[ii + 1][jj][kk]_B_Z << endl;
    cout << "[" << ii - 1 << "," << jj << "]="
         << " " << oldg[ii - 1][jj][kk]_MASS
         << " " << oldg[ii - 1][jj][kk]_MOMX
         << " " << oldg[ii - 1][jj][kk]_MOMY
         << " " << oldg[ii - 1][jj][kk]_MOMZ
         << " " << oldg[ii - 1][jj][kk]_ENER
         << " " << oldg[ii - 1][jj][kk]_B_X
         << " " << oldg[ii - 1][jj][kk]_B_Y
         << " " << oldg[ii - 1][jj][kk]_B_Z << endl;
    cout << "[" << ii << "," << jj << "]="
         << " " << NewGrid[ii][jj][kk]_MASS
         << " " << NewGrid[ii][jj][kk]_MOMX
         << " " << NewGrid[ii][jj][kk]_MOMY
         << " " << NewGrid[ii][jj][kk]_MOMZ
         << " " << NewGrid[ii][jj][kk]_ENER
         << " " << NewGrid[ii][jj][kk]_B_X
         << " " << NewGrid[ii][jj][kk]_B_Y
         << " " << NewGrid[ii][jj][kk]_B_Z << endl;
    cout << "[" << ii << "," << jj << "]="
         << " " << oldg[ii][jj][kk]_MASS
         << " " << oldg[ii][jj][kk]_MOMX
         << " " << oldg[ii][jj][kk]_MOMY
         << " " << oldg[ii][jj][kk]_MOMZ
         << " " << oldg[ii][jj][kk]_ENER
         << " " << oldg[ii][jj][kk]_B_X
         << " " << oldg[ii][jj][kk]_B_Y
         << " " << oldg[ii][jj][kk]_B_Z << endl;
    cout << "[" << ii << "," << jj + 1 << "]="
         << " " << oldg[ii][jj + 1][kk]_MASS
         << " " << oldg[ii][jj + 1][kk]_MOMX
         << " " << oldg[ii][jj + 1][kk]_MOMY
         << " " << oldg[ii][jj + 1][kk]_MOMZ
         << " " << oldg[ii][jj + 1][kk]_ENER
         << " " << oldg[ii][jj + 1][kk]_B_X
         << " " << oldg[ii][jj + 1][kk]_B_Y
         << " " << oldg[ii][jj + 1][kk]_B_Z << endl;
    cout << "[" << ii << "," << jj - 1 << "]="
         << " " << oldg[ii][jj - 1][kk]_MASS
         << " " << oldg[ii][jj - 1][kk]_MOMX
         << " " << oldg[ii][jj - 1][kk]_MOMY
         << " " << oldg[ii][jj - 1][kk]_MOMZ
         << " " << oldg[ii][jj - 1][kk]_ENER
         << " " << oldg[ii][jj - 1][kk]_B_X
         << " " << oldg[ii][jj - 1][kk]_B_Y
         << " " << oldg[ii][jj - 1][kk]_B_Z << endl;
  }
  return NewGrid;
}

int
monopole(double *source_term, const double *old, double divb) {
  double bx, by, bz, vdotb;
  double vx, vy, vz;
  double rho, rhoi;
  rho = old[0];
  rhoi = 1 / rho;
  vx = old[1] * rhoi;
  vy = old[2] * rhoi;
  vz = old[3] * rhoi;

  bx = old[5];
  by = old[6];
  bz = old[7];

  vdotb = bx * vx + by * vy + vz * bz;

  source_term[0] = 0;
  source_term[1] = (-bx) * divb;
  source_term[2] = (-by) * divb;
  source_term[3] = (-bz) * divb;
  source_term[4] = (-vdotb) * divb;
  source_term[5] = (-vx) * divb;
  source_term[6] = (-vy) * divb;
  source_term[7] = (-vz) * divb;
#ifdef DEBUG_DIVB
  if (divb != 0)
    {
      cout << divb << endl;
    }
#endif /*DEBUG_DIVB */

  return 0;
}
