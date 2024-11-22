import makePredictions as mp

# use this file to call ML model predictions from MOEKA c++ program 
mp.makePredictionFromFile(mp.sys.argv[1], mp.sys.argv[2])