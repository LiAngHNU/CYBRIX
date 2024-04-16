% To calculate and plot the results

time_final = length(timet)*time_step;
plotstart=1; plotend=time_final*(1/time_step);

mpg=distance_in_mile(end)/fuel_consum_in_g(end)*1000*3.8*0.75;

figure(1);
clf;
subplot(2,1,1);
plot(timet(plotstart:plotend),demand_spd(plotstart:plotend),'r:',timet(plotstart:plotend),...
    actual_spd(plotstart:plotend),'b-',timet(plotstart:plotend),actual_spd(plotstart:plotend),'b-.','LineWidth',2);
hold on;
plot(timet(plotstart:plotend),ControlMode(plotstart:plotend)*10,'k-','LineWidth',2);
hold off;
ylabel('Vehicle speed (mph)','fontWeight','bold','fontSize',12);
title('Control Power & Mode','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
subplot(2,1,2);
plot(timet(plotstart:plotend),P_d(plotstart:plotend),...
    'c-',timet(plotstart:plotend),P_ch(plotstart:plotend),'r-','LineWidth',2);
hold on;
plot(timet(plotstart:plotend),P_engine_d(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen_d(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot_d(plotstart:plotend),'k-.','LineWidth',2);
plot(timet(plotstart:plotend),P_engine_d(plotstart:plotend)+P_mot_d(plotstart:plotend)+P_gen_d(plotstart:plotend),'y-.','LineWidth',2);
hold off;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
ylabel('Demand power (W)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;

figure(2);
clf;
subplot(2,1,1);
hold on;
plot(timet(plotstart:plotend),W_engine(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),W_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),W_mot(plotstart:plotend),'k-.','LineWidth',2);
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('speed (RPM)','fontWeight','bold','fontSize',12);
grid;
subplot(2,1,2);
plot(timet(plotstart:plotend),P_d(plotstart:plotend),...
    'c-',timet(plotstart:plotend),P_ch(plotstart:plotend),'r-','LineWidth',2);
hold on;
plot(timet(plotstart:plotend),P_engine_d(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen_d(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot_d(plotstart:plotend),'k-.','LineWidth',2);
hold off;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
ylabel('demand power (W)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;


figure(3);
clf;
subplot(2,1,1);
hold on;
plot(timet(plotstart:plotend),T_engine(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),T_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),T_mot(plotstart:plotend),'k-.','LineWidth',2);
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('torque (Nm)','fontWeight','bold','fontSize',12);
grid;
subplot(2,1,2);
plot(timet(plotstart:plotend),P_d(plotstart:plotend),...
    'c-',timet(plotstart:plotend),P_ch(plotstart:plotend),'r-','LineWidth',2);
hold on;
plot(timet(plotstart:plotend),P_engine_d(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen_d(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot_d(plotstart:plotend),'k-.','LineWidth',2);
hold off;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
ylabel('demand power (W)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;

figure(4);
clf;
subplot(2,1,1);
hold on;
plot(timet(plotstart:plotend),P_engine(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot(plotstart:plotend),'k-.','LineWidth',2);
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('actual power (kW)','fontWeight','bold','fontSize',12);
grid;
subplot(2,1,2);
plot(timet(plotstart:plotend),P_d(plotstart:plotend),...
    'c-',timet(plotstart:plotend),P_ch(plotstart:plotend),'r-','LineWidth',2);
hold on;
plot(timet(plotstart:plotend),P_engine_d(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen_d(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot_d(plotstart:plotend),'k-.','LineWidth',2);
hold off;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
ylabel('demand power (W)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;

figure(5);
clf;
subplot(2,1,1);
hold on;
plot(timet(plotstart:plotend),P_engine(plotstart:plotend),'b-.','LineWidth',2);
plot(timet(plotstart:plotend),P_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_mot(plotstart:plotend),'k-.','LineWidth',2);
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('actual power (kW)','fontWeight','bold','fontSize',12);
grid;
subplot(2,1,2);
[AX,H1,H2] = plotyy(timet(plotstart:plotend),P_batt(plotstart:plotend),...
    timet(plotstart:plotend),output_soc(plotstart:plotend),'plot');
set(get(AX(1),'Ylabel'),'String','battery power (kW)','fontWeight','bold','fontSize',12)
set(get(AX(2),'Ylabel'),'String','SOC','fontWeight','bold','fontSize',12)
set(H1,'LineWidth',2);
set(H2,'LineWidth',2);
set(AX(1),'YLim',[-50 60])
set(AX(2),'fontWeight','bold','fontSize',12)
xlabel('time (sec)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;

figure(6);
clf;
plot(W_engine(plotstart:20:plotend),T_engine(plotstart:20:plotend),'ms','LineWidth',2);
title('Engine operating points','fontWeight','bold','fontSize',12);
xlabel('Speed (RPM)','fontWeight','bold','fontSize',12);
ylabel('Torque (Nm)','fontWeight','bold','fontSize',12);
set(gca,'fontSize',12,'fontWeight','bold')
grid;

figure(7);
clf;
hold on;
plot(timet(plotstart:plotend),P_engine(plotstart:plotend),'b-','LineWidth',2);
plot(timet(plotstart:plotend),P_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),P_ring(plotstart:plotend)/1000,'y-.','LineWidth',2);
plot(timet(plotstart:plotend),-P_ring(plotstart:plotend)/1000-P_gen(plotstart:plotend),'r-','LineWidth',2);
plot(timet(plotstart:plotend),actual_spd(plotstart:plotend),'b:');
plot(timet(plotstart:plotend),ControlMode(plotstart:plotend)*10-60,'k-');
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('Power (kW)','fontWeight','bold','fontSize',12);
grid;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
title('Power Flow','fontWeight','bold','fontSize',12);
legend('P_{engine}','P_{generator}','P_{ring}','P_{generator}+P_{ring}');

figure(8);
clf;
hold on;
plot(timet(plotstart:plotend),T_engine(plotstart:plotend),'b-','LineWidth',2);
plot(timet(plotstart:plotend),T_gen(plotstart:plotend),'g-.','LineWidth',2);
plot(timet(plotstart:plotend),T_ring(plotstart:plotend),'y-.','LineWidth',2);
plot(timet(plotstart:plotend),-T_ring(plotstart:plotend)-T_gen(plotstart:plotend),'r-','LineWidth',2);
plot(timet(plotstart:plotend),actual_spd(plotstart:plotend),'b:');
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('Torque (Nm)','fontWeight','bold','fontSize',12);
grid;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
title('Torque Flow','fontWeight','bold','fontSize',12);
legend('T_{engine}','T_{generator}','T_{ring}','T_{generator}+T_{ring}');


figure(9);
clf;
hold on;
plot(timet(plotstart:plotend),P_engine(plotstart:plotend),'r--','LineWidth',2);
plot(timet(plotstart:plotend),P_batt(plotstart:plotend),'m-.','LineWidth',2);
plot(timet(plotstart:plotend),actual_spd(plotstart:plotend)*1.1,'b-','LineWidth',2);
hold off;
set(gca,'fontSize',12,'fontWeight','bold')
ylabel('Power (kW)','fontWeight','bold','fontSize',12);
grid;
xlabel('time (sec)','fontWeight','bold','fontSize',12);
legend('P_{engine}','P_{battery}');


figure(10);
clf;
plot(timet(plotstart:plotend),demand_spd(plotstart:plotend),...
    'r:',timet(plotstart:plotend),actual_spd(plotstart:plotend),...
    'b-',timet(plotstart:plotend),actual_spd(plotstart:plotend),'g-.','LineWidth',2);
set(gca,'fontSize',12,'fontWeight','bold')
xlabel('time (sec)','fontWeight','bold','fontSize',12);
title(['Main Results:  Total travel ',num2str(distance_in_mile(end)),...
        ' Miles;  Fuel ',num2str(mpg),' MPG.'],'fontWeight','bold','fontSize',12);
hold on;
[AX,H1,H2] = plotyy(timet(plotstart:plotend),actual_spd(plotstart:plotend),...
    timet(plotstart:plotend),output_soc(plotstart:plotend),'plot');
set(get(AX(1),'Ylabel'),'String','Vehicle Speed (MPH)','fontWeight','bold','fontSize',12)
set(get(AX(2),'Ylabel'),'String','State of Charge','fontWeight','bold','fontSize',12)
set(H2,'LineStyle','-.')
set(H2,'LineWidth',2)
set(AX(2),'fontSize',12,'fontWeight','bold')
set(gca,'fontSize',12,'fontWeight','bold')
legend('Reference Speed','Actual Speed','SOC')
grid;
