Linear_steer_input

f = logspace(-2,2,100);
w = 2*pi*f;

[mag1,ph1,w]=bode(MatrixA,MatrixB,MatrixC(1,:),MatrixD(1,:),1,w);
[mag2,ph2,w]=bode(MatrixA,MatrixB,MatrixC(2,:),MatrixD(2,:),1,w);

%Matching phases
ph1=ph1-ph1(1);
ph2=ph2-ph2(1);

%figure
scrsz = get(0,'ScreenSize');
figure('Position',[scrsz(3)/4 scrsz(4)/3 scrsz(3)/2 scrsz(4)/2])

subplot(2,2,1), ...
    semilogx(f,mag1,'-'),axis([0 40 -40 30]), axis autoy, ylabel('gain, rad/N-m-sec','fontsize',12),grid,
title('Bode plot: 60 kph, Steer Torque to Yaw Rate')
subplot(2,2,3), ...
   semilogx(f,ph1,'-'),axis([0 40 -200 200]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12)
grid

subplot(2,2,2), ...
    semilogx(f,mag2/9.81,'-'),axis([0 40 -40 30]), axis autoy, ylabel('gain, g/N-m','fontsize',12),grid,
title('Bode plot: 60 kph, Steer Torque to Ay')
subplot(2,2,4), ...
   semilogx(f,ph2,'-'),axis([0 40 -200 200]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12)
grid
