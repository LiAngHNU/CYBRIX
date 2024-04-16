import ctypes
import sys,string,os,re


class VehicleSimulationWithLiveAnimation:
    def __init__(self):
        self.dll_handle = None

    def get_api(self, dll_handle):
        self.dll_handle = dll_handle

        if dll_handle.Initialization is not None and \
                        dll_handle.UpdateOneStep is not None and \
                        dll_handle.Terminate is not None:
            dll_handle.GetLastLiveUpdateTime.restype = ctypes.c_double
            #dll_handle.vs_road_l.restype = ctypes.c_double
            #vsimports = (ctypes.c_double*1)()
            #vsexports = (ctypes.c_double*1)()
            #dll_handle.vs_integrate_io.argtypes = [ctypes.c_double, ctypes.byref(vsimports), ctypes.byref(vsexports)]
            ##dll_handle.vs.integrate_io.argtypes = [ctypes.c_double, ctypes.c_int]
            return True
        else:
            return False

    def get_char_pointer(self, python_string):
        # python version is greater or equal to 3.0 then we need to define the encoding when converting a string to
        # bytes. Once that is done we can convert the the python string to a char*.
        if sys.version_info >= (3, 0):
            char_pointer = ctypes.c_char_p(bytes(python_string, 'UTF-8'))
        else:
            char_pointer = ctypes.c_char_p(bytes(python_string))
        return char_pointer

    def get_parameter_value(self, line):
        index = line.find(' ')
        if index >= 0:
            return line[index:].strip()
        else:
            return None

    def get_dll_path(self, path_to_sim_file):
        dll_path = None
        prog_dir = None
        veh_code = None
        product_name = None
        product_ver = None
        library_name = None
        bitness_suffix = '_64' if ctypes.sizeof(ctypes.c_voidp) == 8 else '_32'
        platform = sys.platform

        sim_file = open(path_to_sim_file, 'r')
        for line in sim_file:
            if line.lstrip().startswith('PROGDIR'):
                prog_dir = self.get_parameter_value(line)
                if prog_dir == '.':
                  prog_dir = os.getcwd()
            elif line.lstrip().startswith('DLLFILE'):
                dll_path = self.get_parameter_value(line)
            elif line.lstrip().startswith('VEHICLE_CODE'):
                veh_code = self.get_parameter_value(line)
            elif line.lstrip().startswith('PRODUCT_ID'):
                product_name = self.get_parameter_value(line)
            elif line.lstrip().startswith('PRODUCT_VER'):
                product_ver = self.get_parameter_value(line)

        sim_file.close()

        if "tire" in veh_code:
            if platform == 'linux':
              library_name = 'libtire.so.%s'%(product_ver)
            else:
              library_name = "tire" + bitness_suffix
        elif product_name == "CarSim":
            if platform == 'linux':
              library_name = 'libcarsim.so.%s'%(product_ver)
            else:
              library_name = "carsim" + bitness_suffix
        elif product_name == "TruckSim":
            if platform == 'linux':
              library_name = 'libtrucksim.so.%s'%(product_ver)
            else:
              library_name = "trucksim" + bitness_suffix
        else:
            if platform == 'linux':
              library_name = 'libcarsim.so.%s'%(product_ver)
            else:
              library_name = veh_code + bitness_suffix

        if dll_path is None:
            if sys.platform == 'linux':
              dll_path = os.path.join(prog_dir, library_name);
            else:
              dll_path = os.path.join(prog_dir, "Programs", "Solvers", library_name + ".dll")
        return dll_path

    def ReadConfiguration(self, path_to_sim_file):
        path_to_sim_file_ptr = self.get_char_pointer(path_to_sim_file)
        platform = sys.platform
        if path_to_sim_file_ptr is not None:
            if platform == 'linux':
              ref_n_import = ctypes.c_long()
              ref_n_export = ctypes.c_longlong()
            else:
              ref_n_import = ctypes.c_int32()
              ref_n_export = ctypes.c_int64()
            ref_t_start = ctypes.c_double()
            ref_t_stop = ctypes.c_double()
            ref_t_step = ctypes.c_double()
            self.dll_handle.Initialization(path_to_sim_file_ptr,
                                                  ctypes.byref(ref_n_import),
                                                  ctypes.byref(ref_n_export),
                                                  ctypes.byref(ref_t_start),
                                                  ctypes.byref(ref_t_stop),
                                                  ctypes.byref(ref_t_step))
            configuration = {'n_import': ref_n_import.value,
                             'n_export': ref_n_export.value,
                             't_start': ref_t_start.value,
                             't_stop': ref_t_stop.value,
                             't_step': ref_t_step.value}
            return configuration

    def updateonestep(self, t_current, import_array, export_array):
        t_current_c_double = ctypes.c_double(t_current)
        import_c_double_array = (ctypes.c_double * len(import_array))(*import_array)
        export_c_double_array = (ctypes.c_double * len(export_array))(*export_array)

        c_integer_return = self.dll_handle.UpdateOneStep(t_current_c_double,
                                                           ctypes.byref(import_c_double_array),
                                                           ctypes.byref(export_c_double_array))

        export_array = [export_c_double_array[i] for i in range(len(export_array))]
        return c_integer_return, export_array

    def lastliveupdatetime(self):
        lastliveupdate = self.dll_handle.GetLastLiveUpdateTime()
        return lastliveupdate

    def terminate(self, t):
        t_c_double = ctypes.c_double(t)
        self.dll_handle.Terminate(t_c_double)

