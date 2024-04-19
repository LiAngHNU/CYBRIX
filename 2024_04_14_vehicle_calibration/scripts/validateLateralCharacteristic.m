%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%  __________________________________________________________ %%%%%%%
%%%%%%  /\  \      /\__\      /\  \      /\  \      /\  \      /\__\ %%%%%%
%%%%%  /::\  \    |::L__L    /::\  \    /::\  \    _\:\  \    |::L__L %%%%%
%%%%  /:/\:\__\___|:::\__\__/::\:\__\__/::\:\__\__/\/::\__\__/::::\__\ %%%%
%%%%  \:\ \/__/   /:;;/__/  \:\::/  /  \;:::/  /  \::/\/__/  \;::;/__/ %%%%
%%%%%  \:\__\     \/__/      \::/  /    |:\/__/    \:\__\     |::|__| %%%%%
%%%%%%  \/__/_________________\/__/______\|__|______\/__/______\/__/ %%%%%%
%%%%%%%   @ Vehicle Lateral Calibration Validator                   %%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
VEH_TYPE = '2_Axle_Tractor_Type_M_Electric';                              %
MDL_TYPE = 'sine_exported_models';                                        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialize Model %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_10.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_20.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_30.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_40.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_50.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_60.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_70.mat'));%
load(strcat('results\',VEH_TYPE,'\','lateral\',MDL_TYPE,'\','ss_80.mat'));%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialize Model %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
vx_table  = [        2.78,         5.56,         8.33,        11.11,...   %
                    13.88,        16.67,        19.44,        22.22;];    %
a11_table = [ss_10.A(1,1), ss_20.A(1,1), ss_30.A(1,1), ss_40.A(1,1),...   %
             ss_50.A(1,1), ss_60.A(1,1), ss_70.A(1,1), ss_80.A(1,1);];    %
a12_table = [ss_10.A(1,2), ss_20.A(1,2), ss_30.A(1,2), ss_40.A(1,2),...   %
             ss_50.A(1,2), ss_60.A(1,2), ss_70.A(1,2), ss_80.A(1,2);];    %
a21_table = [ss_10.A(2,1), ss_20.A(2,1), ss_30.A(2,1), ss_40.A(2,1),...   %
             ss_50.A(2,1), ss_60.A(2,1), ss_70.A(2,1), ss_80.A(2,1);];    %
a22_table = [ss_10.A(2,2), ss_20.A(2,2), ss_30.A(2,2), ss_40.A(2,2),...   %
             ss_50.A(2,2), ss_60.A(2,2), ss_70.A(2,2), ss_80.A(2,2);];    %
b11_table = [ss_10.B(1,1), ss_20.B(1,1), ss_30.B(1,1), ss_40.B(1,1),...   %
             ss_50.B(1,1), ss_60.B(1,1), ss_70.B(1,1), ss_80.B(1,1);];    %
b21_table = [ss_10.B(2,1), ss_20.B(2,1), ss_30.B(2,1), ss_40.B(2,1),...   %
             ss_50.B(2,1), ss_60.B(2,1), ss_70.B(2,1), ss_80.B(2,1);];    %
tiledlayout(1,2,'TileSpacing','compact','Padding','compact');             %
nexttile(1);hold on;grid on;set(gca,'XMinorGrid','on','YMinorGrid','on'); %
title('Matrix A');xlabel('Vx/[m/s]');ylabel('Coefficients/[-]');          %
plot(vx_table,a11_table);               plot(vx_table,a12_table);         %
plot(vx_table,a21_table);               plot(vx_table,a22_table);         %
nexttile(2);hold on;grid on;set(gca,'XMinorGrid','on','YMinorGrid','on'); %
title('Matrix B');xlabel('Vx/[m/s]');ylabel('Coefficients/[-]');          %
plot(vx_table,b11_table);               plot(vx_table,b21_table);         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Validate Model %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
u = [out.this_uy.Data]';                v = [out.this_vx.Data]';          %
x_ref = [out.this_vy.Data,out.this_avz.Data]';                            %
x_pre = zeros(2,length(u));             e_pre = zeros(2,length(u));       %
t = [0.01:0.01:(length(u)/100)];        ts = 0.01;                        %
for i = 2:1:length(t)                                                     %
    a11 = interp1(vx_table,a11_table,v(1,i),'linear','extrap');           %
    a12 = interp1(vx_table,a12_table,v(1,i),'linear','extrap');           %
    a21 = interp1(vx_table,a21_table,v(1,i),'linear','extrap');           %
    a22 = interp1(vx_table,a22_table,v(1,i),'linear','extrap');           %
    b11 = interp1(vx_table,b11_table,v(1,i),'linear','extrap');           %
    b21 = interp1(vx_table,b21_table,v(1,i),'linear','extrap');           %
    A = [a11,a12;a21,a22;];             B = [b11;b21;];                   %
    x_pre(:,i) = A * x_pre(:,i-1) + B * u(:,i-1);                         %
    e_pre(:,i) = x_pre(:,i) - x_ref(:,i);                                 %
end                                                                       %
tiledlayout(1,2,'TileSpacing','compact','Padding','compact');             %
nexttile(1); hold on; grid on; title('Lateral Velocity/[m/s]');           %
set(gca,'XMinorGrid','on','YMinorGrid','on'); xlabel('Time/[s]');         %
set(gca,'XMinorTick','on','YMinorTick','on'); ylabel('Vy/[m/s]');         %
plot(t,x_ref(1,:),'b:','LineWidth',1);                                    %
plot(t,x_pre(1,:),'m-','LineWidth',1);                                    %
plot(t,e_pre(1,:),'r-','LineWidth',1);                                    %
legend('measured Vy','predicted Vy','predicted error'); xlim([0 t(end)]); %
nexttile(2); hold on; grid on; title('Angular Velocity/[rad/s]');         %
set(gca,'XMinorGrid','on','YMinorGrid','on'); xlabel('Time/[s]');         %
set(gca,'XMinorTick','on','YMinorTick','on'); ylabel('AVz/[rad/s]');      %
plot(t,x_ref(2,:),'b:','LineWidth',1);                                    %
plot(t,x_pre(2,:),'m-','LineWidth',1);                                    %
plot(t,e_pre(2,:),'r-','LineWidth',1);                                    %
legend('measured AVz','predicted AVz','predicted error');xlim([0 t(end)]);%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ version: v1.00              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ author:  Li.Ang             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ email:   ang1997@hnu.edu.cn %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@ date:    2024-04-18         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%