import math
from operator import mul
import vs

export_pressure = [0, 0, 0, 0]
signal_input = [1, 1, 1, 1]

# This file is created based on the existing Simulink ABS model.
def simple_abs_controller_model(signal, intab):
    global signal_input

    mps_to_kph = 3.6

    # signal is "OPEN". This is a flag but we won't be using it in this code.
    # intabl is empty.  <== not using intab, use getval instead.

    vehicle_vel = vs.getval("Vx")*mps_to_kph #
    wheel_vel = [vehicle_vel,vehicle_vel,vehicle_vel,vehicle_vel]
    wheel_vel[0] = vs.getval("Vx_L1")*mps_to_kph # Vx_L1
    wheel_vel[1] = vs.getval("Vx_R1")*mps_to_kph # Vx_R1
    wheel_vel[2] = vs.getval("Vx_L2")*mps_to_kph # Vx_L2
    wheel_vel[3] = vs.getval("Vx_R2")*mps_to_kph # Vx_R2
    pbk_con = vs.getval("Pbk_Con") # Pbk_Con 

    wheel_vel_sub = [vehicle_vel+0.00001-x for x in wheel_vel] 

    slip_ratio = [x/(vehicle_vel+0.00001) for x in wheel_vel_sub] # front rear
    # added 0.00001 to vehicle_vel to prevent divide by zero.

    front_relay_on_point = 0.1
    front_relay_off_point = 0.05
    front_relay_output_when_on = 0
    front_relay_output_when_off = 1
    rear_relay_on_point = 0.1
    rear_relay_off_point = 0.05
    rear_relay_output_when_on = 0
    rear_relay_output_when_off = 1

    for i in range(0,2):
        if slip_ratio[i] >= front_relay_on_point:
            if signal_input[i] == 1:
                signal_input[i] = front_relay_output_when_on
        if slip_ratio[i] <= front_relay_off_point:
            if signal_input[i] == 0:
                signal_input[i] = front_relay_output_when_off
        if slip_ratio[i+2] >= rear_relay_on_point:
            if signal_input[i+2] == 1:
                signal_input[i+2] = rear_relay_output_when_on
        if slip_ratio[i+2] <= rear_relay_off_point:
            if signal_input[i+2] == 0:
                signal_input[i+2] = rear_relay_output_when_off

    speed_threshold = 3.0 
    if vehicle_vel < speed_threshold:
      signal_input = [1, 1, 1, 1]  

    control_mode = signal_input
    control_mode[2] = min(signal_input[2],signal_input[3])
    control_mode[3] = min(signal_input[2],signal_input[3])

    # pass control_mode to brake_actuator_model  
    pressure = [0,0,0,0]
    pressure = brake_actuator_model(control_mode, pbk_con)

    vs.var("pbk_L1").setvalue(pressure[0]*1000000.0)
    vs.var("pbk_R1").setvalue(pressure[1]*1000000.0)
    vs.var("pbk_L2").setvalue(pressure[2]*1000000.0)
    vs.var("pbk_R2").setvalue(pressure[3]*1000000.0)

    return 0.0

def brake_actuator_model(control_mode, pbk_con):
    global export_pressure

    mc_pressure = pbk_con/1000000.0

    gain_value = [1.0, 1.0, 0.4, 0.4]
    gain_results = [x*y for x, y in zip(control_mode, gain_value)] 

    input_to_transfer_func = [x*mc_pressure for x in gain_results] 

    for i in range(0,4):
        # retained the last time step information and then recaculate them
        #export_pressure[i] = export_pressure[i]*0.9833333 + input_to_transfer_func[i]*0.016666666
        export_pressure[i] = export_pressure[i]*0.9918 + input_to_transfer_func[i]*0.00825

    return export_pressure


