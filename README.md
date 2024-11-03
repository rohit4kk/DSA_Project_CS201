This program simulates traffic management by tracking vehicle positions on a road and monitoring for congestion based on vehicle density within specified distances. It utilizes a van Emde Boas (vEB) tree for efficient management of vehicle positions, allowing fast insertion, deletion, and range counting operations.

Input File Format
Each line in input.txt should contain two integers:
<x_coordinate> <speed>

x_coordinate: Initial position of the vehicle in meters.
speed: Vehicle speed in meters per second.

Final Output:
Displays remaining vehicles in the vEB tree after the specified runtime.
Logs vehicles that exceed the maximum distance to delete_output.txt.

Maximum Distance: Vehicles are removed if their position exceeds 30,000 meters.
Congestion Range: The program monitors congestion between 1,000 to 3,000 meters around the main vehicle.
Sleep Function: sleep(1) is used to simulate real-time vehicle movement updates every second.

Instructions for Compilation and Execution
Compile the Code
gcc -o veb_tree.c veb_tree
Run the Program
./veb_tree
