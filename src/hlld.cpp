#include "riemann.h"
#define DEBUG_HLLD
#define DEBUG_EV
#undef DEBUG_HLLD
int
hlld(const double *leftstate,
     const double *rightstate,
     double *fhlld,
     double *Res_state,
     int time_step, double *max_speed, int idir) {

  ofstream outFile;
  char outfilename[50] = "output/hlld.log";

  double rl = leftstate[0];
  double ul = leftstate[1];
  double vl = leftstate[2];
  double wl = leftstate[3];
  double pl = leftstate[4];
  double el = leftstate[4];
  double Bul = leftstate[5];
  double Bvl = leftstate[6];
  double Bwl = leftstate[7];

  double rr = rightstate[0];
  double ur = rightstate[1];
  double vr = rightstate[2];
  double wr = rightstate[3];
  double pr = rightstate[4];
  double er = rightstate[4];
  double Bx = rightstate[5];
  double Bvr = rightstate[6];
  double Bwr = rightstate[7];
  double calfven = 0;
  double calfven2 = 0;
  double cfast2 = 0;
  double bsquared = 0;
  double gammam1 = gammag - 1;

  double lstate[7];
  double rstate[7];
  double fl[8];
  double fr[8];
  double Ul[8];
  double UlStar[8];
  double Ulss[8];
  double Ur[8];
  double UrStar[8];
  double Urss[8];

  double rho = 0;
  double rhoi = 0;
  double bu = 0;
  double bv = 0;
  double bw = 0;
  double p = 0;
  double csound2 = 0;
  double term = 0;
  double a_Star2 = 0;
  double v2 = 0;
  double S_l, S_m, S_r, cfast_r, cfast_l;

#ifdef DEBUG_HLLD
  outFile.open (outfilename, ofstream::app);
  if (outFile.fail())
    {
      cerr << "Unable to open file" << endl;
    }
#endif /* DEBUG_HLLD */

  double B2l = Bx * Bx + Bvl * Bvl + Bwl * Bwl;
  double B2r = Bx * Bx + Bvr * Bvr + Bwr * Bwr;
  /* Convert conserved to primitives */
  v2 = ul * ul + vl * vl + wl * wl;
  bsquared = Bul * Bul + Bvl * Bvl + Bwl * Bwl;
  el = pl / gammam1 + (double) 0.5 * (rl * v2) + 0.5 * bsquared;
  if (pl < 0) {
    cout << "hlld:Negative pressure in roe solver!" << pl << endl;
    return (1);
  }

  v2 = ur * ur + vr * vr + wr * wr;
  bsquared = Bx * Bx + Bvr * Bvr + Bwr * Bwr;
  er = pr / gammam1 + (double) 0.5 * (rr * v2) + 0.5 * bsquared;
  if (pr < 0) {
    cout << "hlld:Negative pressure in roe solver!" << pr << endl;
    return (1);
  }

  lstate[0] = rl;
  lstate[1] = ul;
  lstate[2] = vl;
  lstate[3] = wl;
  lstate[4] = pl;
  lstate[5] = Bvl;
  lstate[6] = Bwl;

  rstate[0] = rr;
  rstate[1] = ur;
  rstate[2] = vr;
  rstate[3] = wr;
  rstate[4] = pr;
  rstate[5] = Bvr;
  rstate[6] = Bwr;



/* Compute S_l, S_r and S_m, s_lStar and s_mStar */

  /* Compute fast and slow speeds */
  rho = rl;
  rhoi = 1 / rho;
  bu = Bx;
  bv = lstate[5];
  bw = lstate[6];
  bsquared = (bu * bu + bv * bv + bw * bw) * rhoi;
  p = pl;
  calfven2 = fabs(bu * bu * rhoi);
  csound2 = gammag * pl * rhoi;
  a_Star2 = csound2 + bsquared;
  term = sqrt(a_Star2 * a_Star2 - 4.0 * csound2 * calfven2);
  cfast2 = 0.5 * (a_Star2 + term);
  cfast_l = sqrt(cfast2);

  rho = rr;
  rhoi = 1 / rho;
  bu = Bx;
  bv = rstate[5];
  bw = rstate[6];
  bsquared = (bu * bu + bv * bv + bw * bw) * rhoi;
  p = pr;
  calfven2 = fabs(bu * bu * rhoi);
  csound2 = gammag * pr * rhoi;
  a_Star2 = csound2 + bsquared;
  term = sqrt(a_Star2 * a_Star2 - 4.0 * csound2 * calfven2);
  cfast2 = 0.5 * (a_Star2 + term);
  cfast_r = sqrt(cfast2);

  S_l = (min)(lstate[1], rstate[1]) - max(cfast_l, cfast_r);
  S_r = (max)(lstate[1], rstate[1]) + max(cfast_l, cfast_r);

  double vldotbl = ul * Bx + vl * Bvl + wl * Bwl;
  double vrdotbr = ur * Bx + vr * Bvr + wr * Bwr;
  double ptl = pl + 0.5 * B2l;
  double ptr = pr + 0.5 * B2r;
  /* Build fl and fr */
  fl[0] = rl * ul;
  fl[1] = rl * ul * ul + ptl - Bx * Bx;
  fl[2] = rl * ul * vl - Bx * Bvl;
  fl[3] = rl * ul * wl - Bx * Bwl;
  fl[4] = (el + ptl) * ul - Bx * vldotbl;
  fl[5] = 0;
  fl[6] = Bvl * ul - Bx * vl;
  fl[7] = Bwl * ul - Bx * wl;

  fr[0] = rr * ur;
  fr[1] = rr * ur * ur + ptr - Bx * Bx;
  fr[2] = rr * ur * vr - Bx * Bvr;
  fr[3] = rr * ur * wr - Bx * Bwr;
  fr[4] = (er + ptr) * ur - Bx * vrdotbr;
  fr[5] = 0;
  fr[6] = Bvr * ur - Bx * vr;
  fr[7] = Bwr * ur - Bx * wr;

  if (S_l > 0) {
    /* Build ul */
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fl[q];
    }
    return 0;
  }

  if (S_r < 0) {
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fr[q];
    }
    return 0;
  }

#ifdef DEBUG_HLLD
  //if (ul != ur)
  if (rl != rr)
    {
      outFile << endl;
      outFile << "Time Step " << time_step << endl;
      outFile << "Left and Right States -----" << endl;
      outFile << "rl " << rl
    << " ul " << ul
    << " vl " << vl
    << " wl " << wl
    << " pl " << pl
    << " Bx " << Bx << " Bvl " << Bvl << " Bwl " << Bwl << endl;
      outFile << "rr " << rr
    << " ur " << ur
    << " vr " << vr
    << " wr " << wr
    << " pr " << pr
    << " Bx " << Bx << " Bvr " << Bvr << " Bwr " << Bwr << endl;
      outFile << endl;
      outFile << "Fast, Slow , Alfven speeds-----" << endl;
      outFile
    << " cfast " << cfast_l
    << endl;
      outFile
    << " cfast " << cfast_r
    << endl;
      outFile << endl;
    }
#endif /* DEBUG_HLLD */
  double A_r = (S_r - ur) * rr;
  double A_l = (S_l - ul) * rl;

  S_m = A_r * ur - A_l * ul - ptr + ptl;
  S_m = S_m / (A_r - A_l);
/* Compute lStar and rStar states */

  double ulStar = S_m;
  double urStar = S_m;
  double urss = S_m;
  double ulss = S_m;

  double ptStar = A_r * ptl - A_l * ptr + A_l * A_r * (ur - ul);
  ptStar = ptStar / (A_r - A_l);

  double rlStar = A_l / (S_l - S_m);
  double rrStar = A_r / (S_r - S_m);

  double vlStar = vl - Bx * Bvl * (S_m - ul) / (A_l * (S_l - S_m) - Bx * Bx);
  double vrStar = vr - Bx * Bvr * (S_m - ur) / (A_r * (S_r - S_m) - Bx * Bx);

  double BvlStar = Bvl * (rl * pow((S_l - ul), 2.0) - Bx * Bx) / (A_l * (S_l - S_m) - Bx * Bx);
  double BvrStar = Bvr * (rr * pow((S_r - ur), 2.0) - Bx * Bx) / (A_r * (S_r - S_m) - Bx * Bx);

  double wlStar = wl - Bx * Bwl * (S_m - ul) / (A_l * (S_l - S_m) - Bx * Bx);
  double wrStar = wr - Bx * Bwr * (S_m - ur) / (A_r * (S_r - S_m) - Bx * Bx);

  double BwlStar = Bwl * (rl * pow((S_l - ul), 2.0) - Bx * Bx) / (A_l * (S_l - S_m) - Bx * Bx);
  double BwrStar = Bwr * (rr * pow((S_r - ur), 2.0) - Bx * Bx) / (A_r * (S_r - S_m) - Bx * Bx);

  double vldotblStar = ulStar * Bx + vlStar * BvlStar + wlStar * BwlStar;
  double vrdotbrStar = urStar * Bx + vrStar * BvrStar + wrStar * BwrStar;
  //double B2lStar=Bx*Bx + BvlStar*BvlStar +BwlStar*BwlStar ;
  //double B2rStar=Bx*Bx + BvrStar*BvrStar +BwrStar*BwrStar ;

  double elStar = ((S_l - ul) * el - ptl * ul + ptStar * S_m + Bx * (vldotbl - vldotblStar)) / (S_l - S_m);
  double erStar = ((S_r - ur) * er - ptr * ur + ptStar * S_m + Bx * (vrdotbr - vrdotbrStar)) / (S_r - S_m);

/* Compute lStar2 and rStar2 states */

  double sqrtrlStar = sqrt(rlStar);
  double sqrtrrStar = sqrt(rrStar);

  double vss = (sqrtrlStar * vlStar + sqrtrrStar * vrStar + (BvrStar - BvlStar) * sgn(Bx)) / (sqrtrlStar + sqrtrrStar);
  double wss = (sqrtrlStar * wlStar + sqrtrrStar * wrStar + (BwrStar - BwlStar) * sgn(Bx)) / (sqrtrlStar + sqrtrrStar);
  double Bvss = (sqrtrlStar * BvlStar + sqrtrrStar * BvrStar + (sqrtrlStar * sqrtrrStar) * (vrStar - vlStar) * sgn(Bx))
      / (sqrtrlStar + sqrtrrStar);
  double Bwss = (sqrtrlStar * BwlStar + sqrtrrStar * BwrStar + (sqrtrlStar * sqrtrrStar) * (wrStar - wlStar) * sgn(Bx))
      / (sqrtrlStar + sqrtrrStar);

  double vdotbss = ulss * Bx + vss * Bvss + wss * Bwss;

  double elss = elStar - sqrtrlStar * (vldotblStar - vdotbss) * sgn(Bx);
  double erss = erStar + sqrtrrStar * (vrdotbrStar - vdotbss) * sgn(Bx);
/* Set solution fluxes equal to appropriate state based on evaluation of S_l etc
 * */
  double S_lStar = S_m - fabs(Bx) / sqrtrlStar;
  double S_rStar = S_m + fabs(Bx) / sqrtrrStar;


  /* Build ul and ulStar ur urStar urss ulss */
  Ul[0] = rl;
  Ul[1] = rl * ul;
  Ul[2] = rl * vl;
  Ul[3] = rl * wl;
  Ul[4] = el;
  Ul[5] = Bx;
  Ul[6] = Bvl;
  Ul[7] = Bwl;

  Ur[0] = rr;
  Ur[1] = rr * ur;
  Ur[2] = rr * vr;
  Ur[3] = rr * wr;
  Ur[4] = er;
  Ur[5] = Bx;
  Ur[6] = Bvr;
  Ur[7] = Bwr;

  UlStar[0] = rlStar;
  UlStar[1] = rlStar * ulStar;
  UlStar[2] = rlStar * vlStar;
  UlStar[3] = rlStar * wlStar;
  UlStar[4] = elStar;
  UlStar[5] = Bx;
  UlStar[6] = BvlStar;
  UlStar[7] = BwlStar;

  UrStar[0] = rrStar;
  UrStar[1] = rrStar * urStar;
  UrStar[2] = rrStar * vrStar;
  UrStar[3] = rrStar * wrStar;
  UrStar[4] = erStar;
  UrStar[5] = Bx;
  UrStar[6] = BvrStar;
  UrStar[7] = BwrStar;

  Ulss[0] = rlStar;
  Ulss[1] = rlStar * ulss;
  Ulss[2] = rlStar * vss;
  Ulss[3] = rlStar * wss;
  Ulss[4] = elss;
  Ulss[5] = Bx;
  Ulss[6] = Bvss;
  Ulss[7] = Bwss;

  Urss[0] = rrStar;
  Urss[1] = rrStar * urss;
  Urss[2] = rrStar * vss;
  Urss[3] = rrStar * wss;
  Urss[4] = erss;
  Urss[5] = Bx;
  Urss[6] = Bvss;
  Urss[7] = Bwss;




  /*
if ( fabs(Bx) <= 1e-7)
{
if ( S_l <= 0  && S_lStar >=0 )
{
  // L Star
  for (int q=0; q<8; q++)
  fhlld[q]=fl[q]+S_l*UlStar[q] -S_l*Ul[q];
}else if ( S_rStar  <=0 && S_r >= 0)
{
  // R Star
  for (int q=0; q<8; q++)
  fhlld[q]=fr[q]+S_r*UrStar[q] -S_r*Ur[q];
}
return 0;

}
else {

*/
  if (S_l <= 0 && S_lStar >= 0) {
    // L Star
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fl[q] + S_l * UlStar[q] - S_l * Ul[q];
    }
  } else if (S_rStar <= 0 && S_r >= 0) {
    // R Star
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fr[q] + S_r * UrStar[q] - S_r * Ur[q];
    }
  } else if (S_lStar <= 0 && S_m >= 0) {
    // L ss
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fl[q] + S_lStar * Ulss[q] - (S_lStar - S_l) * UlStar[q] - S_l * Ul[q];
    }
  } else if (S_m <= 0 && S_rStar >= 0) {
    // R ss
    for (int q = 0; q < 8; q++) {
      fhlld[q] = fr[q] + S_rStar * Urss[q] - (S_rStar - S_r) * UrStar[q] - S_r * Ur[q];
    }
  }


  //}







#ifdef DEBUG_HLLD
  //if (rl != rr)
  if (rl != rr)
    {
      outFile << "Flux Vector -----" << endl;
      outFile << fhlld[0]
    << " " << fhlld[1]
    << " " << fhlld[2]
    << " " << fhlld[3]
    << " " << fhlld[4]
    << " " << fhlld[5]
    << " " << fhlld[6] << " " << fhlld[7] << endl;
      outFile << "##############" << endl;
      outFile << endl;
    }
  for (int ii = 0; ii < 8; ii++)
    {
      if (fabs (fhlld[ii]) < -1e10)
    {
      fhlld[ii] = 0;
    }
    }
#endif /* DEBUG_HLLD */

#ifdef DEBUG_HLLD
  outFile.close ();
#endif /* DEBUG_HLLD */
  return 0;
}
