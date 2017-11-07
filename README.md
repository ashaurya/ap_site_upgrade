##Trivial Upgrade Algorithm##

Triggers upgrade of all APs at once

###How to compile###

___All___ 
make all

___Debug___
To enable debug (gdb): make debug

___Compress___
make tar

___Clean___
make clean


###Execution###

./version\_server : ___END to kill the server, any other command to increase version number___
./server : ___KILLS to kill the server, RESET to trigger the site upgrade (resets)___
./client 100 : ___Start 100 Clients___
./table\_data : ___Query Table Data___
