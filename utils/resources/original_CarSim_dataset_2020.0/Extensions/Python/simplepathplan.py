
import math
import vs

def collisions (signal, intab):

    num_objects = int(vs.getval("NUM_OBS"))
    #Look through all objects, to see if they are close to current
    #position of vehicle.
    vs.debug(0)


    #Get position of vehicle
    lat_veh = vs.getval("Lat_Veh")
    stat_veh = vs.getval("Station")
    veh_x = vs.path_x_id(stat_veh,lat_veh,1,1)
    veh_y = vs.path_y_id(stat_veh,lat_veh,1,1)
    #vs.print(str(num_objects))
    #vs.print(str(veh_x))
    #vs.print(str(veh_y))

    for obj in range(1, (num_objects+1)):
       sobj = str(int(obj))
       object_ht = vs.getval("H_OBJ("+sobj+")")

       # Avoid 0 height objects
       if (object_ht > 0.0):
          obj_lat = vs.getval("LatO_"+sobj)
          obj_stat = vs.getval("S_Obj_"+sobj)
          obj_x = vs.path_x_id(obj_stat,obj_lat,1,1)
          obj_y = vs.path_y_id(obj_stat,obj_lat,1,1)
          dist = (veh_x-obj_x)*(veh_x-obj_x)+(veh_y-obj_y)*(veh_y-obj_y)
          dist = math.sqrt(dist)
          if (dist < 1.5):
              collision_time = vs.getval("COLLISION_TIME")
              if (collision_time <= 0.0):
                  vs.var("COLLISION_TIME").setvalue(vs.getval("T"))
              #vs.print(str(dist))
              break

    return 0.0

def returntopath (signal, intab):
    vals = []
    entry = [0.0]*2

    # No Inputs:


    lat_veh = vs.getval("Lat_Veh")
    station  = vs.getval("Station")
    goal_lat_targ = 0.0


    #define 5 points which can be used to define a Flat Spline
    offval = lat_veh/2.0
    if (offval < 0.0):
        offval = -offval
    seglength = offval*1.5
    entry[0] = (station)
    entry[1] = (lat_veh)
    #vs.print("RTP0:" + str(entry[0]) + "Lat:" + str(entry[1]))
    vals.append(entry.copy())

    entry[0] = (station+seglength)
    entry[1] = (((2*lat_veh)+goal_lat_targ)/3.0)
    #vs.print("RTP1:" + str(entry[0]) + "Lat:" + str(entry[1]))
    vals.append(entry.copy())

    entry[0] = (station+seglength*2)
    entry[1] = ((lat_veh+goal_lat_targ)/2.0)
    #vs.print("RTP2:" + str(entry[0]) + "Lat:" + str(entry[1]))
    vals.append(entry.copy())

    entry[0] = (station+seglength*3)
    entry[1] = (((2*goal_lat_targ)+lat_veh)/3.0)
    #vs.print("RTP3:" + str(entry[0]) + "Lat:" + str(entry[1]))
    vals.append(entry.copy())

    entry[0] = (station+seglength*4)
    entry[1] = goal_lat_targ
    #vs.print("RTP4:" + str(entry[0]) + "Lat:" + str(entry[1]))
    vals.append(entry.copy())

    #signal value is used to name path file
    vs.create_table(vals, signal, 'SPLINE_FLAT', 'm', 'm')

    #No output for this routine
    return 0.0


# This is a routine which takes input from a running vehicle and
# creates a new path to avoid the detected obstruction.
def pathtoavoidspp (signal, intab):
    vals = []
    entry = [0.0]*2
    OffsetL = 0
    OffsetR = 0
    DistL = -1.0
    DistR = -1.0
    dis = [0]*5
    bear = [0]*5


    #vs.debug(1)

    #Inputs: Velocity, Dist., Bearing, Lat_Targ, Station
    num = len(intab)
    if (num < 6):
        return -1.0

    Velocity = intab[0]
    Distance = intab[1]
    Bearing =  intab[2]
    LatTarg =  intab[3]
    Station =  intab[4]
    GoalLatTarg = intab[5]

    dis[0] = Distance
    bear[0] = Bearing


    yaw  = vs.getval("Yaw")
    dis[1] = vs.getval("DisS1_2")
    bear[1] = vs.getval("BrCS1_2")

    strng = "D1,"+str('%.3f'%Distance)+" "+str('%.3f'%(57.2958*Bearing))+"\n"

    strng = strng+" D2,"+str('%.3f'%dis[1])+" "+str('%.3f'%(57.2958*bear[1]))+"\n"

    dis[2] = vs.getval("DisS1_3")
    bear[2] = vs.getval("BrCS1_3")


    strng = strng+" D3,"+str('%.3f'%dis[2]) + " " + str('%.3f'%(57.2958*bear[2]))+"\n"

    dis[3] = vs.getval("DisS1_4")
    bear[3] = vs.getval("BrCS1_4")

    strng = strng + " D4," + str('%.3f'%dis[3]) + " " + str('%.3f'%(57.2958*bear[3]))+"\n"

    dis[4] = vs.getval("DisS1_5")
    bear[4] = vs.getval("BrCS1_5")

    strng = strng + " D5," + str('%.3f'%dis[4]) + " " + str('%.3f'%(57.2958*bear[4]))+"\n"

    #Debug statement
    #vs.print(strng)

    #Update offsets with arrays of bearings and distances
    for i in range(len(dis)):
        if(dis[i] != -1.0):
            offval = math.fabs(dis[i]*math.sin(bear[i]))
            #vs.print("Oval"+str(i)+" "+str('%.2f'%offval))
            #vs.print("DisL: " + str('%.2f'%DistL) + " DisR: " + str('%.2f'%DistR))
            if (offval > 4.5):
                offval = 4.5
            disval = dis[i]*math.cos(bear[i])
            #vs.print("dv:" + str('%.2f'%disval))
            if (bear[i] <= 0.0):
                if (DistR == -1.0):
                    OffsetR = offval
                    DistR = disval
                else:
                    if (disval < DistR):
                        DistR = disval
                    if (offval < OffsetR):
                        if (math.fabs(DistR-disval) < 5.0):
                            OffsetR = offval
            else:
                if (DistL == -1.0):
                    OffsetL = offval
                    DistL = disval
                else:
                    if (disval < DistL):
                        DistL = disval
                    if (offval < OffsetL):
                        if (math.fabs(DistL-disval) < 5.0):
                            OffsetL = offval
            #vs.print("Oval" + str(i) + " " + str('%.2f' % offval))
            #vs.print("DisL: " + str('%.2f' % DistL) + " DisR: " + str('%.2f' % DistR))

    #Now use offsets to plot a path
    if ((DistL == -1.0) and (DistR == -1.0)):
        pathshift1 = 0.0
        Dist1 = 5.0
    elif ((DistL == -1.0) or (DistR == -1.0)):
        if (DistL != -1.0):
            pathshift1 = -(4.5-OffsetL)
            Dist1 = DistL
        else:
            pathshift1 = (4.5-OffsetR)
            Dist1 = DistR
    else:
        if (abs(DistL-DistR) < 5.0):
            if (OffsetR > OffsetL):
                pathshift1 = -(4.5 - (4.5-(OffsetR-OffsetL)/2))
                Dist1 = DistR
            else:
                pathshift1 = (4.5 - (4.5-(OffsetL-OffsetR)/2))
                Dist1 = DistL

        else:
            if (DistL < DistR):
                pathshift1 = -(4.5 - OffsetL)
                Dist1 = DistL
            else:
                pathshift1 = (4.5 - OffsetR)
                Dist1 = DistR



    #vs.print(strng)

    #Normalize to nominal 120 km/hr = 33.33 m/s
    velnorm = 1.0 # For now, do not make use of velocity

    # How far off-center is the obstruction?


    #define 5 points which can be used to define a Flat Spline
    entry[0] = (Station)
    entry[1] = (LatTarg)
    #vs.print("0:"+str('%.3f'%entry[0])+"Lat:"+str('%.3f'%entry[1]))
    vals.append(entry.copy())

    entry[0] = (Station+(velnorm*Dist1/2))
    entry[1] = (LatTarg+pathshift1/math.sqrt(2))
    #vs.print("1:"+str('%.3f'%entry[0]) + "Lat:" + str('%.3f'%entry[1]))
    vals.append(entry.copy())

    entry[0] = (Station+Dist1)
    entry[1] = ((LatTarg+GoalLatTarg)/2.0+pathshift1)
    #vs.print("2:"+str('%.3f'%entry[0]) + "Lat:" + str('%.3f'%entry[1]))
    vals.append(entry.copy())

    #if (dis2 > 0.0):
    #    if (Dist2 < Dist1):
    #        entry[0] = (Station+ Dist1 + 2.0)
    #    else:
    #        entry[0] = (Station + Dist2)
    #    entry[1] = ((LatTarg + GoalLatTarg) / 2.0 + pathshift2)
    #    #vs.print("3a" + str(entry[0]) + "Lat:" + str(entry[1]))
    #    vals.append(entry.copy())

    #    if (dis3 > 0.0):
    #        entry[0] = (Station + Dist3)
    #        entry[1] = ((LatTarg + GoalLatTarg) / 2.0 + pathshift3)
    #        vals.append(entry.copy())

    entry[0] = (Station + (3 * velnorm * Dist1) / 2)
    entry[1] = ((LatTarg+GoalLatTarg)/2.0+pathshift1)
    #vs.print("3:" + str('%.3f'%entry[0]) + "Lat:" + str('%.3f'%entry[1]))
    vals.append(entry.copy())

    entry[0] = (Station + (2 * velnorm * Dist1))
    entry[1] = ((LatTarg+GoalLatTarg)/2.0+pathshift1)
    #vs.print("4:" + str('%.3f'%entry[0]) + "Lat:" + str('%.3f'%entry[1]))
    vals.append(entry.copy())

    #signal value is used to name path file
    vs.create_table(vals, signal, 'SPLINE_FLAT', 'm', 'm')

    #set the value of when to detect again
    vs.var("det_station").setvalue(Station+(1.0+(Dist1)))

    #No output for this routine
    return 0.0

# This is a routine which takes input from a running vehicle and
# creates a new path to avoid the detected obstruction.
def Qpathtomerge (signal, intab):
    vals = []
    entry = [0.0]*2

    #Inputs: Velocity, Bearing, LatTarg, Station
    Velocity = intab[0]  #Velocity of vehicle
    Bearing  = intab[1]  #Bearing of vehicle
    LatTarg  = intab[2]  #Lat offset of vehicle from path
    Station  = intab[3]  #Station of vehicle

    StatBearing = vs.path_yaw_id(Station, 0, 0, 0)

    #If Bearings are same and vehicle is offset with LTarg,
    #make a path to reduce Lat Offset to 0.

    #If Bearings form obtuse angle, create hyperbola path, eccentricity=2
    #1. Find interesecting Point
    StationOffset = math.abs(LatTarg)*math.tan(Bearing) #Assume Bearing points back toward road
    StationIntersect = Station+StationOffset



    #If Bearings form acute angle, do something else

    #If Bearings are diverging,


