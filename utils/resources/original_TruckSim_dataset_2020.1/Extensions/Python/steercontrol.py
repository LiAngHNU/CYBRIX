
import math
import vs

#import ctypes

# This is a routine which takes input from a running vehicle and
# creates a new path to avoid the detected obstruction.
#vs_outvals = []
def steer (signal, intab):

    # No Inputs
    num = len(intab)
    if (num != 0):
        return -1.0


    ROAD_ID = vs.var("ROAD_PATH_ID").value()
    LForward = vs.var("L_FORWARD").value()
    XCG_TM = vs.getval("XCG_TM")
    YCG_TM = vs.var("YCG_TM").value()

    Yaw = vs.var("YAW").value()

    Xpreview = XCG_TM + LForward*math.cos(Yaw)
    Ypreview = YCG_TM + LForward*math.sin(Yaw)

    RoadL = vs.road_l_id(Xpreview, Ypreview, ROAD_ID, 1)

    LatTrack = vs.getval("LAT_TRACK")
    GainStr = vs.getval("GAIN_STEER_CTRL")

    if (signal == "OPEN"):
        steer = (LatTrack - RoadL)*GainStr


    vs.outvals = []

    vs.outvals.append(steer)
    vs.outvals.append(Xpreview)
    vs.outvals.append(Ypreview)
    return 0.0

def steer_param (signal, intab):

    # No Inputs
    num = len(intab)
    if (num != 0):
        return -1.0


    ROAD_ID = vs.var("ROAD_PATH_ID").value()
    LForwardVar = vs.var("L_FORWARD")

    LForward = LForwardVar.value()

    #Example on how to examine validity of value
    if (LForwardVar.ident() == -1):
        vs.print(LForwardVar.name())
        vs.print("Value not found")
        return -1.0

    XCG_TM = vs.getval("XCG_TM")
    YCG_TM = vs.var("YCG_TM").value()

    Yaw = vs.var("YAW").value()

    Xpreview = XCG_TM + LForward*math.cos(Yaw)
    Ypreview = YCG_TM + LForward*math.sin(Yaw)

    RoadL = vs.road_l_id(Xpreview, Ypreview, ROAD_ID, 1)
    RoadPres = vs.road_l_id(XCG_TM, YCG_TM, ROAD_ID, 1)

    LatTrack = vs.getval("LAT_TRACK")
    GainStr = vs.getval("GAIN_STEER_CTRL")

    if (signal == "OPEN"):
        steer = (LatTrack - RoadL)*GainStr

    LatError = (LatTrack - RoadPres)

    vs.var("LERROR").setvalue(LatError)

    LatError = abs(LatError)

    toterr = vs.getval("TOTERR")

    vs.var("TOTERR").setvalue(toterr+LatError)



    vs.outvals = []

    vs.outvals.append(steer)
    vs.outvals.append(Xpreview)
    vs.outvals.append(Ypreview)
    return 0.0

def update_tab (signal, intab):

    # No Inputs
    num = len(intab)
    if (num != 0):
        return -1.0

    Iters = vs.var("ITERS").value()
    Iters = int(Iters)
    Toterr = vs.var("TOTERR").value()

    vs.tab("LFWD_ES").entry(0, Iters, 1).setvalue(Toterr)

    vs.outvals = []

    return 0.0


def bestval_lfwd (signal, intab):

    # No Inputs
    num = len(intab)
    if (num != 0):
        return -1.0

    Niters = vs.var("NITERS").value()
    Printvals = vs.var("PRINTVALS").value()

    bestval = vs.tab("LFWD_OPTS").entry(0, 1, 1).value()
    besterr = vs.tab("LFWD_ES").entry(0, 1, 1).value()

    Niters = int(Niters)

    if (Printvals == 1):
        vs.print("L_FORWARD parameter examined.")
    for ival in range(1,Niters+1):
        newerr = vs.tab("LFWD_ES").entry(0, ival, 1).value()
        entrystr = "Index: "+str(ival)+" Entry: "+str(vs.tab("LFWD_OPTS").entry(0, ival, 1).value())+" Value: "+str(round(vs.tab("LFWD_ES").entry(0, ival, 1).value(),4))
        if (Printvals == 1):
            vs.print(entrystr)
        if (newerr < besterr):
            besterr = newerr
            bestval = vs.tab("LFWD_OPTS").entry(0, ival, 1).value()

    if (Printvals == 1):
        vs.print("Best parameter value is:")
        vs.print(str(bestval))
        vs.print("Error Sum for this value is:")
        vs.print(str(round(besterr,4)))

    vs.var("BESTVAL").setvalue(bestval)
    vs.var("BESTVALERROR").setvalue(besterr)


    vs.outvals = []

    return 0.0

