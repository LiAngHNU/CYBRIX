import argparse
from ctypes import cdll
import os
import struct
import platform
import numpy as np
import cv2
import math
import sys

import GetSharedBufferInfo
import Simulation_with_LiveAnimation

if __name__ == '__main__':

    if sys.version_info[0] < 3:
        error_occurred = "Python version must be 3.0 or greater."
    else:
        systemSize = (8 * struct.calcsize("P"))  # 32 or 64

        parser = argparse.ArgumentParser(
            description='Python 3.5 script that runs the simulation in simfile.sim in the current directory.')

        parser.add_argument('--simfile', '-s', dest='path_to_sim_file',
                        default=os.path.join(os.getcwd(), "simfile.sim"),
                        help="Path to simfile. For example D:\\Product_dev\\Image\\CarSim\\Core\\CarSim_Data\\simfile.sim")

        parser.add_argument('--runs', '-r', type=int, dest='number_of_runs',
                        default=1,
                        help="Number of runs to make per single load of DLL. This parameter exists to replicate how real-time system use the solver")

        args = parser.parse_args()
        path_to_sim_file = args.path_to_sim_file
        number_of_runs = args.number_of_runs
        if number_of_runs < 1:
            number_of_runs = 1

        vs = Simulation_with_LiveAnimation.VehicleSimulationWithLiveAnimation()
        path_to_vs_dll = 'vs_lv_ds_x64.dll'
        error_occurred = 1
        current_os = platform.system()
        if path_to_vs_dll is not None and os.path.exists(path_to_vs_dll):
            if current_os == "Linux":
              mc_type = platform.machine()
              if mc_type == 'x86_64':
                dllSize = 64
              else:
                dllSize = 32
            else:
              if "_x64" in path_to_vs_dll:  # change this from _64 to _x64 since the naming has x64
                dllSize = 64
              else:
                dllSize = 32
            
            if systemSize != dllSize:
                print("Python size (32/64) must match size of .dlls being loaded.")
                print("Python size:", systemSize, "DLL size:", dllSize)
            else:  # systems match, we can continue

                status = 0
                vs_dll = cdll.LoadLibrary(path_to_vs_dll)
                if vs_dll is not None:
                    if vs.get_api(vs_dll):

                        for i in range(0, number_of_runs):
                            print(os.linesep + "++++++++++++++++ Starting run number: " + str(i + 1) + " ++++++++++++++++" + os.linesep)
                            configuration = vs.ReadConfiguration(path_to_sim_file.replace('\\\\', '\\'))
                            t_current = configuration.get('t_start')
                            swa = 5.0
                            import_array = [swa]
                            export_array = []
                            fov = 55.0  # change this if you are changing this parameter in CarSim/TruckSim.

                            # Run the integration loop
                            while status is 0:
                                j = 0
                                while status is 0:

                                    t_current = t_current + configuration.get('t_step')  # increment the time

                                    # Call the VS API integration function

                                    status, export_array = vs.updateonestep(t_current, import_array, export_array)

                                    if j == 100: # only process every 100

                                        polyShape = []
                                        depthShape = []

                                        for y in range(GetSharedBufferInfo.height - 1, -1, -1):
                                            for x in range(0, GetSharedBufferInfo.width, 1):
                                                pixelOffset = (y * GetSharedBufferInfo.width) + x
                                                startOfPixel = pixelOffset * 3
                                                line = [GetSharedBufferInfo.pixeldata[startOfPixel + 2],
                                                        GetSharedBufferInfo.pixeldata[startOfPixel + 1],
                                                        GetSharedBufferInfo.pixeldata[startOfPixel + 0]]
                                                depth_in_pixel = GetSharedBufferInfo.depthdata[y * GetSharedBufferInfo.width + x]
                                                depthShape.append(depth_in_pixel)
                                                polyShape.append(line)

                                        polyShape2 = np.asarray(polyShape)
                                        depthShape2 = np.asarray(depthShape)

                                        depth_np = depthShape2.reshape(GetSharedBufferInfo.height, GetSharedBufferInfo.width, 1)

                                        image_np = polyShape2.reshape(GetSharedBufferInfo.height,
                                                                      GetSharedBufferInfo.width, 3).astype(np.uint8)

                                        image_gray = cv2.cvtColor(image_np, cv2.COLOR_RGB2GRAY)

                                        # not sure if GaussianBlur is necessary.
                                        image_blur = cv2.GaussianBlur(image_gray, (5,5),0)

                                        edges = cv2.Canny(image_blur, 70, 140)

                                        two_third_from_top = int((GetSharedBufferInfo.height*2)/3)

                                        row_region = int(GetSharedBufferInfo.height*3/5)
                                        half_rows = int(GetSharedBufferInfo.height/2)
                                        half_columns = int(GetSharedBufferInfo.width/2)

                                        for y in range(0, GetSharedBufferInfo.height-1):
                                            for x in range(0, GetSharedBufferInfo.width-1):
                                                if y < row_region:
                                                    edges[y][x] = 0
                                                else:
                                                    if x <= half_columns:
                                                        if y < -(half_rows/half_columns)*x + GetSharedBufferInfo.height:
                                                            edges[y][x] = 0
                                                        else:
                                                            edges[y][x] = edges[y][x]
                                                    else:
                                                        if y < (half_rows/half_columns)*x:
                                                            edges[y][x] = 0
                                                        else:
                                                            edges[y][x] = edges[y][x]

                                        lines = []
                                        lines = cv2.HoughLinesP(edges, 1, np.pi / 180, 15, 30)

                                        right_array = []
                                        left_array = []

                                        latR = 0
                                        latL = 0

                                        angle_to_vehicle = 90.0
                                        half_of_fov = fov / 2.
                                        drift_warning = ""

                                        for i in range(0, len(lines)):
                                            for x1, y1, x2, y2 in lines[i]:
                                                if (x1 > GetSharedBufferInfo.width/2) and (x2 > GetSharedBufferInfo.width/2):
                                                    if(x1 != x2) and (y1 != y2):
                                                        if abs(x1-x2)> 5 and abs(y1-y2) > 5:
                                                            temp_array = [x1, y1, x2, y2]
                                                            right_array.append(temp_array)
                                                if (x1 < GetSharedBufferInfo.width/2) and (x1 < GetSharedBufferInfo.width/2):
                                                    if (x1 != x2) and (y1 != y2):
                                                        if abs(x1 - x2) > 5 and abs(y1 - y2) > 5:
                                                            temp_array = [x1, y1, x2, y2]
                                                            left_array.append(temp_array)

                                        if len(right_array) != 0:
                                            right_array_avg = np.mean(right_array,axis=0)
                                            right_array_avg = right_array_avg.astype(int)

                                            right_upper_pts_x = int(right_array_avg[2]-((right_array_avg[2]-right_array_avg[0])*\
                                                              (right_array_avg[3]-two_third_from_top))/\
                                                              (right_array_avg[3]-right_array_avg[1]))

                                            right_lower_pts_x = int(right_array_avg[2]-((right_array_avg[2]-right_array_avg[0])*\
                                                              (right_array_avg[3]-GetSharedBufferInfo.height))/\
                                                              (right_array_avg[3]-right_array_avg[1]))

                                            right_line_depth = depth_np[two_third_from_top][right_upper_pts_x]
                                            lat_on_pixel_R = half_columns - right_upper_pts_x
                                            ang_to_right_lane = (lat_on_pixel_R * half_of_fov / half_columns)
                                            latR = right_line_depth * math.tan(ang_to_right_lane / 180 * math.pi)

                                            cv2.line(image_np, (right_upper_pts_x, two_third_from_top),
                                                     (right_lower_pts_x, GetSharedBufferInfo.height), (0, 255, 0), 5)

                                        if len(left_array) != 0:
                                            left_array_avg = np.mean(left_array,axis=0)
                                            left_array_avg = left_array_avg.astype(int)

                                            left_upper_pts_x = int(((left_array_avg[2] - left_array_avg[0]) * \
                                                                   (two_third_from_top - left_array_avg[1])) / \
                                                                  (left_array_avg[3] - left_array_avg[1]) +
                                                                  left_array_avg[0])
                                            left_lower_pts_x = int(((left_array_avg[2] - left_array_avg[0]) * \
                                                                   (GetSharedBufferInfo.height - left_array_avg[1])) / \
                                                                  (left_array_avg[3] - left_array_avg[1]) +
                                                                  left_array_avg[0])

                                            left_line_depth = depth_np[two_third_from_top][left_upper_pts_x]
                                            lat_on_pixel_L = half_columns - left_upper_pts_x
                                            ang_to_left_lane = (lat_on_pixel_L * half_of_fov / half_columns)
                                            latL = left_line_depth * math.tan(ang_to_left_lane / 180 * math.pi)

                                            cv2.line(image_np, (left_upper_pts_x, two_third_from_top),
                                                     (left_lower_pts_x, GetSharedBufferInfo.height), (0, 255, 0), 5)

                                        if latR != 0 and latL != 0:
                                            if abs(abs(latL)-abs(latR)) > 0.7:
                                                if abs(latL) > abs(latR):
                                                    swa = 25.0
                                                    drift_warning = "Drift to the Right"
                                                else:
                                                    swa = -25.0
                                                    drift_warning = "Drift to the Left"
                                        if abs(latR) < 1.0 and latR != 0:
                                            swa = 25.0
                                            drift_warning = "Drift to the Right"

                                        if abs(latL) < 1.0 and latL != 0:
                                            swa = -25.0
                                            drift_warning = "Drift to the Left"

                                        text_latR = 'Right Lateral Distance to the Line: {0:.2f} m'.format(float(latR))
                                        text_latL = 'Left Lateral Distance to the Line: {0:.2f} m'.format(float(latL))
                                        cv2.putText(image_np, text_latL, (5,10),
                                                    cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 0), 1)
                                        cv2.putText(image_np, text_latR, (5,25),
                                                    cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 0), 1)
                                        cv2.putText(image_np, drift_warning, (half_columns,
                                                                              int(GetSharedBufferInfo.height*2/3)),
                                                    cv2.FONT_HERSHEY_SIMPLEX, 0.4,(0,0,255),1)

                                        cv2.imshow('lane detection', cv2.resize(image_np, (
                                            int(GetSharedBufferInfo.width * 1.5), int(GetSharedBufferInfo.height * 1.5))))

                                        if cv2.waitKey(25) & 0xFF == ord('q'):
                                            cv2.destroyAllWindows()
                                            break
                                        j = 0

                                        GetSharedBufferInfo.api_ver2_contents.UnLock(GetSharedBufferInfo.sbHandle)
                                        import_array = [swa]
                                        swa = 5.0
                                    j = j + 1

                                            # Terminate solver
                            vs.terminate(t_current)

                            # error_occurred = vs.run(path_to_sim_file.replace('\\\\', '\\'))
                            if error_occurred is not 0:
                                break
                            print(os.linesep + "++++++++++++++++ Ending run number: " + str(i + 1) + " ++++++++++++++++" + os.linesep)

    sys.exit(error_occurred)
