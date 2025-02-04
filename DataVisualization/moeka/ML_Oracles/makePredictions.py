import sys
import pickle
import pandas as pd
import pathlib

def makePredictionFromDatapoint(name, args):
    args = (args,)
    loaded_model = pickle.load(open(name, 'rb'))
    
    prediction = int(loaded_model.predict(args)[0])
    print(prediction)
    
    return prediction


def makePredictionFromFile(modelName, fileName):
    loaded_model = pickle.load(open(modelName, 'rb'))
    df = pd.read_csv(fileName, header=None)
    ds = df.to_numpy()
    
    outputFile = 'ML_Oracle_results.csv'
    
    with open(outputFile, "w") as f:
        for d in ds:
            prediction = int(loaded_model.predict(d[:-1].reshape(1, -1))[0])
            f.write(str(prediction) + "\n")
            
    print(outputFile)
    

def testViolationOfMonotonicity(modelName, fileName):
    loaded_model = pickle.load(open(modelName, 'rb'))
    df = pd.read_csv(fileName, header=None)
    ds = df.to_numpy()
    
    errors = 0
    viol = 0
    trueVal = 0
    falseVal = 0
    
    # perform for 0 index first for efficiency
    prediction = int(loaded_model.predict(ds[0][:-1].reshape(1, -1))[0])
        
    if (prediction != ds[0][-1]):
        errors += 1
    
    # 1 index to end
    for i in range(1, len(ds)):
        prediction = int(loaded_model.predict(ds[i][:-1].reshape(1, -1))[0])
        
        if (prediction != ds[i][-1]):
            errors += 1
        elif prediction == ds[i][-1]:
            trueVal += 1
            
        if (ds[i][-1] == 1) and (prediction == 0):
            viol += 1
    print ("acc: ", trueVal / len(ds))
    print ("total errors: ", errors)
    print("error as %: ", errors/ len(ds)) 
    print ("total viol: ", viol)
    print ("viol: ", viol / len(ds))
            

#args = [int(arg) for arg in sys.argv[2:]]
#makePredictionFromFile(sys.argv[1], sys.argv[2])
#makePredictionFromDatapoint("RandomForest.sav", (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1))
#makePredictionFromFile("RandomForest.sav", "D:\.School\Work\Github DataVisualization\DataVisualization\DataVisualization\hanselChains.csv")
