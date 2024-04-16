
import math
import vs
# This is a routine which takes input from a running vehicle and
# creates a new path to avoid the detected obstruction.
def pathtoavoid (signal, intab):
    vals = []
    entry = [0.0]*2

    #Inputs: Velocity, Dist., Bearing, Lat_Targ, Station
    num = len(intab)
    if (num < 5):
        return -1.0

    Velocity = intab[0]
    Distance = intab[1]
    Bearing =  intab[2]
    LatTarg =  intab[3]
    Station =  intab[4]


    #Normalize to nominal 120 km/hr = 33.33 m/s
    velnorm = 1.0 # For now, do not make use of veolocity

    # How far off-center is the obstruction?
    offset = math.fabs(Distance*math.tan(Bearing))
    pathshift = (4.5 - offset)
    if (Bearing > 0.045):
        pathshift = -pathshift

    #define 5 points which can be used to define a Flat Spline
    entry[0] = (Station)
    entry[1] = (LatTarg)
    vals.append(entry.copy())

    entry[0] = (Station+(velnorm*Distance/2))
    entry[1] = (LatTarg+pathshift/math.sqrt(2))
    vals.append(entry.copy())

    entry[0] = (Station+Distance)
    entry[1] = (LatTarg+pathshift)
    vals.append(entry.copy())

    entry[0] = (Station+(3*velnorm*Distance)/2)
    entry[1] = (LatTarg+pathshift/math.sqrt(2))
    vals.append(entry.copy())

    entry[0] = (Station+(2*velnorm*Distance))
    entry[1] = LatTarg
    vals.append(entry.copy())

    #signal value is used to name path file
    vs.create_table(vals, signal, 'SPLINE_FLAT', 'm', 'm')

    #No output for this routine
    return 0.0