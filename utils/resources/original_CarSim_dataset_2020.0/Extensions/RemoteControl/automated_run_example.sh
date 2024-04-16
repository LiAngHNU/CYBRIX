#! /bin/sh

# Quick start guide uuid
filename="Run_6cb07365-85a3-4a3b-95f9-c5c844687601"

# copy the original simfile.sim just in case.
cp simfile.sim simfile_bck.sim

carsim-cli -uuid ${filename} -rundoc

# following lines are added to deal with some conflict with small/big letter names.
olddocname=$(find Results/${filename}  -name '*_doc.par')
newdocname="./Results/"${filename}"/Run_doc.par"

mv ${olddocname} ${newdocname}
#

# the original speed is 120. Change to another speed 
for speed in 115 110 105 100

do

# copy the LastRun_echo.par to a unique name 
filepre="speed_"${speed}
string0=${filepre}".par"  #string0 is echo filename
 
#cp ./Results/$filename/Run_all.par ./Results/$filename/${timestamp}_all.par

# filepath
filepath="./Results/"${filename}

# copy speed*_.par file to filepath
cp ${string0}  ${filepath}/.

# editing simfile.sim
replacement_string1="INPUT Results/"${filename}"/"${string0}
replacement_string2="ECHO Results/"${filename}"/"${filepre}"_echo.par"
replacement_string3="FINAL Results/"${filename}"/"${filepre}"_end.par"
replacement_string4="LOGFILE Results/"${filename}"/"${filepre}"_log.txt"
replacement_string5="ERDFILE Results/"${filename}"/"${filepre}".erd"
sed -i "/INPUT/c $replacement_string1" simfile.sim
sed -i "/ECHO/c $replacement_string2" simfile.sim
sed -i "/FINAL/c $replacement_string3" simfile.sim
sed -i "/LOGFILE/c $replacement_string4" simfile.sim
sed -i "/ERDFILE/c $replacement_string5" simfile.sim

# the following two lines to remove unwanted ^M 
sed -e 's/\r//' simfile.sim > simfile_temp.sim
mv simfile_temp.sim simfile.sim


## run carsim with a new speed
carsim-cli

#in case you want to use timestamp format.
#timestamp=$(date +%Y-%m-%d_%H.%M.%S)
##echo "$timestamp"

#cp ./Results/$filename/Run_all.par ./Results/$filename/${timestamp}_all.par
#cp ./Results/$filename/Run_echo.par ./Results/$filename/${timestamp}_echo.par
#cp ./Results/$filename/Run_end.par ./Results/$filename/${timestamp}_end.par
#cp ./Results/$filename/Run_log.txt ./Results/$filename/${timestamp}_log.txt
#cp ./Results/$filename/Run.erd ./Results/$filename/${timestamp}.csv

done

cp simfile_bck.sim simfile.sim

#
