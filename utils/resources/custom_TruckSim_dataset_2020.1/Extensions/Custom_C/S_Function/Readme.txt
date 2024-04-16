Copyright (c) 2020. Mechanical Simulation Corporation. All rights reserved.

This readme provides steps to rebuild the S-Function "vs_sf_steer_control".

1. Download VS_SDK from Mechanical Simulation's website.

2. Unpack the VS_SDK to your computer.

3. Open Matlab and make sure you have Matlab's C/C++ compiler
   set. You can check this setup by running the following command:
     mex -setup
     
4. Navigate to the below path using a Matlab command window:
   <vs_sdk root>\VS_SDK\Libraries\vs_api\C\Examples\Projects\S_Function
   
5. Type "build" into the Matlab command window to rebuild the S-Function "vs_sf_steer_control". 
   If you have 64bit Matlab, a "vs_sf_steer_control.mexw64" will be created.
   If you have 32bit Matlab, a "vs_sf_steer_control.mexw32" will be created.
