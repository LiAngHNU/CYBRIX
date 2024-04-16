#include "vs_def_api.h"

VS_API void simple_sti(
  void(__cdecl *message)(
  char   *msg),
  int    *iswtch,
  int    *jobflg,
  int    *idtyre,
  double *time,
  double  dis[3],
  double  tramat[9],  // 9-element vector passed as Fortran 3x3 matrix
  double *angtwc,
  double  vel[3],
  double  omega[3],
  double *omegar,
  int    *ndeqvr,
  double deqvar[],
  int    *ntypar,
  double  typarr[],
  int    *nchtds,
  char    chtdst[],
  int    *len_chtdst,
  void(__cdecl *road)(
  int    *idtyre,
  double dis[2],
  double *z,
  double dz[2],
  double *mu,
  int *ierr),
  int    *idroad,
  int    *nropar,
  double  ropar[],
  int    *nchrds,
  char    chrdst[],
  int    *len_chrdst,
  double  force[3],
  double  torque[3],
  double *deqini,
  double *deqder,
  char    tyrmod[256],
  int    *len_tyrmod,
  int    *nvars,
  double  varinf[],
  int    *nwork,
  double  wrkarr[],
  int    *niwork,
  int     iwrkar[],
  int    *ierr);

static void sCalcTireCtcForces(int id, vs_real w);

void get_ctc_xy(int id, vs_real xw0, vs_real yw0, vs_real zw0,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real r_old, vs_real dzdx_old, vs_real dzdy_old,
  vs_real *xgnd_est, vs_real *ygnd_est);

void get_ctc_kin(int id, vs_real vwx, vs_real vwy, vs_real vwz,
  vs_real avwx, vs_real avwy, vs_real avwz,
  vs_real dzdx_1, vs_real dzdy_1, vs_real r_old_1,
  vs_real *gamma_1, vs_real *rnew_1,
  vs_real *vctcx_1, vs_real *vctcy_1,
  vs_real *xgnd_1, vs_real *ygnd_1, vs_real *zgnd_1,
  vs_real *pitch_gnd, vs_real *roll_gnd);

void ctc_transform(int id, vs_real rnew_1, vs_real dzdx_1, vs_real dzdy_1,
  vs_real fxctc_1, vs_real fyctc_1, vs_real fzctc_1,
  vs_real mxctc, vs_real myctc, vs_real mzctc,
  vs_real *fxw0, vs_real *fyw0, vs_real *fzw0,
  vs_real *mxw0, vs_real *myw0, vs_real *mzw0);
