# FMI version 2.0
# pyFMI example

from pyfmi import load_fmu
import sys
import os

def pyFmiSimulation(file):
    path, fl = os.path.split(os.path.realpath(__file__))
    filename = os.path.join(path, file)
    model = load_fmu(filename, log_level=7, log_file_name="FMI2_Baseline.log")
    opts = model.simulate_options()
    opts['ncp'] = 1000
    opts['result_file_name'] = "result.csv"

    name = model.get_name()
    print("Model Name: ", name)
    model.set_log_level(7)
    model.print_log()

    inputList = model.get_input_list()
    print("Input List: ")
    for inp in inputList:
        print(inp)

    outputList = model.get_output_list()
    print("Output List: ")
    for op in outputList:
        print(op)

    res = model.simulate(final_time=10, options=opts)

    simtime = res['time']
    print("Simulation times: ", simtime)

    model.terminate()

def main():
    if sys.version_info >= (3, 0):
        raise "pyFMI is not supported with Python 3.x, use Python 2.7.x"

    fmuFile = sys.argv[1]
    pyFmiSimulation(fmuFile)

if __name__ == "__main__":
    main()
