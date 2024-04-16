%TITLE Big SUV Full-Time 4WD, Front diff. and Rear Diff are not locked.

%
TSTEP          =  0.001;% Time step for numerical integration (s)
%
AV_ENG_IDLE      =  675;% Engine idle speed (rpm)
IENG            =  1.15;% Spin inertia of engine crankshaft (kg-m2)
ITC_INPUT_SHAFT    =  0.015;% Spin inertia of input shaft of torque converter (kg-m2)
ITC_OUTPUT_SHAFT    =  0.1;% Spin inertia of output shaft of torque converter (kg-m2)
%
LOCKED_FD_DAMP    = 0.8;% Torsional damping constant for locking the front
                     %   differential (N-m-s/deg)
LOCKED_FD_K       = 80;% Torsional spring constant for locking the front
                     %   differential (N-m/deg)
LOCKED_FD_OPT     =  0;% Is the front differential locked?: 0 -> no, 1 -> yes
LOCKED_RD_DAMP    = 0.8;% Torsional damping constant for locking the rear
                     %   differential (N-m-s/deg)
LOCKED_RD_K       = 80;% Torsional spring constant for locking the rear
                     %   differential (N-m/deg)
LOCKED_RD_OPT     =  0;% Is the rear differential locked?: 0 -> no, 1 -> yes
LOCKED_XC_DAMP    = 0.8;% Torsional damping constant for locking the transfer case
                     %   (N-m-s/deg)
LOCKED_XC_K       = 80;% Torsional spring constant for locking the transfer case
                     %   (N-m/deg)
LOCKED_XC_OPT     =  0;% Is the transfer case locked?: 0 -> no, 1 -> yes
%
R_EFF_F_FD         =  0.99;% Forward efficiency of front differential (-)
R_EFF_F_RD         =  0.99;% Forward efficiency of rear differential (-)
R_EFF_F_XC         =  0.99;% Forward efficiency of transfer case (-)
R_EFF_R_FD       =  0.99;% Reverse efficiency of front differential (-)
R_EFF_R_RD       =  0.99;% Reverse efficiency of rear differential (-)
R_EFF_R_XC       =  0.99;% Reverse efficiency of transfer case (-)
R_EFF_TR_F(1)    =  0.92;% 1st-gear forward efficiency of transmission (-)
R_EFF_TR_F(2)    =  0.92;% 2nd-gear forward efficiency of transmission (-)
R_EFF_TR_F(3)    =  0.92;% 3rd-gear forward efficiency of transmission (-)
R_EFF_TR_F(4)    =  0.92;% 4th-gear forward efficiency of transmission (-)
R_EFF_TR_F(5)    =  0.92;% 5th-gear forward efficiency of transmission (-)
R_EFF_TR_F(6)    =  0.92;% 6th-gear forward efficiency of transmission (-)
R_EFF_TR_F(7)    =  0.92;% 7th-gear forward efficiency of transmission (-)
R_EFF_TR_F_REVERSE =  0.9;% Reverse-gear forward efficiency of transmission (-)
R_EFF_TR_R(1)    =  0.92;% 1st-gear reverse efficiency of transmission (-)
R_EFF_TR_R(2)    =  0.92;% 2nd-gear reverse efficiency of transmission (-)
R_EFF_TR_R(3)    =  0.92;% 3rd-gear reverse efficiency of transmission (-)
R_EFF_TR_R(4)    =  0.92;% 4th-gear reverse efficiency of transmission (-)
R_EFF_TR_R(5)    =  0.92;% 5th-gear reverse efficiency of transmission (-)
R_EFF_TR_R(6)    =  0.92;% 6th-gear reverse efficiency of transmission (-)
R_EFF_TR_R(7)    =  0.92;% 7th-gear reverse efficiency of transmission (-)
R_EFF_TR_R_REVERSE =  0.9;% Reverse-gear reverse efficiency of transmission (-)
R_GEAR_FD        =  5.0;% Gear ratio of front differential (-)
R_GEAR_RD        =  5.0;% Gear ratio of rear differential (-)
R_GEAR_TR(1)     =  7.59;% 1st-gear ratio of transmission (-)
R_GEAR_TR(2)    =  5.06;% 2nd-gear ratio of transmission (-)
R_GEAR_TR(3)     =  3.38;% 3rd-gear ratio of transmission (-)
R_GEAR_TR(4)       =  2.25;% 4th-gear ratio of transmission (-)
R_GEAR_TR(5)     =  1.5;% 5th-gear ratio of transmission (-)
R_GEAR_TR(6)     =  1.0;% 6th-gear ratio of transmission (-)
R_GEAR_TR(7)     =  0.75;% 7th-gear ratio of transmission (-)
R_GEAR_TR_REVERSE  = -3.58;% Reverse-gear ratio of transmission (should be negative)
                       %   (-)
R_GEAR_XC          =  1;% Gear ratio of transfer case (-)
R_REAR_XC        =  0.5;% Fraction: transfer case torque on rear driveshaft (-)
%