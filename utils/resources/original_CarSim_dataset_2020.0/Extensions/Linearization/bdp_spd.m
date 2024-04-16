f = logspace(-2,2,100);
w = 2*pi*f;

Linear_30kph
[mag1_0,ph1_0,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_0,ph2_0,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

Linear_60kph
[mag1_1,ph1_1,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_1,ph2_1,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

Linear_90kph
[mag1_2,ph1_2,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_2,ph2_2,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

Linear_120kph
[mag1_3,ph1_3,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_3,ph2_3,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

Linear_150kph
[mag1_4,ph1_4,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_4,ph2_4,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

Linear_180kph
[mag1_5,ph1_5,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2_5,ph2_5,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

%Matching phases
ph1_0=ph1_0-ph1_0(1);
ph2_0=ph2_0-ph2_0(1);
ph1_1=ph1_1-ph1_1(1);
ph2_1=ph2_1-ph2_1(1);
ph1_2=ph1_2-ph1_2(1);
ph2_2=ph2_2-ph2_2(1);
ph1_3=ph1_3-ph1_3(1);
ph2_3=ph2_3-ph2_3(1);
ph1_4=ph1_4-ph1_4(1);
ph2_4=ph2_4-ph2_4(1);
ph1_5=ph1_5-ph1_5(1);
ph2_5=ph2_5-ph2_5(1);

scrsz = get(0,'ScreenSize');
figure('Position',[scrsz(3)/4 scrsz(4)/3 scrsz(3)/2 scrsz(4)/2])

% Gain AVz
subplot(2,2,1), ...
    p=semilogx(f,20*log10(mag1_0),'b-');axis([0 40 -40 30]), axis autoy, ylabel('gain, dB (0dB : 1/s)','fontsize',12),
title('Bode plot: Steer to Yaw Rate (30-180 kph)'),
set(p,'LineWidth',4);
hold on;
subplot(2,2,1), ...
    p=semilogx(f,20*log10(mag1_1),'r--');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,1), ...
    p=semilogx(f,20*log10(mag1_2),'g-.');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,1), ...
    p=semilogx(f,20*log10(mag1_3),'r:');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,1), ...
    semilogx(f,20*log10(mag1_4),'mo'),axis([0 40 -40 30]), axis autoy
subplot(2,2,1), ...
    semilogx(f,20*log10(mag1_5),'C+'),axis([0 40 -40 30]), axis autoy
grid

% Phase AVz
subplot(2,2,3), ...
   p=semilogx(f,ph1_0,'b-');axis([0 40 -200 200]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12),
set(p,'LineWidth',4);
hold on;
subplot(2,2,3), ...
   p=semilogx(f,ph1_1,'r--');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,3), ...
   p=semilogx(f,ph1_2,'g-.');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,3), ...
   p=semilogx(f,ph1_3,'r:');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,3), ...
   semilogx(f,ph1_4,'mo');axis([0 40 -200 200]), axis autoy
subplot(2,2,3), ...
   semilogx(f,ph1_5,'C+');axis([0 40 -200 200]), axis autoy  
grid

% Gain Ay
subplot(2,2,2), ...
    p=semilogx(f,20*log10(mag2_0*pi/9.81/180),'b-');axis([0 40 -40 30]), axis autoy, ylabel('gain, dB (0dB : g/deg)','fontsize',12),
title('Bode plot: Steer to Ay (30-180 kph)'),
set(p,'LineWidth',4);
hold on;
subplot(2,2,2), ...
    p=semilogx(f,20*log10(mag2_1*pi/9.81/180),'r--');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,2), ...
    p=semilogx(f,20*log10(mag2_2*pi/9.81/180),'g-.');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,2), ...
    p=semilogx(f,20*log10(mag2_3*pi/9.81/180),'r:');axis([0 40 -40 30]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,2), ...
    semilogx(f,20*log10(mag2_4*pi/9.81/180),'mo');axis([0 40 -40 30]), axis autoy,
subplot(2,2,2), ...
    semilogx(f,20*log10(mag2_5*pi/9.81/180),'c+');axis([0 40 -40 30]), axis autoy
grid

% Phase Ay
subplot(2,2,4), ...
   p=semilogx(f,ph2_0,'b-');axis([0 40 -200 200]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12),
set(p,'LineWidth',4);
hold on;
subplot(2,2,4), ...
   p=semilogx(f,ph2_1,'r--');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,4), ...
   p=semilogx(f,ph2_2,'g-.');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,4), ...
   p=semilogx(f,ph2_3,'r:');axis([0 40 -200 200]), axis autoy,
set(p,'LineWidth',4);
subplot(2,2,4), ...
   semilogx(f,ph2_4,'mo');axis([0 40 -200 200]), axis autoy,
subplot(2,2,4), ...
   semilogx(f,ph2_5,'c+');axis([0 40 -200 200]), axis autoy
grid

hold off;