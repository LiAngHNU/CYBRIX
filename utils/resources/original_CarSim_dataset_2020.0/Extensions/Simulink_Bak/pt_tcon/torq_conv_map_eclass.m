% Inverse of torque converter capacity vs. speed ratio of torque
% converter output shaft to engine. Column 1 = speed ratio (-). Column 2 = 1/K
% (SQRT(F*L)/rpm), where K = torque converter capacity (K = rpm/SQRT(F*L)).
INV_CAP_TC_TABLE = [
            0,     0.009121
          0.1,     0.009121
          0.2,     0.009121
          0.3,     0.009121
          0.4,      0.00912
          0.5,     0.009112
          0.6,     0.009066
          0.7,     0.008864
          0.8,     0.008142
          0.9,     0.005941
         0.95,      0.00366
         0.99,     0.000872
         1.01,    -0.000872
         1.05,     -0.00366
          1.1,    -0.005941
          1.2,    -0.008142
          1.3,    -0.008864
          1.4,    -0.009066
          1.5,    -0.009112
          1.6,     -0.00912
          1.7,    -0.009121
          1.8,    -0.009121
          1.9,    -0.009121
            2,    -0.009121
]; % ENDTABLE


% Torque converter output to input torque ratio vs. speed ratio of
% torque converter output shaft to engine. Column 1 = speed ratio (-).  Column
% 2 = torque ratio (-).
RM_TC_TABLE = [
            0,        1.864
        0.102,         1.78
        0.202,        1.701
        0.306,        1.608
        0.353,         1.56
          0.4,        1.519
        0.452,        1.464
        0.502,        1.412
        0.551,        1.356
          0.6,        1.309
         0.65,        1.259
          0.7,        1.203
         0.75,        1.151
        0.801,        1.098
         0.82,        1.074
         0.84,        1.048
        0.849,        1.041
        0.861,        1.027
         0.87,        1.014
        0.881,        0.996
         0.89,        0.996
          0.9,        0.993
         0.91,        0.998
        0.921,        0.986
        0.928,        0.993
         0.94,        0.996
        0.951,        0.981
         0.99,            1
            1,            1
]; % ENDTABLE
