% This initialization file was originally made by the University of Michigan 
% on Feb. 25, 2005.  

% For rule based control

clear all;			% Initialize workspace

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%load initialization data file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
hybrid1_sim_data;
disp('Data loaded sucessfully!');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Simulation initial conditions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ess_init_soc=0.6;   %initial battery state of charge (1.0 = 100% charge)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% driver controller parameters
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% keep unchanged
Kf_c = 1/10;
Kp_c = 30;
Ti_c = 60;
Tt_c = 65;
v_max_c = 100;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ENGINE CONTROL
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%lookup map for calculate maxmum engine driving power
eng_ctrl_spd=[104.7 110 115.2 120.4 125.7 130.9 136.1 141.4 ...
            146.6 151.8 157.1 162.3 167.6 172.8 178 183.3 188.5 193.7 199 ...
            204.2 209.4 214.7 219.9 225.1 230.4 235.6 240.9 246.1 251.3 256.6 ...
            261.8 267 272.3 277.5 282.7 288 293.2 298.5 303.7 308.9 314.2 ...
            319.4 324.6 329.9 335.1 340.3 345.6 350.8 356 361.3 366.5 371.8 ...
            377 382.2 387.5 392.7 397.9 403.2 408.4 413.6 418.9];
eng_ctrl_max_trq=[77.33 78.2 79.07 79.94 80.81 81.68 82.35 82.85 83.35 83.85 ...
            84.35 84.83 85.2 85.58 85.95 86.32 86.7 87.13 87.6 88.08 88.55 ...
            89.02 89.47 89.79 90.11 90.43 90.76 91.08 91.4 91.73 92.05 92.38 ...
            92.7 93.03 93.35 93.67 93.99 94.32 94.64 94.96 95.29 95.61 95.93 ...
            96.26 96.58 96.9 97.22 97.55 97.87 98.19 98.52 98.84 99.17 99.49 ...
            99.81 100.2 100.5 100.9 101.3 101.7 102];

%base on engine optimal speed map in Advisor
%proved by later engine operating discovery
eng_spd_opt=eng_ctrl_spd;
eng_pwr_opt=eng_spd_opt.*eng_ctrl_max_trq*0.9; % to smooth the control curve, we apply 0.9 throttle, close to the optimized curve
eng_pwr_max=40000; %engine max power (W)

eng_idle_spd=1000*pi/30;  % (rad/s), engine's idle speed
max_eng_spd=4000*pi/30; %rad/s
%eng_off_time=5;  %(guess)% (sec), before lasting this time, engine cant be totally shut off. needs to be set at engine's idle speed

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GENERATOR CONTROL
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
max_gen_spd=6500*pi/30; %rad/s
min_gen_spd=-6500*pi/30; %rad/s
%Generator Speed PI Control (base on PSAT)
gc_kp_on= 0.9;        % Proportional Factor Controller gains  N*m/(rad/s) 
gc_ki_on=0.005;       % Integration Factor  N*m/(rad/s)/sec       

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BATTERY CONTROL
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% SOC boundaries
high_soc=0.75;  % highest desired battery state of charge
low_soc=0.50;   % below this value, the engine must be on and charge
stop_soc=0.45;  % lowest desired battery state of charge, avoid reaching this point
regstop_soc=0.8;  % reach this point, regenerative brake will stop
soc_control=0.55;   % for charging mode control
target_soc=0.6; % try to maintain the soc 



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Rule Base Control Strategy
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%in ADVISOR
%eng_launch_spd=45*1000/3600; % 9/6/00 from recent files looks like 28 mph (45 kph) is max speed with which engine can be off.
w_engine_on = 30*1600/3600/R_tire; % (rad/s) (30 mph Guess)
P_ev_mode = 12000; %(w) (Guess)
P_eng_mode = 40000; %(w)
%P_engbat_mode = 55000; %(w)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Simulation and Results
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
time_step = 0.025;  %Time step for plot
%sim('hybrid1.mdl');
%display('Simulation completed!');
%hybrid1_sim_plot;