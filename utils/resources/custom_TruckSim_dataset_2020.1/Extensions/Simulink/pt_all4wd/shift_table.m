% Transmission speed vs. throttle position for downshift. Column 1 =
% throttle position (-). Column 2 = transmission speed when downshift occurs
% (rpm). Table ID = IGD specifies target for each table  (IGD 2 means downshift
% from 3 to 2).
%IGD        1 , Table ID number
DOWNSHIFT_TRANS_TABLE_1 = [
0, 70
0.2, 70
0.8, 110
1, 110
]; % ENDTABLE

% IGD        2 , Table ID number
DOWNSHIFT_TRANS_TABLE_2 = [
0, 213
0.2, 213
0.8, 450
1, 450
]; % ENDTABLE

% IGD        3 , Table ID number
DOWNSHIFT_TRANS_TABLE_3 = [
0, 320
0.2, 320
0.8, 710
1, 710
]; % ENDTABLE

% IGD        4 , Table ID number
DOWNSHIFT_TRANS_TABLE_4 = [
0, 480
0.75, 480
0.9, 1067
1, 1067
]; % ENDTABLE

% IGD        5 , Table ID number
DOWNSHIFT_TRANS_TABLE_5 = [
0, 620
0.75, 620
0.9, 1250
1, 1250
]; % ENDTABLE

% IGD        6 , Table ID number
DOWNSHIFT_TRANS_TABLE_6 = [
0, 960
0.75, 960
0.9, 1500
1, 1500
]; % ENDTABLE


% Transmission speed vs. throttle position for upshift Column 1 =
% throttle position (-). Column 2 = transmission speed when upshift occurs
% (rpm). Table ID = IGU specifies target for each table  (IGU 2 means upshift
% from 2 to 3).
% IGU        1 , Table ID number
UPSHIFT_TRANS_TABLE_1 = [
0, 158
0.2, 158
0.8, 380
1, 380
]; % ENDTABLE

% IGU        2 , Table ID number
UPSHIFT_TRANS_TABLE_2 = [
0, 237
0.2, 237
0.8, 600
1, 600
]; % ENDTABLE

% IGU        3 , Table ID number
UPSHIFT_TRANS_TABLE_3 = [
0, 356
0.2, 356
0.8, 888
1, 888
]; % ENDTABLE

% IGU        4 , Table ID number
UPSHIFT_TRANS_TABLE_4 = [
0, 633
0.1, 633
0.8, 1333
1, 1333
]; % ENDTABLE

% IGU        5 , Table ID number
UPSHIFT_TRANS_TABLE_5 = [
0, 800
0.1, 800
0.8, 2000
1, 2000
]; % ENDTABLE

% IGU        6 , Table ID number
UPSHIFT_TRANS_TABLE_6 = [
0, 1067
0.2, 1067
0.8, 2750
1, 2750
]; % ENDTABLE
