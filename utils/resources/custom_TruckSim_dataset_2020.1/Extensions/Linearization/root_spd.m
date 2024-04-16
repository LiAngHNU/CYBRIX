clear

% Perform eigen analysis for multiple files
Linear_20kph
eig2 = eig(MatrixA);
Linear_30kph
eig3 = eig(MatrixA);
Linear_40kph
eig4 = eig(MatrixA);
Linear_50kph
eig5 = eig(MatrixA);
Linear_60kph
eig6 = eig(MatrixA);
Linear_70kph
eig7 = eig(MatrixA);
Linear_80kph
eig8 = eig(MatrixA);
Linear_90kph
eig9 = eig(MatrixA);
Linear_100kph
eig10 = eig(MatrixA);
Linear_110kph
eig11 = eig(MatrixA);
Linear_120kph
eig12 = eig(MatrixA);

eig_all=[eig3;eig4;eig5;eig6;eig7;eig8;eig9;eig10;eig11];

eig_0_filtered = eigen_range_filter_ts(eig2);
eig_h_filtered = eigen_range_filter_ts(eig12);
eig_filtered = eigen_range_filter_ts(eig_all);

figure
hold on;
plot(real(eig_0_filtered), imag(eig_0_filtered),'rs');
plot(real(eig_h_filtered), imag(eig_h_filtered),'gd');
plot(real(eig_filtered), imag(eig_filtered),'o');
title('TruckSim model: Speed 20-120 kph')
grid
hold off;



