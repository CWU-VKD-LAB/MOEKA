# -*- coding: utf-8 -*-

from ast import Num
import os
from re import S
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

def makeModel(input_file, selected_classifier, output_csv=None, monotonicity=None):

    # Get the directory where this script resides.
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Derive the base name from the input file.
    base_name = os.path.splitext(os.path.basename(input_file))[0]

    # Determine the output CSV file name if not provided.
    if output_csv is None:
        output_csv = base_name + '_ordinal_normalization.csv'
    output_csv_full = os.path.join(script_dir, output_csv)

    # Read the CSV file.
    df = pd.read_csv(input_file)

    # Assume that the target is the last column.
    # Compute the number of feature columns.
    num_features = df.shape[1] - 1

    # start ordinal values all as 10 and i guess we work from here.
    ordinalValues = [4] * num_features 

    # set our monotonicity as 1 for all attributes.
    monotonicity = [1] * num_features

    # Convert the DataFrame to a NumPy array.
    # (We assume that the first num_features columns are features and the last column is the target.)
    ds = df.to_numpy()

    # --- Normalize the features to their ordinal scales ---
    for i in range(num_features):
        col = ds[:, i]
        col_min = np.min(col)
        col_max = np.max(col)
        # Avoid division by zero in case the feature is constant.
        if col_max - col_min != 0:
            col_std = (col - col_min) / (col_max - col_min)
        else:
            col_std = col - col_min  # all zeros if constant
        # Scale to the range [0, ordinalValues[i]].
        col_scaled = col_std * int(ordinalValues[i]) # cast as an int because its a string
        ds[:, i] = col_scaled

    # Make a copy for the final (ordinal mapped) dataset.
    ordinal_mapped_ds = np.copy(ds)

    # For each feature column, adjust non-boundary values by rounding.
    for row in ordinal_mapped_ds:
        for i in range(num_features):
            if row[i] != 0 and row[i] != (ordinalValues[i] - 1):
                row[i] = np.ceil(row[i]) - 1

    # Create a DataFrame for the normalized data.
    # This uses the original DataFrame's columns, so headers are preserved.
    df_ordinal = pd.DataFrame(ordinal_mapped_ds, columns=df.columns)

    # Save the normalized dataset to a CSV file.
    df_ordinal.to_csv(output_csv_full, index=False)
    print("Normalized dataset saved to", output_csv_full)

    # --- Save the ordinal values to a file ---
    ordinal_values_file = os.path.join(script_dir, base_name + '_ordinal_values.txt')
    with open(ordinal_values_file, 'w') as f:
        f.write(','.join(str(val) for val in ordinalValues))
    print("Ordinal values saved to", ordinal_values_file)

    # --- Instantiate the classifier if necessary ---
    # If the selected_classifier parameter is a string, convert it to an instance.
    if isinstance(selected_classifier, str):
        classifier_dict = {       
            'DecisionTree' : DecisionTreeClassifier,
            'DecisionTreeMonotone' : DecisionTreeClassifier, # DOES THIS NEED TO BE CHANGED??????
            'RandomForest' : RandomForestClassifier,
            'RandomForestMonotone' : RandomForestClassifier,
            'LinearDiscriminantAnalysis' : LinearDiscriminantAnalysis,
            'ExtraTrees' : ExtraTreesClassifier,
            'MLP' : MLPClassifier,
            'AdaBoost' : AdaBoostClassifier,
            'HistGradBoostClassifier' : HistGradientBoostingClassifier,
            'HistGradBoostClassifierMonotone' : HistGradientBoostingClassifier
        }
        if selected_classifier in classifier_dict:
            selected_classifier = classifier_dict[selected_classifier]()
        else:
            raise ValueError("Unknown classifier: " + selected_classifier)

    # --- Split the data into features and target ---
    # Features: all columns except the last.
    # Target: the last column.
    X = np.delete(ds, -1, axis=1)
    y = ds[:, -1]
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )

    # --- Train and evaluate the classifier ---
    selected_classifier.fit(X_train, y_train)
    score = selected_classifier.score(X_test, y_test)
    y_pred = selected_classifier.predict(X_test)
    clf_report = classification_report(y_test, y_pred, zero_division=1)
    scores = cross_val_score(selected_classifier, X, y, cv=10)
    classifier_name = selected_classifier.__class__.__name__
    print("Classifier:", classifier_name)
    print(clf_report)
    print("Test score:", score)
    print("Cross val scores:", scores)
    print("Mean cross val score:", scores.mean())

    # Save the trained model to a file.
    model_filename = f"{base_name}_{classifier_name}.sav"
    model_filepath = os.path.join(script_dir, model_filename)
    with open(model_filepath, 'wb') as f:
        pickle.dump(selected_classifier, f)
    print("Saved model to", model_filepath)

    # --- Write functionAttributes.txt ---
    # This file contains:
    #   1. The total number of target classes.
    #   2. For each feature (all columns except the target),
    #      its name and its corresponding K value (from ordinalValues).
    func_attr_file = os.path.join(script_dir, "functionAttributes.txt")
    with open(func_attr_file, 'w') as f:
        # Determine the total number of target classes.
        target_col_name = df_ordinal.columns[-1]
        try:
            # If the target is numeric and 0-indexed, max value + 1 gives the number of classes.
            num_classes = int(df_ordinal[target_col_name].max()) + 1
        except Exception:
            num_classes = len(df_ordinal[target_col_name].unique())
        f.write("Total classification classes: " + str(num_classes) + "\n\n")
        f.write("Attributes and their K values (based on normalized data):\n")
        for i, col in enumerate(df_ordinal.columns[:-1]):
            k_value = ordinalValues[i]
            f.write(f"{col}: {k_value}\n")
    print("Function attributes saved to", func_attr_file)

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
   
    makeModel(args.dataset, args.classifier, output_csv=args.output)
