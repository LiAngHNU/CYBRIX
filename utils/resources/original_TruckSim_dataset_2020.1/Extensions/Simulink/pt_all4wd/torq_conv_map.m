% Inverse of torque converter capacity vs. speed ratio of torque
% converter output shaft to engine. Column 1 = speed ratio (-). Column 2 = 1/K
% (SQRT(F*L)/rpm), where K = torque converter capacity (K = rpm/SQRT(F*L)).
INV_CAP_TC_TABLE = [
0, 0.009797959
0.1, 0.009700949
0.2, 0.009605841
0.3, 0.009512525
0.4, 0.009420127
0.5, 0.009322277
0.6, 0.009187466
0.7, 0.008898309
0.8, 0.008098066
0.9, 0.005854699
0.95, 0.003590465
0.99, 0.000852466
1.01, -0.000852466
1.05, -0.003590465
1.1, -0.005854699
1.2, -0.008098066
1.3, -0.008898309
1.4, -0.009187466
1.5, -0.009322277
1.6, -0.009420127
1.7, -0.009512525
1.8, -0.009605841
1.9, -0.009797959
2, -0.009797959
]; % ENDTABLE


% Torque converter output to input torque ratio vs. speed ratio of
% torque converter output shaft to engine. Column 1 = speed ratio (-).  Column
% 2 = torque ratio (-).
RM_TC_TABLE = [
0, 2.05
0.87, 1.05
0.9, 1.01
1, 1
]; % ENDTABLE
