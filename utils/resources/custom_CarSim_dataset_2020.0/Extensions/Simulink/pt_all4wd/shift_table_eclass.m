% Transmission speed vs. throttle position for downshift. Column 1 =
% throttle position (-). Column 2 = transmission speed when downshift occurs
% (rpm). Table ID = IGD specifies target for each table  (IGD 2 means downshift
% from 3 to 2).
%IGD        1 , Table ID number
DOWNSHIFT_TRANS_TABLE_1 = [
0, 503
0.4, 503
0.8, 925
1, 925
]; % ENDTABLE

% IGD        2 , Table ID number
DOWNSHIFT_TRANS_TABLE_2 = [
0, 750
0.4, 750
0.8, 1416
1, 1416
]; % ENDTABLE

% IGD        3 , Table ID number
DOWNSHIFT_TRANS_TABLE_3 = [
0, 1051
0.4, 1051
0.8, 2109
1, 2109
]; % ENDTABLE

% IGD        4 , Table ID number
DOWNSHIFT_TRANS_TABLE_4 = [
0, 1440
0.4, 1440
0.8, 2956
1, 2956
]; % ENDTABLE

% IGD        5 , Table ID number
DOWNSHIFT_TRANS_TABLE_5 = [
0, 1756
0.4, 1756
0.8, 4050
1, 4050
]; % ENDTABLE

% IGD        6 , Table ID number
DOWNSHIFT_TRANS_TABLE_6 = [
0, 2057
0.4, 2057
0.8, 4939
1, 4939
]; % ENDTABLE


% Transmission speed vs. throttle position for upshift Column 1 =
% throttle position (-). Column 2 = transmission speed when upshift occurs
% (rpm). Table ID = IGU specifies target for each table  (IGU 2 means upshift
% from 2 to 3).
% IGU        1 , Table ID number
UPSHIFT_TRANS_TABLE_1 = [
0, 629
0.2, 629
0.8, 1233
1, 1233
]; % ENDTABLE

% IGU        2 , Table ID number
UPSHIFT_TRANS_TABLE_2 = [
0, 938
0.2, 938
0.8, 1888
1, 1888
]; % ENDTABLE

% IGU        3 , Table ID number
UPSHIFT_TRANS_TABLE_3 = [
0, 1314
0.2, 1314
0.8, 2813
1, 2813
]; % ENDTABLE

% IGU        4 , Table ID number
UPSHIFT_TRANS_TABLE_4 = [
0, 1800
0.2, 1800
0.8, 3942
1, 3942
]; % ENDTABLE

% IGU        5 , Table ID number
UPSHIFT_TRANS_TABLE_5 = [
0, 2195
0.2, 2195
0.8, 5400
1, 5400
]; % ENDTABLE

% IGU        6 , Table ID number
UPSHIFT_TRANS_TABLE_6 = [
0, 2571
0.2, 2571
0.8, 6585
1, 6585
]; % ENDTABLE
