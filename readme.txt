STOWAGE - EXERCISE 2 

HOW TO RUN THE PROJECT:

	We have 3 folders: Algorithm, Common, and Simulation.
	in both Algorithm and Simulation, you need to run - make.
	After that, in our Simulation folder, an exexution file named 	"simulation" will be created.
	In order to run our simulation, use the following command:
		simulation -travel_path <travel path> -algorithm_path <algorithm path> -output <output path>
	Flags can be given in defferent order.
	In case no path is given for output directory, the output files will 	be created in the current 	directory.

	After running this command, execpt our output files, several informative messages will be printed to the screen.


OUTPUT FILES:
	At every run of our simulation, a result file is created as requested in this exercise, named simulation.results.
	If an error occurred durring our run, an error file is created, named simulation.errors.
	Our error file reports:
		# Errors reported by our simulation - for example bad input file
		# Algorithm warnnings: when an algorithm fails to report this errors
		# Algorithm mistakes: for exampple, makes an illegal move or forgets to unload container at a given port.

ERROR HANDLING: ?