SYSTEM LAB : ASSIGNMENT 4

QUESTION 4 : SYSTEM CALL IMPLEMENTATION

	In this assignment you need to add a new system call which prints a greeting message (“Hello World”) in the kernel’s log.Also add one sample C program to evaluate this newly built system call. 		Once the procedure is completed, generate a patch file. Submit this patch file along with your test C program as your deliverable for this question.

FILES AND FOLDERS :

	question4 folder contains the following : 

	files - contains the sample program("my_system_call.c") to run after following the program flow using running program 			sections
	obj   - contains the output file for the sample program(".my_system_call")	
		Run the make command to generate the output file in obj folder
	
	"old_new.diff" : patch file to be kept appropriately as mentioned in program flow.


PROGRAM FLOW : 

	1. You need to have ubuntu machine. (16.04 or higher)

	2. Download the kernel(linux-4.18.1)using following command (or manually)
	   	wget https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.18.1.tar.xz

	3. Extract the kernel using following command
		sudo tar -xvf linux-4.18.4.tar.xz -C /usr/src/

	4. Copy the patch from question4 folder to /usr/src/linux-4.18.1/

	5. Apply the patch using following command
		patch -p1 < hello_patch.diff

	6. Compile the kernel. Some packages are required to build using following commands
		sudo apt-get install gcc
		sudo apt-get install libncurses5-dev
		sudo apt-get install bison
		sudo apt-get install flex
		sudo apt-get install libssl-dev
		sudo apt-get install libelf-dev
 
	7. Create the .config file by menuconfig option.
		make menuconfig
 
 	8. Save the file by .config only(e.g. ".config").
	9. Build the kernel by issuing make command.
		make

	10. Insert the new kernel to current one using:
		make modules_install install

	11. Restart the system.Check the kernel version using 
		uname -r

RUNNING THE PROGRAM : 
	
	1. Compile and Run the sample program "my_system_call.c" from the question4 folder. 
		 OR 
	1. Include the system call header file and call the system call with number (548) where our command resides.
	   Compile and Run the program.	
	2. Type dmesg on terminal to see "Hello World" Message .
		
