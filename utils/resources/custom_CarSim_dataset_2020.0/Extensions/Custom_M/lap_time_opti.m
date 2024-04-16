% Mechanical Simulation Corporation
% October 2019

clearvars; close all; clc

% Initial solution
x0 = [50; % [m] SPEED_CURV_LENGTH
   0.25]; % [s] TPREV_CONSTANT

% Run the optimization routine 
options = optimset('Display', 'iter','PlotFcns',@optimplotfval);
tic
[x,fval] = fminsearch(@(x)lap_time(x),x0,options);
toc

% Print results to MATLAB console
disp('Final values:')
disp(['SPEED_CURV_LENGTH = ' num2str(x(1)) ' [m] (was ' num2str(x0(1)) ')'])
disp(['TPREV_CONSTANT = ' num2str(x(2)) ' [s] (was ' num2str(x0(2)) ')'])

