import numpy as np
import pandas as pd
import pickle
from sklearn import tree
from sklearn.ensemble import AdaBoostClassifier, RandomForestClassifier, HistGradientBoostingClassifier, ExtraTreesClassifier
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import cross_val_score
from sklearn.metrics import classification_report, confusion_matrix

def makeModel(ordinalValues):
    # input file goes here
    input_file = "./heart_failure_clinical_records_dataset.csv"

    df = pd.read_csv(input_file)
    """cols = [0, 4, 7, 8, 11, 12] # attributes which are used in heart failure paper, last col is class
    df = df[df.columns[cols]]"""
    
    names = [
        "DecisionTree",
        "DecisionTreeMonotone",
        "RandomForest",
        "RandomForestMonotone",
        "LinearDiscriminantAnalysis",
        #"ExtraTrees",
        #"ExtraTreesMonotone",
        #"NeuralNet",
        #"AdaBoost",
        #"HistGradBoostClassifier",
        #"HistGradBoostClassifierMonotone"
    ]

    # below combination of monotonicity constraints is optimal for random forest classifer on original dataset
    # TODO: invert the "-1" attributes to enforce increasing monotonicity
    #mon = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    mon = [1, -1, 1, 1, -1, -1, -1, 1, -1, -1, 1, -1]
    
    #mon constraints for columns 4, 7, 8, 9
    #mon = [-1, -1, 1, 1]
    
    DIM = len(mon)
    
    classifiers = [
        DecisionTreeClassifier(max_depth=5, random_state=42),
        DecisionTreeClassifier(max_depth=5, random_state=42, monotonic_cst=mon),
        RandomForestClassifier(max_depth=5, n_estimators=8, max_features=1, random_state=42,),
        RandomForestClassifier(max_depth=5, n_estimators=8, max_features=1, random_state=42, monotonic_cst=mon),
        LinearDiscriminantAnalysis(),
        """ExtraTreesClassifier(max_depth=5, n_estimators=10, max_features=1, random_state=42),
        ExtraTreesClassifier(max_depth=5, n_estimators=10, max_features=1, random_state=42, monotonic_cst=mon),
        MLPClassifier(alpha=1, max_iter=1000, random_state=42),
        AdaBoostClassifier(algorithm="SAMME", random_state=42),
        HistGradientBoostingClassifier(),
        HistGradientBoostingClassifier(monotonic_cst=mon) # shape of DIM for heart failure dataset"""
    ]

    # preprocess dataset, split into training and test part
    ds = df.to_numpy()
    
    # negative attributes
    for i in range(len(mon)):
        if mon[i] == -1:
            dmax = np.max(ds[:,[i]])
            dmin = np.min(ds[:,[i]])
            
            for j in range(len(ds[:,i])):
                ds[:,i][j] = dmax - ds[:,i][j] + dmin
    
    # scale ds to ordinal values
    for i in range(DIM):
        col_min = np.min(ds[:,[i]])
        col_max = np.max(ds[:,[i]])
        
        # formula taken from https://scikit-learn.org/1.5/modules/generated/sklearn.preprocessing.MinMaxScaler.html#sklearn.preprocessing.MinMaxScaler
        col_std = (ds[:,[i]] - col_min) / (col_max - col_min)
        col_scaled = col_std * (ordinalValues[i] - 0) + 0
        ds[:,[i]] = col_scaled
        
    # save dataset as new csv
    ordinal_mapped_ds = np.copy(ds)
    
    # is there a flaw here? maybe shouldnt test expert model on this but on computational model
    for d in ordinal_mapped_ds:
        for i in range(DIM):
            if d[i] != 0 and d[i] != (ordinalValues[i] - 1):
                d[i] = np.ceil(d[i]) - 1
    
    df = pd.DataFrame(ordinal_mapped_ds)
    df.to_csv('heart_failure_ordinal_normalization.csv', index=False)
    
    # lets see what results there are when the ds is ordinal
    #ds = ordinal_mapped_ds
    
    # split data
    X = np.delete(ds, [-1], 1) # everything but last row of dataset  (classes)
    y = np.delete(ds, np.s_[:-1], 1) # last row (classes)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )
    
    
    
    # warning unless ravel - doesnt change results
    y = np.ravel(y)
    y_train = np.ravel(y_train)
    y_test = np.ravel(y_test)

    # iterate over classifiers and save them
    for name, clf in zip(names, classifiers):
        #clf = make_pipeline(StandardScaler(), clf)
        clf.fit(X_train, y_train)
        score = clf.score(X_test, y_test)
        y_pred = clf.predict(X_test)
        clf_report = classification_report(y_pred, y_test, zero_division=1)
        scores = cross_val_score(clf, X, y, cv=10)
        print(name)
        print(clf_report)
        print(str(score))
        print("cross val scores: ", scores)
        print("mean cross val score: ", scores.mean())
        pickle.dump(clf, open(name + ".sav", 'wb'))
        
        if (name == 'LinearDiscriminantAnalysis'):
            print("coefficients: ", clf.coef_)
            

def makePrediction(name, args):
    args = (args,)
    loaded_model = pickle.load(open(name, 'rb'))
    
    prediction = int(loaded_model.predict(args)[0])
    print("prediction: " + str(prediction))
    
    return prediction

# last value below is kn+1 (not used in above code)
# heart failure ordinal values: 3, 2, 4, 2, 2, 2, 3, 2, 2, 2, 2, 3, 2 
makeModel((3, 2, 4, 2, 2, 2, 3, 2, 2, 2, 2, 3))
#makePrediction("RandomForest.sav", (82,1,379,0,50,0,47000,1.3,136,1,0,13))

# make model for cols 4, 7, 8, 9
#makeModel((2, 2, 2, 2))