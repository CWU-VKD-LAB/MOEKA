# MOEKA (Monotone Boolean Function Expert Knowledge Acquisition)

MOEKA is a specialized user-interview based software system designed for expert knowledge acquisition and representation of monotone Boolean functions. It provides both a command-line interface and a graphical user interface for creating, visualizing, and analyzing monotone Boolean functions.

## Software Purpose

MOEKA solves the problem of efficiently capturing expert knowledge about monotone Boolean functions through an interactive process. It uses Hansel chains and various optimization algorithms to minimize the number of questions needed to reconstruct a complete monotone Boolean function from expert knowledge.

## Key Features

- **Interactive Knowledge Acquisition**: Guides experts through a systematic process of defining monotone Boolean functions
- **Multiple Interface Options**:
  - Command-line interface for direct interaction
  - Graphical user interface for visual representation and manipulation
- **Optimization Algorithms**:
  - Hansel chain-based questioning strategies
  - Binary search capabilities
  - Chain jumping optimization
  - Majority vector handling
- **Visualization Tools**:
  - Multiple Disk Form (MDF) visualization
  - Function hierarchy representation
  - Mathematical notation display
  - English language representation
- **Function Management**:
  - Create and edit functions manually
  - Compare multiple functions
  - Hierarchical function organization
  - Support for k-valued attributes

## Getting Started

### Prerequisites

- Windows operating system
- Visual Studio (for building from source)
- Required dependencies (see Dependencies folder)

### Building

1. Clone the repository
2. Open `DataVisualization.sln` in Visual Studio
3. Build the solution

### Running

The program can be run in two modes:

1. **Console Mode**: For direct command-line interaction
2. **GUI Mode**: For visual interface and manipulation

## Usage

### GUI Interface

The GUI provides several key components:

1. Model manipulation controls
2. Function hierarchy tree view
3. Help/Introduction window
4. Quick access buttons for common operations
5. Model scaling controls
6. Model selection interface

### Function Creation

1. Use the Prep Window to define function attributes and parameters
2. Create clauses and sub-functions in the Function Screen
3. Use the Interview Screen for guided function creation
4. Visualize and edit the resulting model

## Contributing

Please read the LICENSE file for details on our code of conduct and the process for submitting pull requests.

## License

MOEKA is licensed under the MIT License, allowing free use for both personal and commercial purposes. For full terms, see the `LICENSE` file.
