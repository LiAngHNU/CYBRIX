#
# Install JModelica, download: http://www.jmodelica.org/
#
# To run this command from iPython: %run run_baseline2
#
from pyfmi import load_fmu


#set step size 0.001 (1 kHz)
model = load_fmu("Baseline_FMI2.fmu")
opts = model.simulate_options()
opts['ncp'] = 1000
res = model.simulate(final_time=10, options=opts)
