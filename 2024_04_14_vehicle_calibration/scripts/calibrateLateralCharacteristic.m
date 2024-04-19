%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%  __________________________________________________________ %%%%%%%
%%%%%%  /\  \      /\__\      /\  \      /\  \      /\  \      /\__\ %%%%%%
%%%%%  /::\  \    |::L__L    /::\  \    /::\  \    _\:\  \    |::L__L %%%%%
%%%%  /:/\:\__\___|:::\__\__/::\:\__\__/::\:\__\__/\/::\__\__/::::\__\ %%%%
%%%%  \:\ \/__/   /:;;/__/  \:\::/  /  \;:::/  /  \::/\/__/  \;::;/__/ %%%%
%%%%%  \:\__\     \/__/      \::/  /    |:\/__/    \:\__\     |::|__| %%%%%
%%%%%%  \/__/_________________\/__/______\|__|______\/__/______\/__/ %%%%%%
%%%%%%%   @ Vehicle Lateral Calibration Processor                   %%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Calibration Settings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
VEL_TEST = 80;                                                            %
MDL_NAME = ['ss_',num2str(VEL_TEST)];                                     %
VEH_TYPE = '2_Axle_Tractor_Type_M_Electric';                              %
MDL_TYPE = 'step_exported_models';                                        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Process Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
t = [0.00:0.01:50.00]';                 ts = 0.01;                        %
                                                                          %
this_uy = out.this_uy.Data;                                               %
this_vx = out.this_vx.Data;             this_vy = out.this_vy.Data;       %
this_avx = out.this_avx.Data;           this_avz = out.this_avz.Data;     %
                                                                          %
last_uy = out.this_uy.Data;             last_uy(1,1) = this_uy(1,1);      %
last_vx = out.this_vx.Data;             last_vx(1,1) = this_vx(1,1);      %
last_vy = out.this_vy.Data;             last_vy(1,1) = this_vy(1,1);      %
last_avx = out.this_avx.Data;           last_avx(1,1) = this_avx(1,1);    %
last_avz = out.this_avz.Data;           last_avz(1,1) = this_avz(1,1);    %
                                                                          %
u = [last_uy];                          y = [this_vy,this_avz];           %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Process Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ss_data = iddata(y,u,ts);                                                 %
ss_est  = ssest(ss_data,2, ...                                            %
                'Ts',ts,...                                               %
                'Form','canonical', ...                                   %
                'DisturbanceModel','none');                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Data Validation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
y_est = lsim(ss_est,u,t);              e_est = y_est - y;                 %
tiledlayout(1,2,'TileSpacing','compact','Padding','compact');             %
nexttile(1);hold on;grid on;set(gca,'XMinorGrid','on','YMinorGrid','on'); %
title('Output 1');xlabel('Time/[s]');ylabel('Lateral Velocity/[m/s]');    %
plot(t,u); plot(t,y(:,1)); plot(t,y_est(:,1)); plot(t,e_est(:,1));        %
nexttile(2);hold on;grid on;set(gca,'XMinorGrid','on','YMinorGrid','on'); %
title('Output 2');xlabel('Time/[s]');ylabel('Yaw Rate/[rad/s]');          %
plot(t,u); plot(t,y(:,2)); plot(t,y_est(:,2)); plot(t,e_est(:,2));        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Save Model %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eval([MDL_NAME,'=ss_est']);                                               %
save(strcat('results\',VEH_TYPE,'\', ...                                  %
            'lateral\',MDL_TYPE,'\',MDL_NAME,'.mat'),MDL_NAME);           %
clear;                                                                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ version: v1.00              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ author:  Li.Ang             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ email:   ang1997@hnu.edu.cn %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ date:    2024-04-17         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%