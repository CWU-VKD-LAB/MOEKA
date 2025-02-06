import os
import numpy as np
import pandas as pd
import pickle
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, HistGradientBoostingClassifier, ExtraTreesClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler

def makeModel(input_file, ordinalValues, output_csv=None):
    # Get the directory where this Python file resides.
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Derive the base name from the input file.
    # For example, if input_file is "heart_failure_clinical_records_dataset.csv",
    # base_name will be "heart_failure_clinical_records_dataset".
    base_name = os.path.splitext(os.path.basename(input_file))[0]
    
    # Determine the output CSV file name if not provided.
    # This CSV will store the normalized (ordinal mapped) dataset.
    if output_csv is None:
        output_csv = base_name + '_ordinal_normalization.csv'
    
    # Build the full path for the output CSV using the script directory.
    output_csv_full = os.path.join(script_dir, output_csv)
    
    # input file goes here (previously hard coded)
    # Now, the input file is provided as a parameter.
    df = pd.read_csv(input_file)
    """cols = [0, 4, 7, 8, 11, 12] # attributes which are used in heart failure paper, last col is class
    df = df[df.columns[cols]]"""
    
    # Define classifier names.
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
    
    # below combination of monotonicity constraints is optimal for random forest classifier on original dataset
    # TODO: invert the "-1" attributes to enforce increasing monotonicity
    # mon = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    mon = [1, -1, 1, 1, -1, -1, -1, 1, -1, -1, 1, -1]
    
    # mon constraints for columns 4, 7, 8, 9
    # mon = [-1, -1, 1, 1]
    
    DIM = len(mon)
    
    # Define the classifiers to be used.
    classifiers = [
        DecisionTreeClassifier(max_depth=5, random_state=42),
        DecisionTreeClassifier(max_depth=5, random_state=42, monotonic_cst=mon),
        RandomForestClassifier(max_depth=5, n_estimators=8, max_features=1, random_state=42),
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
            dmax = np.max(ds[:, [i]])
            dmin = np.min(ds[:, [i]])
            
            # For each element in the column, adjust the value.
            # This transformation inverts the attribute so that increasing monotonicity can be enforced.
            for j in range(len(ds[:, i])):
                ds[:, i][j] = dmax - ds[:, i][j] + dmin
    
    # scale ds to ordinal values
    for i in range(DIM):
        col_min = np.min(ds[:, [i]])
        col_max = np.max(ds[:, [i]])
        
        # formula taken from https://scikit-learn.org/1.5/modules/generated/sklearn.preprocessing.MinMaxScaler.html#sklearn.preprocessing.MinMaxScaler
        col_std = (ds[:, [i]] - col_min) / (col_max - col_min)
        col_scaled = col_std * (ordinalValues[i] - 0) + 0
        ds[:, [i]] = col_scaled
        
    # save dataset as new csv
    # Create an "ordinal mapped" version of the dataset.
    ordinal_mapped_ds = np.copy(ds)
    
    # is there a flaw here? maybe shouldn't test expert model on this but on computational model
    # For every data point, adjust non-boundary values by rounding up.
    for d in ordinal_mapped_ds:
        for i in range(DIM):
            if d[i] != 0 and d[i] != (ordinalValues[i] - 1):
                d[i] = np.ceil(d[i]) - 1
    
    # Save the ordinal normalized dataset to a CSV file with a dynamic name.
    df_ordinal = pd.DataFrame(ordinal_mapped_ds)
    df_ordinal.to_csv(output_csv_full, index=False)
    print("Normalized dataset saved to", output_csv_full)
    
    # Save the ordinal values to a separate file for use by the C++ file.
    ordinal_values_file = os.path.join(script_dir, base_name + '_ordinal_values.txt')
    with open(ordinal_values_file, 'w') as f:
        f.write(','.join(str(val) for val in ordinalValues))
    print("Ordinal values saved to", ordinal_values_file)
    
    # lets see what results there are when the ds is ordinal
    # ds = ordinal_mapped_ds
    
    # split data: assume the last column is the target.
    X = np.delete(ds, [-1], axis=1)  # everything but last column (features)
    y = np.delete(ds, np.s_[:-1], axis=1)  # last column (target)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )
    
    # warning unless ravel - doesn't change results
    y = y.ravel()
    y_train = y_train.ravel()
    y_test = y_test.ravel()
    
    # iterate over classifiers and save them
    for name, clf in zip(names, classifiers):
        # clf = make_pipeline(StandardScaler(), clf)
        clf.fit(X_train, y_train)
        score = clf.score(X_test, y_test)
        y_pred = clf.predict(X_test)
        clf_report = classification_report(y_test, y_pred, zero_division=1)
        scores = cross_val_score(clf, X, y, cv=10)
        print(name)
        print(clf_report)
        print(str(score))
        print("cross val scores: ", scores)
        print("mean cross val score: ", scores.mean())
        
        # Build a dynamic model filename based on the input file's base name and classifier name.
        model_filename = f"{base_name}_{name}.sav"
        # Build the full path using the script directory.
        model_filepath = os.path.join(script_dir, model_filename)
        pickle.dump(clf, open(model_filepath, 'wb'))
        print("Saved model to", model_filepath)

def makePrediction(model_filename, args):
    # Given a model filename and an input data point, load the model and make a prediction.
    # The data point (args) should be in the proper format (e.g., tuple or list).
    args = (args,)
    loaded_model = pickle.load(open(model_filename, 'rb'))
    
    prediction = int(loaded_model.predict(args)[0])
    print("prediction: " + str(prediction))
    
    return prediction

#   - The dataset file path,
#   - The name of the classifier to run,
if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Train a model using a specified classifier and dataset.")
    parser.add_argument("dataset", help="Path to the input dataset CSV file")
    parser.add_argument("classifier", help="Name of the classifier to run (e.g., 'DecisionTree', 'RandomForest', etc.)")
    parser.add_argument("--output", help="Name of the output normalized CSV file", default=None)
    args = parser.parse_args()
    
    # Hardcoded ordinal values (these can be made dynamic later if needed)
    ordinalValues = (3, 2, 4, 2, 2, 2, 3, 2, 2, 2, 2, 3)
    
    # Call makeModel with the provided command line arguments.
    # Only the specified classifier (args.classifier) will be run.
    makeModel(args.dataset, ordinalValues, output_csv=args.output)
