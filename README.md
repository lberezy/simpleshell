#Simple Shell

This is a simple "shell" written to test process forking, signals and exec() in
C. This is not intended to be a fully functional shell.

##Features:

* Change Prompt Statement with `PS <string>`
	* `PS default` to reset to default
* Supports multiple arguments to commands
	* e.g. `man 1 grep`
	* Tokenises input based on spaces

##Not working

* Pipes
* Backgrounding processes
* IO redirection
* "quoted items with spaces" not treated as single tokens