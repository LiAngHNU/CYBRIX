/* This file contains routines to integrate the User STI tire model to VS solvers. 

   Copyright 1997 - 2015. Mechanical Simulation Corporation. All rights reserved.

   09-16-15 Y. Watanabe: Based on an existing user's tire model interface and changed to
                         to generic STI interface. Also the program is moved from internal
                         to external of VS solvers.
*/

//#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vs_deftypes.h"
#include "vs_sti_tire.h"
#include "tire_sti.h"

#include <windows.h>

// structure for USER STI parameter sets for a tire
typedef struct {
  vs_real typarr[27], wrkarr[1], infVar[100], ropar[10],
          dis[3], tramat[9], vel[3], omega[3], force[3], torque[3],
          angtwc, omegar, deqvar[2], deqini[2], deqder[2];
  int     is_first, ntypar, ndeqvr, nropar, nwork, niwork, nvars,
          iswtch, iwrkar[1], idtyre, ierr, nchtds, nchrds, idroad,
          len_tyrmod, len_chrdst, len_chtdst, len_road;
  char    tyrmod[256], chrdst[256], chtdst[256], chsdst[256], road[256],
          descVar[100][33], keyVar[100][15], desc[100][33], keyHash[20][15];
  int     id[30], // indices to all parameters, used to set visibility
          npar, // number of parameters (129 the last time this was updated)
          *opt_tire_model; // pointer to external tire model option parameter
} UserPars;

static UserPars **sUserTire;
static int sUserTireN;
static vs_real sTime;
static int *sIside, sAxleMap[3], *sUnitsN, *sIunit, *sIaxle, *sItire;

static HMODULE USER_Module = NULL;
static int sDualsArePossible = FALSE; // generate labels for duals?
static int sIsDLLfound = FALSE;
static FILE *sUserLogFile;

#define READ_BUFFER_LEN 200

// local functions that need to be declared here
static void sCalc (int itab, vs_real *fx0, vs_real *fy0, vs_real *fz0,
                    vs_real *mx0, vs_real *my0, vs_real *mz0, vs_real **infVar);
static void sCopyWcVars (int itab, vs_real xwc, vs_real ywc, vs_real zwc,
                       vs_real yaw, vs_real pitch, vs_real roll,
                       vs_real vxwc, vs_real vywc, vs_real vzwc,
                       vs_real avx, vs_real avy, vs_real avz,
                       vs_real rot, vs_real spin, vs_real t);
static void sCopyWcVars2(int itab, vs_real xwc, vs_real ywc, vs_real zwc,
                          vs_real yaw, vs_real pitch, vs_real roll,
                          vs_real vxwc, vs_real vywc, vs_real vzwc,
                          vs_real avx, vs_real avy, vs_real avz,
                          vs_real rot, vs_real spin, vs_real t, vs_real ldual_half);
static void sInsall (int nTires, int duals);
static void sSetdef (void);                         
                          
// VS API functions used in this wrapper                          
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
  sCopyWcVars(itire, xw0, yw0, zw0, yaw, pitch, roll, vwx, vwy, vwz, avwx,
    avwy, avwz, rot, w, time);
  sCalc(itire, fxw0, fyw0, fzw0, mxw0, myw0, mzw0, &userVar);
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

  sCopyWcVars2(itire, xw0, yw0, zw0, yaw, pitch, roll, vwx, vwy, vwz,
    avwx, avwy, avwz, rot, w, time, 0.5*ldual);
  sCalc(itire, fxw0, fyw0, fzw0, mxw0, myw0, mzw0, &userVar);
  sCalc(itire2, &fx2, &fy2, &fz2, &mx2, &my2, &mz2, &userVar2);

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
  sInsall(nTires, duals);
  sSetdef();
}

// User road
void __cdecl vscUserRoad(int *idtyre, double dis[2], double *z, double dz[2],
  double *mu, int *ierr) {

  int inst;
  vs_real mux = 1, muy = 1, zgnd = 0, dzdx = 0, dzdy = 0;
 
  inst = *idtyre;
  gGet_road_contact_ext(dis[1], dis[0], inst, &zgnd, &dzdy, &dzdx, &muy, &mux);

  *z = zgnd;
  dz[0] = dzdx;
  dz[1] = dzdy;

  *mu = mux;
}

void __cdecl sMessage(char* msg) {
  fprintf(sUserLogFile, "user/STI: %s\n", msg);
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
static void sCallSti (UserPars *mp, int *job, const char *suffix) {
  if (!sIsDLLfound) {
    gPrintf_error(
      "the solver program (DLL) was not found, not properly loaded, "
      "or not supported on this platform.");
    return;
  }

  simple_sti(sMessage, &mp->iswtch, job, &mp->idtyre, &sTime, mp->dis,
                mp->tramat, &mp->angtwc, mp->vel, mp->omega, &mp->omegar,
                &mp->ndeqvr, mp->deqvar, &mp->ntypar, mp->typarr, &mp->nchtds,
                mp->chtdst, &mp->len_chtdst, vscUserRoad, //mp->road, &mp->len_road,
                &mp->idroad,
                &mp->nropar, mp->ropar, &mp->nchrds, mp->chrdst, &mp->len_chrdst,
                mp->force, mp->torque, mp->deqini, mp->deqder,
                mp->tyrmod, &mp->len_tyrmod, &mp->nvars, mp->infVar, &mp->nwork,
                mp->wrkarr, &mp->niwork, mp->iwrkar, &mp->ierr);

  if (mp->ierr > 2)
    gPrintf_error("USERSTI had error (Job flag = %d, Tire %s, "
                    "error code = %d) \n%s \n", *job, suffix, mp->ierr, mp->tyrmod);
}

static void sFree (void) {
  int i;

  for (i = 0; i < sUserTireN; i++) {free (sUserTire[i]);}
  free (sUserTire); sUserTire = NULL;

  if (sUserLogFile) fclose(sUserLogFile);

  if (USER_Module) {
    FreeLibrary(USER_Module);
    USER_Module = NULL;
  }
}

// utility to load the user time model DLL
static void sLoad (char *pathname) {
  static FILE *ifile;
  char log_path[FILENAME_MAX];
  int job = 1;

  if (!pathname) return;

  ifile = fopen(pathname, "r");
  if (!ifile) {
    gPrintf_error ("the User STI solver program (DLL) was not found.");
    return;
  }
  else {
    fclose(ifile);
    USER_Module = LoadLibrary(pathname);
    simple_sti = (void *)GetProcAddress(USER_Module, "simple_sti");

    if (USER_Module)
      sIsDLLfound = TRUE;
    else {
      gPrintf_error ("the User STI solver program (DLL) did not load properly.");
      return;
    }

    strcpy(log_path, "");
    sprintf(log_path, "%s_sti_log.txt", gGet_Filebase_name());
    sUserLogFile = fopen(log_path, "w");

    sCallSti(sUserTire[0], &job, sOut_suffix_tire(0, sDualsArePossible));
  }
}

// utility to initialize the tire model
static void sInit(int itire) {
  int iVext, iXext, i, job;
  UserPars *mp = sUserTire[itire];
  const char *ext = sOut_suffix_tire(itire, sDualsArePossible);
  vs_real rre, r_free, fz_old, dz;
  vs_real kt = 0, temp;

  mp->ntypar = 10;
  mp->nwork  = 1;
  mp->niwork = 1;
  mp->nropar = 2;
  mp->ndeqvr = 2;
  mp->deqini[0] = 0.0;
  mp->deqini[1] = 0.0;
  mp->nvars  = 71;

  mp->ropar[0] = 0;
  mp->ropar[1] = 0;

  for (i = 0; i < mp->ntypar; i++) {
    temp = gTyparr(itire, i);
    mp->typarr[i] = temp;
  }

  job = 2; // read tyre property file, initialize road
  sCallSti (mp, &job, ext);
  if (mp->ierr > 2 || !sIsDLLfound) return;

  /* Temporally set initial wheel height to 1m */
  mp->dis[0] = 0.0;
  mp->dis[1] = 0.0;
  mp->dis[2] = 1.0;

  /* Temporally set unit diagonal matrix for tramat */
  mp->tramat[0] = 1.0;
  mp->tramat[4] = 1.0;
  mp->tramat[8] = 1.0;

  do {
    if (mp->is_first) { // the first time.
      mp->is_first = FALSE;
      job = 0;
      sCallSti (mp, &job, ext);
      if (mp->ierr > 2) return;
    }

    fz_old = mp->infVar[2];
    job = 5;
    dz = -0.0001;
    mp->dis[2] += dz;
    sCallSti (mp, &job, ext);
    if (mp->ierr > 2) return;
    kt=fabs((mp->infVar[2]-fz_old)/dz);
  } while(!mp->infVar[2] || !fz_old);

  // Calculate the effective rolling radius.
  rre = mp->infVar[48];
  r_free = mp->dis[2];

  gTiresSetPars (itire, rre, r_free, kt, 0.0, 0.0, 0.0);

  // Define state variables.
  for (i = 0; i < mp->ndeqvr; i++) {
    sprintf(mp->keyHash[i], "SV_VS_STI_%s_%d", ext, i + 1);
    sprintf(mp->desc[i], "State variable #%d (%s)", i + 1, ext);
    if (!gHave_keyword_in_database(mp->keyHash[i])) {
      iXext = gDefine_sv(mp->keyHash[i], &mp->deqvar[i], 0.0, "-", mp->desc[i]);
      if (gSet_sym_attribute) {
        gSet_sym_attribute(iXext, SV_DERIVATIVE, &mp->deqder[i]);
      }
    }
  }

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
}


// Read User STI control switches for a tire.
vs_bool vs_sti_scan(char *key, char *buffer) {
  int itire;
  static int isfirst = TRUE;
  static FILE *ifile;
  char path_file[FILENAME_MAX], oldExt[20], *p;

  strcpy(path_file, "");

  if (sDualsArePossible) itire = sGet_itab_tire() -1;
  else itire = sGet_itab_wheel() -1;

  if (itire < 0)
    return FALSE;

  if (!strcmp(key, "USER_STI_DLL")) {
    if(!strstr(buffer, ":\\") && !strstr(buffer, "\\\\"))
      sprintf(path_file, "%s%s", gDatabaseDir(), buffer);
    else
      strcpy(path_file, buffer);

	p = strrchr(path_file, '.');  // find .dll
	strcpy(oldExt, p);
	*p = 0;

#ifdef _WIN64 //64 bit solver
        strcat_s(path_file, sizeof(path_file), ".dll64\0");
#else  // 32 bit solver
        strcat_s(path_file, sizeof(path_file), ".dll32\0");
#endif

    ifile = fopen(path_file, "r");  // Veryfy if the external file exist.
    if (ifile)
    {
      fclose(ifile);
    }
    else
    {
      *p = 0;
      strcat_s(path_file, sizeof(path_file), oldExt);
      ifile = fopen(path_file, "r");  // Veryfy again with original setting.
      if (ifile)
        fclose(ifile);
      else
        return FALSE;
    }

    strcpy (sUserTire[itire]->chsdst, path_file);
    if (isfirst) sLoad(path_file);
    isfirst = FALSE;
    return TRUE;
  }

  if (!strcmp(key, "USER_STI_DATA")) {
    if (!strstr(buffer, ":\\") && !strstr(buffer, "\\\\"))
      sprintf(path_file, "%s%s", gDatabaseDir(), buffer);
    else
      strcpy(path_file, buffer);

    strcpy (sUserTire[itire]->chtdst, path_file);
    ifile = fopen(path_file, "r");
    if (!ifile) {
      gPrintf_error ("a property file for User STI tire model was not found.");
      strcpy (sUserTire[itire]->chtdst, "");
      return TRUE;
    }
    else {
      sUserTire[itire]->nchtds = (int)strlen(sUserTire[itire]->chtdst);
      fclose(ifile);
    }

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

    sUserTire[itire]->nchrds = (int)strlen(path_file);
    sprintf(sUserTire[itire]->chrdst, path_file);
    ifile = fopen(path_file, "r");
    if (!ifile) {
      gPrintf_error ("a road data file for User STI tire model was not found.");
      strcpy (sUserTire[itire]->chrdst, "");
    }
    else {
      sUserTire[itire]->nchrds = (int)strlen(sUserTire[itire]->chrdst);
      fclose(ifile);
    }
    return TRUE;
  }
  else if (!strcmp(key, "USER_STI_INIT")) {
    sInit (itire);
    return TRUE;
  }
  return FALSE;
}

// Allocate memory for User tire parameters and install callback functions
static void sInsall (int nTires, int duals) {
  int i;
  sUserTireN = nTires;
  sDualsArePossible = duals;
  char strMap[20];

  sUserTire = (UserPars **)calloc(sUserTireN, sizeof(UserPars *));
  for (i = 0; i < sUserTireN; i++)
    sUserTire[i] = (UserPars *)calloc(1, sizeof(UserPars));

  gInstall_free_function(&sFree);

  sIside = gGet_var_ptr_int("ISIDE"); // pointer to system parameter in VS Solver

  sUnitsN = gGet_var_ptr_int("VEHICLE_NUNITS");
  sIunit = gGet_var_ptr_int("IUNIT");
  sIaxle = gGet_var_ptr_int("IAXLE");
  sItire = gGet_var_ptr_int("ITIRE");

  for (i = 0; i < *sUnitsN; i++) {
    sprintf(strMap, "VEHICLE_AXLE_MAP(%d)", i + 1);
    sAxleMap[i] = *gGet_var_ptr_int(strMap);
  }
}

// Set default values for User tire parameters.
static void sSetdef (void) {
  int i;
  UserPars *mp;

  sIsDLLfound = FALSE;

  // set default values for parameters defined in this file
  for (i = 0; i < sUserTireN; i++) {
    mp = sUserTire[i];
    mp->is_first   = TRUE;
    mp->len_tyrmod = 256;
    mp->len_chrdst = 256;
    mp->len_chtdst = 256;
    mp->len_road   = 256;
    mp->nchtds     = 11;
    mp->idtyre     = i + 1;
    strcpy(mp->chtdst,"");
    strcpy(mp->chsdst,"");
    mp->iswtch = 1;
    /// Loading road data
    mp->nchrds = 21;
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
    gPrintEcho (ifile, "\n! User STI model switches and file pathname. (%s)\n",
      sTab_label_tire(itire));
  }
  else {
    gPrintEcho (ifile, "\n! User STI model switches and file pathname. (%s)\n",
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
static void sCalc (int itab, vs_real *fx0, vs_real *fy0, vs_real *fz0,
                    vs_real *mx0, vs_real *my0, vs_real *mz0, vs_real **infVar) {
  int job;
  const int itire = itab -1;
  UserPars *mp = sUserTire[itire];

  job = 0; // default mode: calculate tire forces
  sCallSti (mp, &job, sOut_suffix_tire(itire, sDualsArePossible));

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
  sCallSti (sUserTire[i], &job, sOut_suffix_tire(i, sDualsArePossible));
}

// calculate direction cosine matrix and copy some wheel-center variables to mp
static void sCopyWcVars (int itab, vs_real xwc, vs_real ywc, vs_real zwc,
                       vs_real yaw, vs_real pitch, vs_real roll,
                       vs_real vxwc, vs_real vywc, vs_real vzwc,
                       vs_real avx, vs_real avy, vs_real avz,
                       vs_real rot, vs_real spin, vs_real t) {
  const double cpitch = cos(pitch), cyaw = cos(yaw), croll = cos(roll),
               spitch = sin(pitch), syaw = sin(yaw), sroll = sin(roll);
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

  mp->tramat[0] = cyaw*cpitch;
  mp->tramat[3] = -syaw*croll + cyaw*spitch*sroll;
  mp->tramat[6] = syaw*sroll + cyaw*spitch*croll;

  mp->tramat[1] = syaw*cpitch;
  mp->tramat[4] = cyaw*croll + syaw*spitch*sroll;
  mp->tramat[7] = -cyaw*sroll + syaw*spitch*croll;

  mp->tramat[2] = -spitch;
  mp->tramat[5] = cpitch*sroll;
  mp->tramat[8] = cpitch*croll;

  mp->angtwc = rot;
  mp->omegar = spin;
  sTime = t;
}

static void sCopyWcVars2(int itab, vs_real xwc, vs_real ywc, vs_real zwc,
  vs_real yaw, vs_real pitch, vs_real roll,
  vs_real vxwc, vs_real vywc, vs_real vzwc,
  vs_real avx, vs_real avy, vs_real avz,
  vs_real rot, vs_real spin, vs_real t, vs_real ldual_half) {
  const double cpitch = cos(pitch), cyaw = cos(yaw), croll = cos(roll),
    spitch = sin(pitch), syaw = sin(yaw), sroll = sin(roll);
  UserPars *mp = sUserTire[itab - 1], *mp2 = sUserTire[itab];
  double z[8];

  mp2->tramat[0] = mp->tramat[0] = cyaw*cpitch;
  mp2->tramat[3] = mp->tramat[3] = -syaw*croll + cyaw*spitch*sroll;
  mp2->tramat[6] = mp->tramat[6] = syaw*sroll + cyaw*spitch*croll;

  mp2->tramat[1] = mp->tramat[1] = syaw*cpitch;
  mp2->tramat[4] = mp->tramat[4] = cyaw*croll + syaw*spitch*sroll;
  mp2->tramat[7] = mp->tramat[7] = -cyaw*sroll + syaw*spitch*croll;

  mp2->tramat[2] = mp->tramat[2] = -spitch;
  mp2->tramat[5] = mp->tramat[5] = cpitch*sroll;
  mp2->tramat[8] = mp->tramat[8] = cpitch*croll;

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


