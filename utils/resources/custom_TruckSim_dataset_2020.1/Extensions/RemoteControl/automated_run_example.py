import os
import glob
import sys
from shutil import move

filename = "Run_f02bf0b3-4066-447e-bb5e-40a929b281c7"

rundoc_string = "trucksim-cli -uuid " + filename + " -rundoc"
os.system(rundoc_string)

filesearch = "Results/" + filename + "/*_doc.par"
olddocfile = glob.glob(filesearch)
newdocfile = "Results/" + filename + "/Run_doc.par"
os.rename(olddocfile[0], newdocfile)

# four different speed
speeds = [75, 80, 85, 90]

def replaceLine(simfile, searchWord, replaceLn):
  # create a temp file.
  fh = open("temp.txt", 'w')
  with open(simfile) as old_file:
    for line in old_file:
      if searchWord in line:
        line = replaceLn
      fh.write(line)
  
  fh.close()
  os.remove(simfile)
  move("temp.txt", simfile)
  
for speed in speeds:
  os.system("cp simfile.sim simfile_back.sim")
  filepre = "speed_" + str(speed)
  string0 = filepre + ".par" 

  # file path
  filepath = "./Results/" + filename

  #  copy speed*.par file to filepath/
  os.system("cp " + string0 + " " + filepath + "/") 

  # editing simfile.sim
  replacement_string1 = "INPUT Results/" + filename + "/" + string0 + "\n"
  replacement_string2 = "ECHO Results/" + filename + "/" + filepre + "_echo.par\n"
  replacement_string3 = "FINAL Results/" + filename + "/" + filepre + "_end.par\n" 
  replacement_string4 = "LOGFILE Results/" + filename + "/" + filepre + "_log.txt\n"
  replacement_string5 = "ERDFILE Results/" + filename + "/" + filepre + ".erd\n"

  replaceLine("simfile.sim", "INPUT", replacement_string1)
  replaceLine("simfile.sim", "ECHO", replacement_string2)
  replaceLine("simfile.sim", "FINAL", replacement_string3)
  replaceLine("simfile.sim", "LOGFILE", replacement_string4)
  replaceLine("simfile.sim", "ERDFILE", replacement_string5)

  newfile = "simfile_" + str(speed) + ".sim"
  os.system("trucksim-cli")
  os.system("cp simfile.sim " + newfile) 
  # bring back the original simfile after each speed.
  os.system("cp simfile_back.sim simfile.sim")

  

