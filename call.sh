
#Sets the absorber (passive) em calo material in mm
ABS_THICKNESS=10

#Sets the gap (active) em calo material in mm
GAP_THICKNESS=5

#Set the amount of electromagnetic calorimeter layers
EMLAYERS=10

./bin/Linux-g++/exampleB4c -m run_el.mac -emlayers $EMLAYERS -absorber $ABS_THICKNESS -gap $GAP_THICKNESS 
mv result.root output_el.root 
./bin/Linux-g++/exampleB4c -m run_pi.mac -emlayers $EMLAYERS -absorber $ABS_THICKNESS -gap $GAP_THICKNESS
mv result.root output_pi.root

root -l process.C
