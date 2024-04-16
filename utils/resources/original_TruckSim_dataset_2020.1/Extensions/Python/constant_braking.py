import math
import vs

def brake(signal, intab):
    brake_pressure = [0.0]*4

    sim_time = vs.getval("t")

    if sim_time > 5.0:
        brake_pressure[0] = 30*1000000.0 
        brake_pressure[1] = 30*1000000.0 
        brake_pressure[2] = 20*1000000.0 
        brake_pressure[3] = 20*1000000.0 

    vs.outvals = []
    vs.outvals.append(brake_pressure[0])
    vs.outvals.append(brake_pressure[1])
    vs.outvals.append(brake_pressure[2])
    vs.outvals.append(brake_pressure[3])


    return 0.0

