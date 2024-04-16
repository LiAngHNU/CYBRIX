function y = lap_time(x)
% Mechanical Simulation Corporation
% October 2019

% Check if last loaded library is still in memory. If so, unload it.
if libisloaded('vs_solver')
  unloadlibrary('vs_solver');
end

% Scan simfile for DLL pathname
simfile = 'lap_time.sim';
SolverPath = vs_dll_path(simfile);

% Load the solver DLL
loadlibrary(SolverPath, 'vs_api_def_m.h', 'alias', 'vs_solver');

% Start and read normal inputs
t = calllib('vs_solver', 'vs_setdef_and_read', simfile, 0, 0); 

% Use VS commands to set x values
vs_statement('SPEED_CURV_LENGTH',num2str(x(1)));
vs_statement('TPREV_CONSTANT',num2str(x(2)));

% Initialize and run
calllib('vs_solver', 'vs_initialize', t, 0, 0);
stop = calllib('vs_solver', 'vs_error_occurred');
while ~stop
    [stop, t] = calllib('vs_solver', 'vs_integrate', t, 0);
end

% Retrieve lap time
nPtr = calllib('vs_solver','vs_get_var_ptr','SV_N_START_CROSS');
setdatatype(nPtr,'doublePtr',1,1);
n = nPtr.Value;
if n >= 2
    yPtr = calllib('vs_solver','vs_get_var_ptr','LAP_TIME');
    setdatatype(yPtr,'doublePtr',1,1);
    y = yPtr.Value; % Lap time
else
    y = 1000; % Penalize: did not cross finish line!
end

% Terminate solver
calllib('vs_solver', 'vs_terminate_run', t);

% Unload solver DLL
unloadlibrary('vs_solver');
end