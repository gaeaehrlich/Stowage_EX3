STOWAGE - EXERCISE 2 

HOW TO RUN THE PROJECT:

	We have 3 folders: bonus, algorithm, common, and simulation.
	in bonus, algorithm and simulation, you need to run - make.
	After that, in our simulation folder, an execution file named 	"simulator" will be created.
	In order to run our simulation, use the following command:
		simulation -travel_path <travel path> -algorithm_path <algorithm path> -output <output path>
	Flags can be given in different order.
	In case no path or a bad path is given for output directory, the output files will 	be created in the current 	directory.

	After running this command, except from our output files, several informative messages will be printed to the screen.


OUTPUT FILES:
	At every run of our simulation, a result file is created as requested in this exercise, named simulation.results.
	If an error occurred during our run, an error file is created, named simulation.errors.

SPECIAL CASES:
    1. Duplicates - if a container with the same ID appears more than once on a certain port, we treat only it's first
       appearance as a valid one, that can be loaded or rejected, and the rest of the appearances must be rejected by
       the algorithm.
    2. Algorithm errors - if the algorithm makes an error in it's instructions, the run on the travel does not terminate,
       but more on that in the bonus file.
    3. Algorithm return value - for each port, the algorithm return value includes the errors that appeared in the ship
       plan and route files, along with the errors that appeared in the cargo data file of the current port.