# -----------------------------------------------------------------------
#
#  MAKE-file to build $(BOARD) object library carsim.lib
#
#  usage: dsmake -f carsim.mk
#
# -----------------------------------------------------------------------

VEH_CODE = $(VC)
VERSION = 20160
MATLAB_PATH = c:\MATLAB\R2007b
BOARD = DS1006

# -----------------------------------------------------------------------
#  library definitions
# -----------------------------------------------------------------------

LIB = vs_sti_simple_tire.lib

# -----------------------------------------------------------------------
#  source files
# -----------------------------------------------------------------------

C_SRCS = vs_sti_simple_tire.c
SRC_FILES = $(C_SRCS)

# -----------------------------------------------------------------------
#  compiler and tools
# -----------------------------------------------------------------------

%if "$(COMP_ROOT)" == ""
   # undefine COMP_ROOT because Command-line definition has higher precedence
%  undef COMP_ROOT
   COMP_ROOT := $(X86_ROOT)
# %echo $(COMP_ROOT)
%endif


CPP       = "$(COMP_ROOT)\bin\x86-cpp.exe"
CC        = "$(COMP_ROOT)\bin\x86-gcc.exe"
AS        = "$(COMP_ROOT)\bin\x86-as.exe"
AR        = "$(COMP_ROOT)\bin\x86-ar.exe"
LN        = "$(COMP_ROOT)\bin\x86-ld.exe"

COMP_VER_INFO_FILE = $(COMP_ROOT,>\LIB\compiler_version.mk)
%if exists("$(COMP_VER_INFO_FILE,"=)")
%  include $(COMP_VER_INFO_FILE)
%endif

# -----------------------------------------------------------------------
#  include directories
# -----------------------------------------------------------------------

INC_PATH =   -I. \
             -I"$(DSPACE_ROOT)\$(BOARD)\RTLib" \
             -I"$(DSPACE_ROOT)\$(BOARD)\RTKernel" \
             -I"$(DSPACE_ROOT)\MATLAB\RTI1006\C" \
             -I"$(DSPACE_ROOT)\MATLAB\RTI1006\SFcn" \
             -I"$(COMP_ROOT)\INCLUDE" \
	     -I"$(COMP_ROOT)\lib\gcc-lib\i686-pc-linux-gnu\$(COMP_VERSION)\include" \
             -I$(MATLAB_PATH)\simulink\include \
             -I$(MATLAB_PATH)\extern\include \
             -I$(MATLAB_PATH)\rtw\c\libsrc \
             -I$(MATLAB_PATH)\rtw\c\src \

#%echo $(INC_PATH)

# -----------------------------------------------------------------------
#  flags
# -----------------------------------------------------------------------

CC_INC_OPT =

CPP_FLAGS  = -D_DS1006 -DDS1006 -Dds1006 -D__KERNEL__ -DMODULE -D_INLINE

CC_FLAGS = -c -pipe -mpreferred-stack-boundary=2 -march=athlon-xp -msse2 \
           -mfpmath=sse -fno-strict-aliasing -fno-common -fomit-frame-pointer \
           -ffast-math -O2 -finline-functions \
           -DSMODE=RTSIM -DUSE_RTMODEL -DRT -DNUMST=1 -DNCSTATES=0 -DTID01EQ=0 -DNO_PRINTF \
           -DRTI_BUILD -D_dSPACE_RT -D_VS_RT $(CPP_FLAGS) $(INC_PATH)

#           $(INC_PATH,<$(CC_INC_OPT))

#           -ffast-math -g \
#           -ffast-math -O2 -finline-functions \


AS_FLAGS   =

AR_FLAGS   = -rsv


# -----------------------------------------------------------------------
#  board definitions and file extensions
# -----------------------------------------------------------------------

BOARD_TYPE     = $(BOARD)

RTLIB_DIR      = $(BOARD_TYPE)\RTLib
RTKERNEL_DIR   = $(BOARD_TYPE)\RTKernel

C_EXT          = c
ASM_EXT        = s
ASM_PP_EXT     = ss
SRC_EXT        = $(C_EXT) $(ASM_EXT) $(ASM_PP_EXT)

OBJ_EXT        = o86
OBJ_EXT_ND     = obj
LIB_EXT        = lib

# -----------------------------------------------------------------------
#  Check for properly installation of tools
# -----------------------------------------------------------------------

# Test for whitespace characters in the TMP and TEMP variables
%foreach TEMPLIST in TEMP TMP
   TEMPVALUE = $($(TEMPLIST))
%  if "$(TEMPVALUE)" != "$(TEMPVALUE,Wx)"
%    echo WARNING: Environment variable $(TEMPLIST) is set to a directory name
%    echo          containing white-space characters, which usually is
%    echo          incompatible with the used make and compiler tools.
%    echo
%    echo          Current setting is:
%    echo          $(TEMPLIST)=$(TEMPVALUE)
%    echo
%  endif
%end

%if !exists($(CPP))
%  echo ERROR: Preprocessor not properly installed as $(CPP)
   ABORT=TRUE
%endif

%if !exists($(CC))
%  echo ERROR: Compiler     not properly installed as $(CC)
   ABORT=TRUE
%endif

%if !exists($(AS))
%  echo ERROR: Assembler    not properly installed as $(AS)
   ABORT=TRUE
%endif

%if !exists($(AR))
%  echo ERROR: Archiver     not properly installed as $(AR)
   ABORT=TRUE
%endif

%if !exists($(LN))
%  echo ERROR: Linker       not properly installed as $(LN)
   ABORT=TRUE
%endif

%if "$(ABORT)"=="TRUE"
%  abort
%endif


# -----------------------------------------------------------------------
#  auxiliary variables
# -----------------------------------------------------------------------

%foreach ext in $(SRC_EXT)
  OBJ_FILES_ND += $(SRC_FILES_ND,M\.$(ext)$$, R,>.$(OBJ_EXT_ND))
  OBJ_FILES_D += $(SRC_FILES,M\.$(ext)$$, R,>.$(OBJ_EXT))
%endfor


OBJ_FILES    =  $(OBJ_FILES_D) $(OBJ_FILES_ND)


# -----------------------------------------------------------------------
# dependency generator definitions
# -----------------------------------------------------------------------

.TYPE.SRC              += .$(ASM_PP_EXT)
.HDR.$(ASM_PP_EXT)     = [\.\#]include[ \t]+["<]\(.*\)[">]
.HDR.$(ASM_EXT)        = \.include[ \t]+"\(.*\)"
.HDRPATH.$(ASM_PP_EXT) = .
.HDRPATH.$(ASM_EXT)    = .


DP_SRC  = $(SRC_FILES,M\.$(C_EXT)$$, R,>.$(OBJ_EXT))
DP_SRC += $(SRC_FILES,M\.$(ASM_EXT)$$, R,>.$(OBJ_EXT))
DP_SRC += $(SRC_FILES,M\.$(ASM_PP_EXT)$$, R,>.$(ASM_EXT))

%foreach x in $(SRC_FILES_ND)
%  if (exists($(x)))
     DP_SRC += $(x,M\.$(C_EXT)$$, R,>.$(OBJ_EXT_ND))
     DP_SRC += $(x,M\.$(ASM_EXT)$$, R,>.$(OBJ_EXT_ND))
     DP_SRC += $(x,M\.$(ASM_PP_EXT)$$, R,>.$(ASM_EXT))
%  endif
%endfor


DP       = dsmkmf

DP_FLAGS = -c -ds INCLUDE="$(INC_PATH)" CFLAGS="-D_DS1006"  \
           .TYPE.OBJ=".$(OBJ_EXT) .$(OBJ_EXT_ND) .$(ASM_EXT)" MKMF_SRCS="$(DP_SRC)"


# -----------------------------------------------------------------------
#  rules
# -----------------------------------------------------------------------

$(LIB)              .LIBRARY : $(OBJ_FILES)
                    %echo   archiving : $(.NEWSOURCES)
                    *$(AR) $(AR_FLAGS) $(LIB) $(.NEWSOURCES)


%.$(OBJ_EXT)        : %.$(C_EXT)
                    %echo   compiling $< to $@
#                    %echo $(CC) $(CC_FLAGS) $< -o $@
                    *$(CC) $(CC_FLAGS) $< -o $@


%.$(OBJ_EXT)        : %.$(ASM_EXT)
                    %echo   assembling $<
                    *$(AS) $(AS_FLAGS) -o $@ $<


%.$(ASM_EXT)        .PRECIOUS : %.$(ASM_PP_EXT)
                    %echo   preprocessing $<
                    *$(CPP) +zss $(INC_PATH,<$(CC_INC_OPT)) $(CPP_FLAGS) $<
                    copy $(@,R).ixx $(@,R).$(ASM_EXT) > NUL
                    del $(@,R).ixx


%.$(OBJ_EXT_ND)     .PRECIOUS : %.$(ASM_EXT)
                    %echo   assembling $<
                    *$(AS) $(AS_FLAGS) -o $@ $<


%.$(OBJ_EXT_ND)     .PRECIOUS : %.$(C_EXT)
                    %echo   compiling $<
                    *$(CC) $(CC_FLAGS) -o $@ $<


build:              $(LIB)


contents:
                    %echo contents of Carsim library $(LIB)
                    *$(AR) -t $(LIB)
