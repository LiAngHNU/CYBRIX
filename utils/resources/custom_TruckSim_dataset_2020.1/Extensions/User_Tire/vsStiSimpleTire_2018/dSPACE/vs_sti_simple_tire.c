/* This file contains the User Simple external tire module integrated with VS solvers. 

   Copyright 1997 - 2015. Mechanical Simulation Corporation. All rights reserved.

   09-23-15 Y. Watanabe: Created a simple external tire module.
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifndef _VS_RT
#include <windows.h>
#endif

#include "vs_deftypes.h"
#include "vs_sti_simple_tire.h"

#ifndef FALSE
  #define TRUE 1
  #define FALSE 0
#endif

#ifndef max
  #define max(a, b)   (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
  #define min(a, b)   (((a) < (b)) ? (a) : (b))
#endif

// structure for USER STI parameter sets for a tire
typedef struct {
  vs_real typarr[27], infVar[100],
          stateKappa, stateKappaDeriv, stateTana, stateTanaDeriv,
          dis[3], angle[3], vel[3], omega[3], force[3], torque[3],
          angtwc, omegar;
  vs_real c[6], s[6], z[68], q[6], r, dzdx, dzdy, zgnd, mu, comp,
          r_free, kt, cfx, cfy, cmx, cmz,
          vlow_drift, vlow_slip, lrelax_x, lrelax_y,
          alpha, kappa, vx, vy, gamma, fx, fy, fz, mx, my, mz;
  int     is_first, ntypar, nvars, iswtch, idtyre, ierr, idroad;
  char    tyrmod[256], chrdst[256], chtdst[256], chsdst[256], road[256],
          descVar[100][33], keyVar[100][15],
          descKappa[33], keyKappa[20], descTana[33], keyTana[20];
} UserPars;

static UserPars **sUserTire;
static int sUserTireN;
static vs_real sTime;
static int *sIside, *sAxleMap, *sUnitsN, *sIunit, *sIaxle, *sItire;

static int sDualsArePossible = FALSE; // generate labels for duals?
#ifndef _VS_RT
static FILE *sUserLogFile;
#endif

#define READ_BUFFER_LEN 200

VS_STI_WRITE_TO_LOGFILE_FUNC gWrite_to_logfile;
VS_API void vs_sti_write_to_logfile(VS_STI_WRITE_TO_LOGFILE_FUNC function_name) {
  gWrite_to_logfile = function_name;
}
VS_STI_GET_VAR_PTR_INT_FUNC gGet_var_ptr_int;
VS_API void vs_sti_get_var_ptr_int(VS_STI_GET_VAR_PTR_INT_FUNC function_name) {
  gGet_var_ptr_int = function_name;
}
VS_STI_GET_VAR_PTR_FUNC gGet_var_ptr;
VS_API void vs_sti_get_var_ptr(VS_STI_GET_VAR_PTR_FUNC function_name) {
  gGet_var_ptr = function_name;
}
VS_STI_WRITEI_FUNC gWritei;
VS_API void vs_sti_writei(VS_STI_WRITEI_FUNC function_name) {
  gWritei = function_name;
}
VS_STI_WRITE_ECHO_LINE_FUNC gWrite_echo_line;
VS_API void vs_sti_write_echo_line(VS_STI_WRITE_ECHO_LINE_FUNC function_name) {
  gWrite_echo_line = function_name;
}
VS_STI_SET_SYM_ATTRIBUTE_FUNC gSet_sym_attribute;
VS_API void vs_sti_set_sym_attribute(VS_STI_SET_SYM_ATTRIBUTE_FUNC function_name) {
  gSet_sym_attribute = function_name;
}
VS_STI_PRINTF_ERROR_FUNC gPrintf_error;
VS_API void vs_sti_printf_error(VS_STI_PRINTF_ERROR_FUNC function_name) {
  gPrintf_error = function_name;
}
VS_STI_INSTALL_FREE_FUNCTION_FUNC gInstall_free_function;
VS_API void vs_sti_install_free_function(VS_STI_INSTALL_FREE_FUNCTION_FUNC function_name) {
  gInstall_free_function = function_name;
}
VS_STI_DEFINE_SV_FUNC gDefine_sv;
VS_API void vs_sti_define_sv(VS_STI_DEFINE_SV_FUNC function_name) {
  gDefine_sv = function_name;
}
VS_STI_DEFINE_OUT_FUNC gDefine_out;
VS_API void vs_sti_define_out(VS_STI_DEFINE_OUT_FUNC function_name) {
  gDefine_out = function_name;
}
VS_STI_HAVE_KEYWORD_FUNC gHave_keyword_in_database;
VS_API void vs_sti_have_keyword_in_database(VS_STI_HAVE_KEYWORD_FUNC function_name) {
  gHave_keyword_in_database = function_name;
}
VS_STI_GET_FILEBASE_NAME_FUNC gGet_Filebase_name;
VS_API void vs_sti_get_filebase_name(VS_STI_GET_FILEBASE_NAME_FUNC function_name) {
  gGet_Filebase_name = function_name;
}

VS_STI_TYPARR_FUNC gTyparr;
VS_API void vs_sti_typarr(VS_STI_TYPARR_FUNC function_name) {
  gTyparr = function_name;
}

VS_STI_GET_ALLTIRESALIKE_FUNC gGetAllTiresAlike;
VS_API void vs_sti_GetAllTiresAlike(VS_STI_GET_ALLTIRESALIKE_FUNC function_name) {
  gGetAllTiresAlike = function_name;
}

VS_STI_TIRES_SET_PARS_FUNC gTiresSetPars;
VS_API void vs_sti_TiresSetPars(VS_STI_TIRES_SET_PARS_FUNC function_name) {
  gTiresSetPars = function_name;
}
VS_STI_PRINT_ECHO_FUNC gPrintEcho;
VS_API void vs_sti_print_echo(VS_STI_PRINT_ECHO_FUNC function_name) {
  gPrintEcho = function_name;
}

VS_STI_GET_ROAD_CONTACT_EXT_FUNC gGet_road_contact_ext;
VS_API void vs_sti_get_road_contact_ext(VS_STI_GET_ROAD_CONTACT_EXT_FUNC function_name) {
  gGet_road_contact_ext = function_name;
}

VS_STI_DATABASE_DIR_FUNC gDatabaseDir;
VS_API void vs_sti_DatabaseDir(VS_STI_DATABASE_DIR_FUNC function_name) {
  gDatabaseDir = function_name;
}

VS_STI_SET_TYPARR_VISIBLE_FUNC gSetTyparrVisible;
VS_API void vs_sti_set_typarr_visible(VS_STI_SET_TYPARR_VISIBLE_FUNC function_name) {
  gSetTyparrVisible = function_name;
}

// Return a string with a wheel suffix(e.g., R2).
static char *sOut_suffix_wheel(int itab) {
  static char str[6], naxle;
  itab++;

  // Even (Right) or odd (Left)?
  naxle = (int)ceil((vs_real)itab / 2.0);
  if (2 * (itab / 2) == itab) sprintf(str, "R%d", naxle);
  else sprintf(str, "L%d", naxle);

  return str;
}

static char *sOut_suffix_tire(int itab, int duals) {
  int isRightSide, isOuter, naxle;
  static char str[8];

  if (!duals)
    return sOut_suffix_wheel(itab);

  itab++;

  // Determine axle number, side, inner/outer
  isOuter = (2 * (itab / 2) == itab);
  naxle = (int)ceil((vs_real)itab / 4.0);
  isRightSide = !((vs_real)naxle - (vs_real)itab / 4.0 > 0.3);

  sprintf(str, "%s%d%s", isRightSide ? "R" : "L", naxle, isOuter ? "O" : "I");
  return str;
}

// Return a string with an axle label, given 0-indexed itab and system parameters
static char *sTab_label_axle(int itab) {
  int iu;
  static char str[20];
  itab++;
  for (iu = 0; iu < *sUnitsN; iu++) {
    if (itab > sAxleMap[iu])
      itab -= sAxleMap[iu];
    else {
      if (*sUnitsN > 1)
        sprintf(str, "unit %d, axle %d", iu + 1, itab);
      else sprintf(str, "axle %d", itab);
      return str;
    }
  }
  return NULL;
}

// Return a string with a wheel label, given 0-indexed itab
static char *sTab_label_wheel(int itab) {
  static char str[30];
  itab++;

  // Even (Right) or odd (Left)?
  if (2 * (itab / 2) == itab)
    sprintf(str, "%s, right side", sTab_label_axle(itab / 2 - 1));
  else sprintf(str, "%s, left side", sTab_label_axle((itab + 1) / 2 - 1));

  return str;
}

// Return a string with a tire label, given 0-indexed itab
static char *sTab_label_tire(int itab) {
  int num_glb_axle;
  static char str[40];
  itab++;

  // Determine global axle number.
  num_glb_axle = (int)ceil((vs_real)itab / 4.0);

  sprintf(str, "%s, %s, %s", sTab_label_axle(num_glb_axle - 1),
    (vs_real)num_glb_axle - (vs_real)itab / 4.0 > 0.3 ? "left" : "right",
    2 * (itab / 2) == itab ? "outer" : "inner");
  return str;
}

// calculate one-indexed ITAB for an axle using system parameters
static int sGet_itab_axle(void) {
  int i, offset = 0;

#ifdef _BIKESIM
  return *sIaxle;
#endif

  if (*sIunit < 1 || *sIunit > *sUnitsN ||
    *sIaxle < 1 || *sIaxle > sAxleMap[*sIunit - 1])
    return -1;

  for (i = 0; i < *sIunit - 1; i++)
    offset += sAxleMap[i];

  return offset + *sIaxle;
}

// calculate one-indexed ITAB for a tire using system parameters
static int sGet_itab_tire(void) {
  int itab;

  itab = sGet_itab_axle();

  if (itab < 0 || *sIside < 1 || *sIside > 2) return -1;

  else return (itab - 1) * 4 + (*sIside - 1) * 2 + *sItire;
}

// calculate one-indexed ITAB for a wheel using system parameters
static int sGet_itab_wheel(void) {
  int itab;

  itab = sGet_itab_axle();

  if (itab < 0 || *sIside < 1 || *sIside > 2) return -1;

  else return 2 * (itab - 1) + *sIside;
}

// Echo IAXLE and IUNIT given  0-indexed itab and system parameters
static void sEcho_axle(FILE *ifile, int itab) {
  int iu;
  itab++;
  for (iu = 0; iu < *sUnitsN; iu++) {
    if (itab > sAxleMap[iu])
      itab -= sAxleMap[iu];
    else {
      if (*sUnitsN > 1) gPrintEcho(ifile, "IUNIT %2d\n", iu + 1);
      gPrintEcho(ifile, "IAXLE %2d\n", itab);
      return;
    }
  }
}

// Echo IUNIT, IAXLE, ISIDE, and ITIRE given 0-index itab and system parameters
static void sEcho_tire(FILE *ifile, int itab) {
  int iside, itire, num_glb_axle;
  itab++;

  // Determine itire: even (outer -> 2) or odd (inner ->1).
  if (2 * (itab / 2) == itab) // even
    itire = 2;
  else
    itire = 1;

  // Determine global axle number.
  num_glb_axle = (int)ceil((vs_real)itab / 4.0);

  // Determine iside: (left -> 1) or (right -> 2).
  if ((vs_real)num_glb_axle - (vs_real)itab / 4.0 > 0.3)
    iside = 1;
  else
    iside = 2;

  sEcho_axle(ifile, num_glb_axle - 1);
  gPrintEcho(ifile, "ISIDE %2d\n", iside);
  gPrintEcho(ifile, "ITIRE %2d\n", itire);
}

// Echo IUNIT, IAXLE, and ISIDE given 0-index itab and system parameters
static void sEcho_wheel(FILE *ifile, int itab) {
  int iside;
  itab++;

  // Even (Right) or odd (Left)?
  if (2 * (itab / 2) == itab) { // even
    iside = 2;
    sEcho_axle(ifile, itab / 2 - 1);
  }
  else {
    iside = 1;
    sEcho_axle(ifile, (itab + 1) / 2 - 1);
  }

  gPrintEcho(ifile, "ISIDE %2d\n", iside);
}

// Capitalize string
char *sString_to_upper(char *s) {
  char   *p = s;
  while (*p) {
    *p = (char)toupper(*p);
    p++;
  }
  return s;
}

void vs_sti_calc(int itire, vs_real time,
  vs_real xw0, vs_real yw0, vs_real zw0,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real vwx, vs_real vwy, vs_real vwz,
  vs_real avwx, vs_real avwy, vs_real avwz,
  vs_real rot, vs_real w,
  vs_real *fxw0, vs_real *fyw0, vs_real *fzw0,
  vs_real *mxw0, vs_real *myw0, vs_real *mzw0,
  vs_real **infVar) {
  
  vs_real *userVar;
  CopyWcVars(itire, xw0, yw0, zw0, yaw, pitch, roll, vwx, vwy, vwz, avwx,
    avwy, avwz, rot, w, time);
  Calc(itire, fxw0, fyw0, fzw0, mxw0, myw0, mzw0, &userVar);
  *infVar = userVar;  
}

void vs_sti_calc2(int iwheel, vs_real ldual, vs_real time,
  vs_real xw0, vs_real yw0, vs_real zw0,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real vwx, vs_real vwy, vs_real vwz,
  vs_real avwx, vs_real avwy, vs_real avwz,
  vs_real rot, vs_real w,
  vs_real *fxw0, vs_real *fyw0, vs_real *fzw0,
  vs_real *mxw0, vs_real *myw0, vs_real *mzw0,
  vs_real **infVar, vs_real **infVar2) {

  const int itire = 2 * (iwheel - 1) + 1;
  const vs_real ldual_half = 0.5*ldual;
  const int itire2 = itire + 1;
  vs_real fx2, fy2, fz2, mx2, my2, mz2;
  vs_real *userVar, *userVar2;

  CopyWcVars2(itire, xw0, yw0, zw0, yaw, pitch, roll, vwx, vwy, vwz,
    avwx, avwy, avwz, rot, w, time, 0.5*ldual);
  Calc(itire, fxw0, fyw0, fzw0, mxw0, myw0, mzw0, &userVar);
  Calc(itire2, &fx2, &fy2, &fz2, &mx2, &my2, &mz2, &userVar2);

  *infVar = userVar;
  *infVar2 = userVar2;

  // combine results from two tires
  *mzw0 += mz2 + (*fxw0 - fx2)*ldual_half;
  *mxw0 += mx2 + (fz2 - *fzw0)*ldual_half;
  *myw0 += my2;
  *fxw0 += fx2;
  *fyw0 += fy2;
  *fzw0 += fz2;
}

void vs_sti_install(int nTires, int duals) {
  Install(nTires, duals);
  Setdef();
}

void sMessage(char* msg) {
#ifndef _VS_RT
  fprintf(sUserLogFile, "user/STI: %s\n", msg);
#endif
}

void vscUserErrorFourTires(int id) {
  if (gGetAllTiresAlike()) {
    gPrintf_error("the linked Tire dataset (Tire ID #%d) specifies a TRD file. "
      "The pull-down control on the Vehicle Assembly screen "
      "must specify 'All four tires alike' when the linked datset "
      "specifies a TRD file. When linking to different tires, "
      "the datasets should specify COF files. See Technical Memo: "
      "'CarSim with User Tire Model' for more information.", id);
  }
}

// call the User STI function; usersti is declared in user_sti.h
static void sTireCalc (UserPars *mp, int *job, const char *suffix) {
  vs_real yaw = 0, pitch = 0, roll = 0, x_gnd, y_gnd, pgnd, rgnd, dummy;
  static char str[READ_BUFFER_LEN];

  yaw = mp->angle[0];  
  pitch = mp->angle[1];
  roll = mp->angle[2];

  switch (*job) {
  case 0: case 5: {
    get_ctc_xy(mp->idtyre, mp->dis[0], mp->dis[1], mp->dis[2], yaw, pitch, roll, mp->r, mp->dzdx,
      mp->dzdy, &x_gnd, &y_gnd);

    if (!*job){
      gGet_road_contact_ext(y_gnd, x_gnd, mp->idtyre, &mp->zgnd, &mp->dzdy, &mp->dzdx, &dummy, &mp->mu);
    }

    get_ctc_kin(mp->idtyre, mp->vel[0], mp->vel[1], mp->vel[2], mp->omega[0], mp->omega[1], mp->omega[2], 
      mp->dzdx, mp->dzdy, mp->r, &mp->gamma, &mp->r, &mp->vx, &mp->vy, 
      &x_gnd, &y_gnd, &mp->zgnd, &pgnd, &rgnd);

    sCalcTireCtcForces(mp->idtyre, mp->omegar);

    ctc_transform(mp->idtyre, mp->r, mp->dzdx, mp->dzdy, mp->fx, mp->fy,
      mp->fz, mp->mx, mp->my, mp->mz,
      &mp->force[0], &mp->force[1], &mp->force[2], &mp->torque[0], &mp->torque[1], &mp->torque[2]);

    mp->infVar[0] = mp->fx;
    mp->infVar[1] = mp->fy;
    mp->infVar[2] = mp->fz;
    mp->infVar[3] = mp->mx;
    mp->infVar[4] = mp->my;
    mp->infVar[5] = mp->mz;
    mp->infVar[6] = mp->infVar[50] = mp->alpha;
    mp->infVar[7] = mp->infVar[51] = mp->kappa;
    mp->infVar[8] = mp->gamma;
    mp->infVar[16] = mp->mu;
    mp->infVar[17] = mp->mu;
    mp->infVar[43] = mp->comp;
    mp->infVar[45] = mp->vx;
    mp->infVar[46] = mp->vy;
    mp->infVar[48] = mp->r_free;
    mp->infVar[52] = mp->r;
    mp->infVar[65] = x_gnd;
    mp->infVar[66] = y_gnd;
    mp->infVar[67] = mp->zgnd;
    mp->infVar[68] = -mp->dzdx;
    mp->infVar[69] = -mp->dzdy;
    mp->infVar[70] = 1.0;
    break;
  }
  case 1:
    sMessage("***************************************************");
    sMessage("*** Simple linear tire model with STI format    ***");
    sMessage("*** Made by Yukio Watanabe - December 19, 2017  ***");
    sMessage("*************************************************** \n");
    sMessage("Tire model is initialized... \n");
    break;

  case 2:{
    mp->r_free = mp->r = mp->typarr[0];
    mp->kt = mp->typarr[1];
    mp->cfx = mp->typarr[2];
    mp->cfy = mp->typarr[3];
    mp->cmx = mp->typarr[4];
    mp->cmz = mp->typarr[5];
    mp->vlow_drift = mp->typarr[6];
    mp->vlow_slip  = mp->typarr[7];
    mp->lrelax_x = mp->typarr[8];
    mp->lrelax_y = mp->typarr[9];

    sMessage("Each tire is initialized...");
    sprintf(str, "Tire ID: %i", mp->idtyre);
    sMessage(str);
    sprintf(str, "%.4f: Free rolling radius (m)", mp->typarr[0]);
    sMessage(str);
    sprintf(str, "%.1f: Vertical stiffness (N/m)", mp->typarr[1]);
    sMessage(str);
    sprintf(str, "%.1f: Longitudinal slip stiffness (N/1)", mp->typarr[2]);
    sMessage(str);
    sprintf(str, "%.1f: Lateral cornering stiffness (N/rad)", mp->typarr[3]);
    sMessage(str);
    sprintf(str, "%.1f: Overturning stiffness (N-m/rad)", mp->typarr[4]);
    sMessage(str);
    sprintf(str, "%.1f: Aligning stiffness (N-m/rad)", mp->typarr[5]);
    sMessage(str);
    sprintf(str, "%.4f: Low speed limit: allow near-static Fx or Fy", mp->typarr[6]);
    sMessage(str);
    sprintf(str, "      : to avoid drift when stopped on a grade (m/s)");
    sMessage(str);
    sprintf(str, "%.4f: Minimum Vx used in ODE for lagged slip (m/s)", mp->typarr[7]);
    sMessage(str);
    sprintf(str, "%.4f: Tire longitudinal relaxation length (m)", mp->typarr[8]);
    sMessage(str);
    sprintf(str, "%.4f: Tire lateral relaxation length (m)\n", mp->typarr[9]);
    sMessage(str);

    break;
  }
  case 99:
    sMessage("Tire model is terminated...");
    break;
  }

  if (mp->ierr > 2)
    gPrintf_error("USERSTI had error (Job flag = %d, Tire %s, "
                    "error code = %d) \n%s \n", *job, suffix, mp->ierr, mp->tyrmod);
}

static void sFree (void) {
  int i;

  for (i = 0; i < sUserTireN; i++) {free (sUserTire[i]);}
  free (sUserTire); sUserTire = NULL;
#ifndef _VS_RT
  if (sUserLogFile) fclose(sUserLogFile);
#endif
}

// utility to initialize the tire model
static void sInit(int itire) {
  int iVext, i, job, iXext;
  UserPars *mp = sUserTire[itire];
  const char *ext = sOut_suffix_tire(itire, sDualsArePossible);
  vs_real rre, r_free, fz_old, dz;
  vs_real kt = 0, temp;

  mp->ntypar = 10;
  mp->nvars  = 71;

  for (i = 0; i < mp->ntypar; i++) {
    temp = gTyparr(itire, i);
    mp->typarr[i] = temp;
  }

  job = 2; // read tyre property file, initialize road
  sTireCalc (mp, &job, ext);
  if (mp->ierr > 2) return;

  /* Temporally set initial wheel height to 1m */
  mp->dis[0] = 0.0;
  mp->dis[1] = 0.0;
  mp->dis[2] = 1.0;

  /* Temporally set orientation angles to zero */
  mp->angle[0] = 0.0;
  mp->angle[1] = 0.0;
  mp->angle[2] = 0.0;

  do {
    if (mp->is_first) { // the first time.
      mp->is_first = FALSE;
      job = 0;
      sTireCalc (mp, &job, ext);
      if (mp->ierr > 2) return;
    }

    fz_old = mp->infVar[2];
    job = 5;
    dz = -0.0001;
    mp->dis[2] += dz;
    sTireCalc (mp, &job, ext);
    if (mp->ierr > 2) return;
    kt=fabs((mp->infVar[2]-fz_old)/dz);
  } while(!mp->infVar[2] || !fz_old);

  // Calculate the effective rolling radius.
  rre = mp->infVar[48];
  r_free = mp->dis[2];

  gTiresSetPars (itire, rre, r_free, kt, 0.0, 0.0, 0.0);

  // Define VARINF output
  if ((int)strlen(ext) < 4) {
    for (i = 0; i < mp->nvars; i++) {
      if ((int)strlen(ext) > 2) {
        sprintf (mp->keyVar[i], "Inf%d%s", i+1, ext);
      }
      else {
        sprintf (mp->keyVar[i], "Inf%d_%s", i+1, ext);
      }

      sprintf (mp->descVar[i], "External tire VARINF #%d (%s)", i+1, ext);
      if (!gHave_keyword_in_database(mp->keyVar[i])) {
        iVext = gDefine_out (mp->keyVar[i], mp->descVar[i], &mp->infVar[i], "-",
                               "External tire VARINF", "User Tire Model", "User Tire Model");
      }
    }
  }

  // Define longitudinal slip state variables.
  sprintf(mp->keyKappa, "SV_KAPPA_STI_%s", ext);
  sprintf(mp->descKappa, "longitudinal slip STI (%s)", ext);
  if (!gHave_keyword_in_database(mp->keyKappa)) {
    iXext = gDefine_sv(mp->keyKappa, &mp->stateKappa, 0.0, "-", mp->descKappa);
    // Set derivative if necessary.
    if (gSet_sym_attribute) {
      gSet_sym_attribute(iXext, SV_DERIVATIVE, &mp->stateKappaDeriv);
    }
  }

  // Define tangent lateral slip state variables.
  sprintf(mp->keyTana, "SV_TANA_STI_%s", ext);
  sprintf(mp->descTana, "tangent lateral slip STI (%s)", ext);
  if (!gHave_keyword_in_database(mp->keyTana)) {
    iXext = gDefine_sv(mp->keyTana, &mp->stateTana, 0.0, "-", mp->descTana);
    // Set derivative if necessary.
    if (gSet_sym_attribute) {
      gSet_sym_attribute(iXext, SV_DERIVATIVE, &mp->stateTanaDeriv);
    }
  }
}


// Read User STI control switches for a tire.
vs_bool vs_sti_scan(char *key, char *buffer) {
  int itire;
  static int isfirst = TRUE;
#ifndef _VS_RT
  static FILE *ifile;
#endif
  char path_file[FILENAME_MAX];

  strcpy(path_file, "");

  if (sDualsArePossible) itire = sGet_itab_tire() -1;
  else itire = sGet_itab_wheel() -1;

  if (itire < 0)
    return FALSE;

  if (!strcmp(key, "USER_STI_DATA")) {
    if (!strstr(buffer, ":\\") && !strstr(buffer, "\\\\"))
      sprintf(path_file, "%s%s", gDatabaseDir(), buffer);
    else
      strcpy(path_file, buffer);

    strcpy (sUserTire[itire]->chtdst, path_file);
#ifndef _VS_RT
    ifile = fopen(path_file, "r");
    if (!ifile) {
      gPrintf_error ("a property file for User STI tire model was not found.");
      strcpy (sUserTire[itire]->chtdst, "");
      return TRUE;
    }
    else {
      fclose(ifile);
    }
#endif
    // Check if TRD file
    sString_to_upper(path_file);
    if (strstr(path_file, ".TRD")) {
      vscUserErrorFourTires (itire + 1);
    }
    return TRUE;
  }
  else if (!strcmp(key, "USER_STI_SW")) {
    sUserTire[itire]->iswtch = atoi(buffer);
    return TRUE;
  }
  else if (!strcmp(key, "USER_STI_ID_ROAD")) {
    sUserTire[itire]->idroad = atoi(buffer);
    return TRUE;
  }
  else if (!strcmp(key, "USER_STI_ROAD")) {
    if (!strstr(buffer, ":\\") && !strstr(buffer, "\\\\"))
      sprintf(path_file, "%s%s", gDatabaseDir(), buffer);
    else
      strcpy(path_file, buffer);

    sprintf(sUserTire[itire]->chrdst, path_file);
#ifndef _VS_RT
    ifile = fopen(path_file, "r");
    if (!ifile) {
      gPrintf_error ("a road data file for User STI tire model was not found.");
      strcpy (sUserTire[itire]->chrdst, "");
    }
    else {
      fclose(ifile);
    }
#endif
    return TRUE;
  }
  else if (!strcmp(key, "USER_STI_INIT")) {
    sInit (itire);
    return TRUE;
  }
  return FALSE;
}

// Allocate memory for User tire parameters and install callback functions
void Install (int nTires, int duals) {
  int i, job = 1;
#ifndef _VS_RT
  char log_path[FILENAME_MAX];
#endif
  sUserTireN = nTires;
  sDualsArePossible = duals;
 
  sUserTire = (UserPars **)calloc(sUserTireN, sizeof(UserPars *));
  for (i = 0; i < sUserTireN; i++)
    sUserTire[i] = (UserPars *)calloc(1, sizeof(UserPars));

  gInstall_free_function(&sFree);

  sIside = gGet_var_ptr_int("ISIDE"); // pointer to system parameter in VS Solver

  sAxleMap = gGet_var_ptr_int("VEHICLE_AXLE_MAP(1)");
  sUnitsN = gGet_var_ptr_int("VEHICLE_NUNITS");
  sIunit = gGet_var_ptr_int("IUNIT");
  sIaxle = gGet_var_ptr_int("IAXLE");
  sItire = gGet_var_ptr_int("ITIRE");
#ifndef _VS_RT
  strcpy(log_path, "");
  sprintf(log_path, "%s_sti_log.txt", gGet_Filebase_name());
  sUserLogFile = fopen(log_path, "w");
#endif
  sTireCalc(sUserTire[0], &job, sOut_suffix_tire(0, sDualsArePossible));
}

// Set default values for User tire parameters.
void Setdef (void) {
  int i;
  UserPars *mp;

  // set default values for parameters defined in this file
  for (i = 0; i < sUserTireN; i++) {
    mp = sUserTire[i];
    mp->is_first   = TRUE;
    mp->idtyre     = i + 1;
    strcpy(mp->chtdst,"");
    strcpy(mp->chsdst,"");
    mp->iswtch = 1;
    /// Loading road data
    mp->idroad = 0;
    strcpy(mp->chrdst, "");
    strcpy(mp->road,"dummyroad");
    strcpy(mp->tyrmod,"PutNameHere");

	  mp->typarr[0] = 0.4;
    mp->typarr[1] = 400000.0;
    mp->typarr[2] = 80000.0;
    mp->typarr[3] = -40000;
    mp->typarr[4] = -1200;
    mp->typarr[5] = 1600;
    mp->typarr[6] = 0.0556;
    mp->typarr[7] = 0.5556;
    mp->typarr[8] = 0.05;
    mp->typarr[9] = 0.5;
  }
}

// Echo User STI control switches for a tire (itire = 0, 1, ...)
void vs_sti_echo(FILE *ifile, int itire) {
  UserPars *mp = sUserTire[itire];

  // Use function from imap.c to echo iaxle, iside, iunit
  if (sDualsArePossible) {
    gPrintEcho (ifile, "\n! User STI model switches and file path name. (%s)\n",
      sTab_label_tire(itire));
  }
  else {
    gPrintEcho (ifile, "\n! User STI model switches and file path name. (%s)\n",
      sTab_label_wheel(itire));
  }

  if (sDualsArePossible) sEcho_tire (ifile, itire);
  else sEcho_wheel (ifile, itire);

  gPrintEcho (ifile, "! User STI property file pathname\n");
  gWrite_echo_line (ifile, "USER_STI_DATA", mp->chtdst, "");
  gWrite_echo_line (ifile, "USER_STI_DLL", mp->chsdst, "");
  gWritei (ifile, "USER_STI_SW", mp->iswtch,
               "! Operating mode of user STI model");
  gWritei (ifile, "USER_STI_ID_ROAD", mp->idroad,
               "! Operating mode of user road ");
  if (strcmp(mp->chrdst, "")) {
    gPrintEcho (ifile, "! User STI road data file pathname (optional)\n");
    gWrite_echo_line (ifile, "USER_STI_ROAD", mp->chrdst, "");
  }
  gPrintEcho(ifile, "USER_STI_INIT\n");
}


// Calculate tire forces and moments and apply at wheel center
void Calc (int itab, vs_real *fx0, vs_real *fy0, vs_real *fz0,
                    vs_real *mx0, vs_real *my0, vs_real *mz0, vs_real **infVar) {
  int job;
  const int itire = itab -1;
  UserPars *mp = sUserTire[itire];

  job = 0; // default mode: calculate tire forces
  sTireCalc (mp, &job, sOut_suffix_tire(itire, sDualsArePossible));

  *fx0 = mp->force[0]; // forces and moments at wheel center
  *fy0 = mp->force[1];
  *fz0 = mp->force[2];
  *mx0 = mp->torque[0];
  *my0 = mp->torque[1];
  *mz0 = mp->torque[2];

  *infVar = mp->infVar; // array of USER STI variables for use by other functions
}


// Called from vscTiresFinish to close down for this run.
void vs_sti_exit(int i) {
  int job = 99; // 99 means final call
  sTireCalc (sUserTire[i], &job, sOut_suffix_tire(i, sDualsArePossible));
}

// calculate direction cosine matrix and copy some wheel-center variables to mp
void CopyWcVars (int itab, vs_real xwc, vs_real ywc, vs_real zwc,
                       vs_real yaw, vs_real pitch, vs_real roll,
                       vs_real vxwc, vs_real vywc, vs_real vzwc,
                       vs_real avx, vs_real avy, vs_real avz,
                       vs_real rot, vs_real spin, vs_real t) {

  UserPars *mp = sUserTire[itab -1];

  if (t > 3) {
    int dummy = 1;
  }

  mp->dis[0] = xwc;
  mp->dis[1] = ywc;
  mp->dis[2] = zwc;

  mp->vel[0] = vxwc;
  mp->vel[1] = vywc;
  mp->vel[2] = vzwc;

  mp->omega[0] = avx;
  mp->omega[1] = avy;
  mp->omega[2] = avz;

  mp->angle[0] = yaw;
  mp->angle[1] = pitch;
  mp->angle[2] = roll;
  
  mp->angtwc = rot;
  mp->omegar = spin;
  sTime = t;
}

void CopyWcVars2(int itab, vs_real xwc, vs_real ywc, vs_real zwc,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real vxwc, vs_real vywc, vs_real vzwc,
  vs_real avx, vs_real avy, vs_real avz,
  vs_real rot, vs_real spin, vs_real t, vs_real ldual_half) {
  const double cpitch = cos(pitch), cyaw = cos(yaw), croll = cos(roll),
    spitch = sin(pitch), syaw = sin(yaw), sroll = sin(roll);
  UserPars *mp = sUserTire[itab - 1], *mp2 = sUserTire[itab];
  double z[8];

  mp2->angle[0] = mp->angle[0] = yaw;
  mp2->angle[1] = mp->angle[1] = pitch;
  mp2->angle[2] = mp->angle[2] = roll;

  mp2->omega[0] = mp->omega[0] = avx;
  mp2->omega[1] = mp->omega[1] = avy;
  mp2->omega[2] = mp->omega[2] = avz;

  mp->vel[1] = mp2->vel[1] = vywc;

  z[0] = spitch*sroll;
  z[1] = -z[0] * cyaw + croll*syaw;
  z[2] = ldual_half*z[1];
  mp->dis[0] = xwc + z[2]; // x
  z[3] = cyaw*croll + z[0] * syaw;
  z[4] = ldual_half*z[3];
  mp->dis[1] = -(z[4] - ywc); // y
  z[5] = ldual_half*cpitch*sroll;
  mp->dis[2] = -(z[5] - zwc);  // z
  mp2->dis[0] = -(z[2] - xwc); // x
  mp2->dis[1] = ywc + z[4]; // y
  mp2->dis[2] = zwc + z[5]; // z
  z[6] = ldual_half*avz;
  mp->vel[0] = z[6] + vxwc; // vx1
  z[7] = ldual_half*avx;
  mp->vel[2] = -z[7] + vzwc; // vz1
  mp2->vel[0] = -z[6] + vxwc; // vx2
  mp2->vel[2] = z[7] + vzwc; // vz2
  mp->vel[1] = mp2->vel[1] = vywc;

  mp2->angtwc = mp->angtwc = rot;
  mp2->omegar = mp->omegar = spin;
  sTime = t;
}

void vs_sti_SetVisible(void){
  int i;
  for (i = 0; i < sUserTireN; i++) {
    gSetTyparrVisible(i, 1);
  }
}

vs_real vs_sti_loaded_radius(int itab, vs_real fz, vs_real omegar) { return 0.0; }

// utility to return vx in denominator for kappa
static vs_real sVxDenKappa(UserPars *mp, vs_real abs_vx, vs_real dv_rot) {
  vs_real vlow;
  vs_real dv = max(fabs(mp->vy), max(abs_vx, dv_rot)) - mp->vlow_drift;

  if (dv < 0.0) // when "at rest" don't use vlow_kappa
    return max(0.01, abs_vx);

  vlow = max(abs_vx, mp->vlow_slip);
  if (dv >= mp->vlow_drift)
    return vlow;
  return abs_vx + (vlow - abs_vx)*dv / mp->vlow_drift; // transition
}

static vs_real sVxDenAlpha(UserPars *mp, vs_real abs_vx) {
  vs_real vlow; 
  vs_real dv = max(fabs(mp->vy), abs_vx) - mp->vlow_drift;

  if (dv < 0.0) // when "at rest" don't use vlow_alpha
    return abs_vx;

  vlow = max(abs_vx, mp->vlow_slip);
  if (dv >= mp->vlow_drift)
    return vlow;
  return abs_vx + (vlow - abs_vx)*dv / mp->vlow_drift; // transition
}

/* ---------------------------------------------------------------------------------
Calculate instant kappa, the derivative for lagged kappa (state variable used
by tire models), and possibly override lagged kappa.
--------------------------------------------------------------------------------- */
static void sCalcKappaAndDeriv(int id, vs_real w) {  
  UserPars *mp = sUserTire[id - 1];
  vs_real abs_vx = fabs(mp->vx),
    dv_rot = mp->r_free*w - mp->vx,
    vx_den = sVxDenKappa(mp, abs_vx, dv_rot),
    kappa_safe = dv_rot / max(0.01, vx_den);

  // if user has set relaxation length to zero, use safe instant slip
  if (mp->lrelax_x == 0.0) {
    mp->stateKappa = kappa_safe;
    mp->stateKappaDeriv = 0.0;
    return;
  }

  // ODE for kappa_lag
  mp->stateKappaDeriv = (dv_rot - vx_den * mp->stateKappa) / mp->lrelax_x;
}

static void sCalcTanAlphaAndDeriv(int id) {
  UserPars *mp = sUserTire[id - 1];
  vs_real abs_vx = fabs(mp->vx),
    vx_den = sVxDenAlpha(mp, abs_vx),
    tan_alpha = mp->vy / max(0.01, abs_vx);

  // if user has set relaxation length to zero, use instantaneous slip
  if (mp->lrelax_y == 0.0) {
    mp->stateTana = tan_alpha;
    mp->stateTanaDeriv = 0.0;
    return;
  }

  // ODE for dtana_lag
  mp->stateTanaDeriv = (mp->vy - (mp->stateTana)*vx_den) / mp->lrelax_y;
}

static void sCalcTireCtcForces(int id, vs_real w) {
  UserPars *mp = sUserTire[id - 1];

  mp->fx = mp->fy = mp->fz = 0.0;
  mp->mx = mp->my = mp->mz = 0.0;

  // Fz calculation
  mp->comp = mp->r_free - mp->r;
  mp->fz = mp->kt * mp->comp;

  sCalcKappaAndDeriv(id, w);
  sCalcTanAlphaAndDeriv(id);

  if (mp->fz < 0.01) {
    mp->alpha = 0;
    mp->kappa = 0;
    mp->fz = 0;
    mp->comp = 0;
  }
  else { // no internal tire model. Just calculate alpha and kappa, then return.
    vs_real fzmu = mp->mu*mp->fz;

    mp->kappa = mp->stateKappa;
    mp->alpha = atan(mp->stateTana);

    if (mp->mu > 0.0) {
      mp->fx = max(-1 * fzmu, min(fzmu, mp->cfx * mp->kappa * mp->mu));
      mp->fy = max(-1 * fzmu, min(fzmu, mp->cfy * mp->alpha * mp->mu));

      mp->mz = mp->cmz * mp->alpha * mp->mu;
      mp->mx = mp->cmx * mp->gamma * mp->mu;
    }
    else {
      mp->fx = max(-1 * mp->fz, min(mp->fz, mp->cfx * mp->kappa));
      mp->fy = max(-1 * mp->fz, min(mp->fz, mp->cfy * mp->alpha));
      mp->mz = mp->cmz * mp->alpha;
      mp->mx = mp->cmx * mp->gamma;
    }
  }
}

#ifndef _dSPACE_RT
void get_ctc_xy(int id, vs_real xw0, vs_real yw0, vs_real zw0,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real r_old_1, vs_real dzdx_old_1, vs_real dzdy_old_1,
  vs_real *xgnd_est_1, vs_real *ygnd_est_1)
{
  vs_real *c = sUserTire[id - 1]->c, *s = sUserTire[id - 1]->s,
    *z = sUserTire[id - 1]->z, *q = sUserTire[id - 1]->q;

  s[3] = sin(yaw);
  s[4] = sin(pitch);
  s[5] = sin(roll);
  c[3] = cos(yaw);
  c[4] = cos(pitch);
  c[5] = cos(roll);

  q[0] = xw0;
  q[1] = yw0;
  q[2] = zw0;

  // Get estimate of X and Y at CTC for first tire
  z[0] = c[4] * s[5];
  z[1] = c[3] * c[5];
  z[2] = s[4] * s[5];
  z[3] = z[1] + z[2] * s[3];
  z[4] = c[5] * s[3];
  z[5] = z[4] - z[2] * c[3];
  z[6] = z[0] - z[3] * dzdy_old_1 + z[5] * dzdx_old_1;
  z[7] = z[0] * z[6];
  z[8] = z[3] * z[6] + dzdy_old_1;
  z[9] = -z[5] * z[6] + dzdx_old_1;
  z[10] = 1.0 / sqrt(1 - z[7] + z[8] * dzdy_old_1 + z[9] * dzdx_old_1);
  z[11] = z[10] * r_old_1;
  *xgnd_est_1 = q[0] + z[9] * z[11];
  *ygnd_est_1 = q[1] + z[8] * z[11];
}

void get_ctc_kin(int id, vs_real vwx, vs_real vwy, vs_real vwz,
  vs_real avwx, vs_real avwy, vs_real avwz,
  vs_real dzdx_1, vs_real dzdy_1, vs_real r_old_1,
  vs_real *gamma_1, vs_real *rnew_1,
  vs_real *vctcx_1, vs_real *vctcy_1,
  vs_real *xgnd_1, vs_real *ygnd_1, vs_real *zgnd_1,
  vs_real *pitch_gnd, vs_real *roll_gnd)
{
  vs_real *c = sUserTire[id - 1]->c, *s = sUserTire[id - 1]->s,
    *z = sUserTire[id - 1]->z, *q = sUserTire[id - 1]->q;
  vs_real zgnd_old_1;

  // zgnd_old_1 is zgnd based on initial estimate of radius R.
  zgnd_old_1 = -(-q[2] + z[10] * (1 - z[7])*r_old_1);
  z[12] = 1 + (dzdy_1*dzdy_1) + (dzdx_1*dzdx_1);
  z[13] = 1.0 / sqrt(z[12]);
  z[14] = z[0] - z[3] * dzdy_1 + z[5] * dzdx_1;
  *gamma_1 = asin(z[13] * z[14]);

  // Corrected Radius based on new info about zgnd_1
  z[15] = cos(*gamma_1);
  (*rnew_1) = r_old_1 + z[13] * (zgnd_old_1 - (*zgnd_1)) / z[15];

  // Update X, Y, Z at CTC
  z[16] = z[4] - s[5] * (dzdx_1*c[4] + c[3] * s[4]);
  z[17] = s[3] * s[4];
  z[18] = z[1] + (z[17] + dzdy_1*c[4])*s[5];
  z[19] = z[5] * dzdy_1 + z[3] * dzdx_1;
  z[20] = 1.0 / sqrt((z[16] * z[16]) + (z[18] * z[18]) + (z[19] * z[19]));
  z[21] = z[19] * z[20];
  z[22] = z[16] * z[20];
  z[23] = z[18] * z[20];
  z[24] = z[23] * dzdy_1 - z[22] * dzdx_1;
  z[25] = z[13] * z[24];
  z[26] = z[13] * (z[23] + z[21] * dzdx_1);
  z[27] = z[13] * (z[22] + z[21] * dzdy_1);
  z[28] = z[0] * z[14];
  z[29] = z[3] * z[14] + dzdy_1;
  z[30] = -z[5] * z[14] + dzdx_1;
  z[31] = 1 - z[28] + z[29] * dzdy_1 + z[30] * dzdx_1;
  z[32] = 1.0 / sqrt(z[31]);
  z[33] = z[32] * (*rnew_1);
  *xgnd_1 = q[0] + z[30] * z[33];
  *ygnd_1 = q[1] + z[29] * z[33];
  z[34] = 1 - z[28];
  (*zgnd_1) = -(-q[2] + z[32] * z[34] * (*rnew_1));

  // calculate Vx, Vy at CTC
  z[35] = -z[21] * s[4] + c[4] * (z[23] * c[3] + z[22] * s[3]);
  z[36] = z[0] * z[21] + z[3] * z[22] - z[5] * z[23];
  z[37] = c[4] * c[5];
  z[38] = -z[17] * c[5] + c[3] * s[5];
  z[39] = c[3] * c[5] * s[4] + s[3] * s[5];
  z[40] = z[21] * z[37] - z[22] * z[38] + z[23] * z[39];
  z[41] = c[3] * c[4] * avwx;
  z[42] = z[5] * avwy;
  z[43] = z[39] * avwz;
  z[44] = avwx*s[4];
  z[45] = c[5] * avwz + avwy*s[5];
  z[46] = z[45] * c[4];
  z[47] = z[41] + z[43] - z[42] - (z[44] - z[46])*dzdx_1;
  z[48] = c[4] * avwx*s[3];
  z[49] = z[3] * avwy;
  z[50] = z[38] * avwz;
  z[51] = (z[44] - z[46])*dzdy_1;
  z[52] = (z[41] + z[43] - z[42])*dzdy_1 - (z[48] + z[49] - z[50])*dzdx_1;

  // Calculate Vx at CTC
  *vctcx_1 = z[35] * vwx + z[36] * vwy + z[40] * vwz - z[32] * (*rnew_1)*(-z[22] * z[47]
    - z[21] * z[52] + z[23] * (-z[50] - z[51] + z[48] + z[49]) + z[14]
    * (-z[40] * avwx + z[35] * avwz));
  z[53] = -z[25] * s[4] + c[4] * (-z[27] * c[3] + z[26] * s[3]);
  z[54] = z[0] * z[25] + z[3] * z[26] + z[5] * z[27];
  z[55] = -(-z[25] * z[37] + z[26] * z[38] + z[27] * z[39]);

  // Calculate Vy at CTC
  *vctcy_1 = -(-z[53] * vwx - z[54] * vwy - z[55] * vwz - z[32] * (*rnew_1)*(z[26] * z[47] +
    z[25] * z[52] + z[27] * (-z[50] - z[51] + z[48] + z[49]) - z[14]
    * (-z[55] * avwx + z[53] * avwz)));

  // Calculate pitch and roll at CTC (for animator)
  z[56] = asin(z[21]);
  *pitch_gnd = -z[56];
  z[57] = cos(z[56]);
  *roll_gnd = asin(z[25] / z[57]);
}

void ctc_transform(int id, vs_real rnew_1, vs_real dzdx_1, vs_real dzdy_1,
  vs_real fxctc_1, vs_real fyctc_1, vs_real fzctc_1,
  vs_real mxctc, vs_real myctc, vs_real mzctc,
  vs_real *fxw0, vs_real *fyw0, vs_real *fzw0,
  vs_real *mxw0, vs_real *myw0, vs_real *mzw0)
{
  vs_real *c = sUserTire[id - 1]->c, *s = sUserTire[id - 1]->s,
    *z = sUserTire[id - 1]->z;

  z[58] = s[4] + c[4] * (dzdx_1*c[3] + dzdy_1*s[3]);

  // Fx at wheel center
  *fxw0 = -z[13] * z[58] * fzctc_1 + z[35] * fxctc_1 + z[53] * fyctc_1;

  // Fy at wheel center
  *fyw0 = z[13] * z[14] * fzctc_1 + z[36] * fxctc_1 + z[54] * fyctc_1;
  z[59] = z[37] + z[38] * dzdy_1 - z[39] * dzdx_1;

  // Fz at wheel center
  *fzw0 = z[13] * z[59] * fzctc_1 + z[40] * fxctc_1 + z[55] * fyctc_1;

  // Add tire moments and moments of CTC forces applied to wheel center
  z[60] = 1.0 / sqrt(z[12] * z[31]);
  z[61] = -(z[23] * z[29] - z[22] * z[30])*fxctc_1 + (z[27] * z[29] +
    z[26] * z[30])*fyctc_1;
  z[62] = z[21] * z[29];
  z[63] = z[25] * z[29];
  z[64] = (z[62] + z[22] * z[34])*fxctc_1 + (z[63] + z[26] * z[34])*fyctc_1;
  z[65] = z[21] * z[30];
  z[66] = z[25] * z[30];
  z[67] = (z[65] + z[23] * z[34])*fxctc_1 - (z[27] * z[34] - z[66])*fyctc_1;

  // Mx at wheel center
  *mxw0 = -(z[13] * z[58] * mzctc - z[35] * mxctc - z[53] * myctc - rnew_1
    *(z[14] * z[60] * fzctc_1*(-z[19] * s[4] + c[4] * (z[18] * c[3] + z[16] * s[3]))
    + z[32] * (-z[61] * s[4] + c[4] * (z[64] * c[3] - z[67] * s[3]))));

  // My at wheel center
  *myw0 = z[13] * z[14] * mzctc + z[36] * mxctc + z[54] * myctc - z[32] * rnew_1*(z[5]
    * ((z[22] + z[62])*fxctc_1 + (z[26] + z[63])*fyctc_1) + z[3] * ((z[23] +
    z[65])*fxctc_1 - (z[27] - z[66])*fyctc_1) - z[0] * (-z[24] * fxctc_1 +
    (z[27] * dzdy_1 + z[26] * dzdx_1)*fyctc_1));

  // Mz at wheel center
  *mzw0 = z[13] * z[59] * mzctc + z[40] * mxctc + z[55] * myctc + rnew_1*(z[32]
    * (z[37] * z[61] + z[39] * z[64] + z[38] * z[67]) + z[14] * z[60]
    * (-z[16] * z[38] + z[19] * z[37] + z[18] * z[39])*fzctc_1);
}
#endif

