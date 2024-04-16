% Spectrum analysis program
% Written by Yukio Watanabe for CarSim version 2016 at Mechanical Simulation, 
% Ann Arbor, U.S.A. Last modified on December 1, 2015

% Sampling time (T) and sampling frequency (Fs)
T=Time(2)-Time(1);
Fs=1/T;
% Data length (L) and number of FFT points (NFFT)
L=length(M_SW);
NFFT=2^nextpow2(L);
% FFT for simulation data (steer, yaw rate and latacc)
Y_s=fft(M_SW,NFFT);
Y_avz=fft(AVz,NFFT);
Y_ay=fft(Ay,NFFT);
% Power spectrum density
Pyy_s = Y_s.*conj(Y_s)/NFFT/pi/2; 
% Frequency series
f=Fs/2*linspace(0,1,NFFT/2+1);

% Gain
Gain_avz_s = Y_avz./Y_s;
G_avz_s=sqrt(Gain_avz_s.*conj(Gain_avz_s));
Gain_ay_s = Y_ay./Y_s;
G_ay_s=sqrt(Gain_ay_s.*conj(Gain_ay_s));

% Phase
Ph_avz_s = atan2(imag(Gain_avz_s),real(Gain_avz_s))/pi*180;
Ph_ay_s = atan2(imag(Gain_ay_s),real(Gain_ay_s))/pi*180;

% Averaging output frequencies with log spacing (bands/octave)
ff=f(1);
PP_s=Pyy_s(1);
GG_avz=G_avz_s(1);
PPh_avz=Ph_avz_s(1);
GG_ay=G_ay_s(1);
PPh_ay=Ph_ay_s(1);
bands = 32; %Bands should be power of two number (e.g. 16, 32, 64, 128...)
for k = 1:nextpow2(L)-1
    j = (2^k-2^(k-1))/bands;
    
    if (j>=1)
        for m = 1:bands
            f_ave = 0;
            P_ave = 0;
            G_avz_ave = 0;
            Ph_avz_ave = 0;
            G_ay_ave = 0;
            Ph_ay_ave = 0;

            for n = 1:j
                f_ave = f_ave+ f(2^(k-1)+(m-1)*j+n);
                P_ave = P_ave+ Pyy_s(2^(k-1)+(m-1)*j+n);
                G_avz_ave = G_avz_ave+ G_avz_s(2^(k-1)+(m-1)*j+n);
                Ph_avz_ave = Ph_avz_ave+ Ph_avz_s(2^(k-1)+(m-1)*j+n);
                G_ay_ave = G_ay_ave+ G_ay_s(2^(k-1)+(m-1)*j+n);
                Ph_ay_ave = Ph_ay_ave+ Ph_ay_s(2^(k-1)+(m-1)*j+n); 
            end

            f_ave = f_ave/j;
            P_ave = P_ave/j;
            G_avz_ave = G_avz_ave/j;
            Ph_avz_ave = Ph_avz_ave/j;
            G_ay_ave = G_ay_ave/j;
            Ph_ay_ave = Ph_ay_ave/j;

            ff = [ff f_ave];
            PP_s = [PP_s P_ave];
            GG_avz = [GG_avz G_avz_ave];
            PPh_avz = [PPh_avz Ph_avz_ave];
            GG_ay = [GG_ay G_ay_ave];
            PPh_ay = [PPh_ay Ph_ay_ave];
        end        
    else
        i = 2^k-2^(k-1);
        for m = 1:i
            ff = [ff f(2^(k-1)+m)];
            PP_s = [PP_s Pyy_s(2^(k-1)+m)];
            GG_avz = [GG_avz G_avz_s(2^(k-1)+m)];
            PPh_avz = [PPh_avz Ph_avz_s(2^(k-1)+m)]; 
            GG_ay = [GG_ay G_ay_s(2^(k-1)+m)];
            PPh_ay = [PPh_ay Ph_ay_s(2^(k-1)+m)]; 
        end
    end
end

PPh_avz=PPh_avz-PPh_avz(1);
PPh_ay=PPh_ay-PPh_ay(1);

%figure
scrsz = get(0,'ScreenSize');
figure('Position',[scrsz(3)/10 scrsz(4)/3 scrsz(3)*4/5 scrsz(4)/2])

subplot(2,3,1), ...
    plot(Time,M_SW,'-'),axis([0 100 -5 5]), xlabel('time, sec','fontsize',12), ylabel('Steer torque, N-m','fontsize',12),grid,
title('FFT: Steer torque input')
subplot(2,3,4), ...
   loglog(ff,PP_s,'-'),axis([0.01 10 0.0001 1000]), xlabel('frequency, Hz','fontsize',12),ylabel('PSD of steer torque, N2m2/Hz','fontsize',12)
grid

subplot(2,3,2), ...
    semilogx(ff,GG_avz/180*pi,'-'),axis([0.01 10 0 0.06]), ylabel('gain, rad/N-m-sec','fontsize',12),grid,
title('Bode plot: Steer torque to Yaw Rate')
subplot(2,3,5), ...
   semilogx(ff,PPh_avz,'-'),axis([0.01 10 -150 0]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12)
grid

subplot(2,3,3), ...
    semilogx(ff,GG_ay,'-'),axis([0.01 10 0 0.1]), ylabel('gain, g/N-m','fontsize',12),grid,
title('Bode plot: Steer torque to Ay')
subplot(2,3,6), ...
   semilogx(ff,PPh_ay,'-'),axis([0.01 10 -150 0]), axis autoy, xlabel('frequency, Hz','fontsize',12),ylabel('phase, deg','fontsize',12)
grid

