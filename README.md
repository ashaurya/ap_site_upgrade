__Neighbor-Lock Upgrade Algorithm__

Resets one AP, locks nearby AP, resets next one and so on. Sleeps for 200 seconds after one cycle

__How to compile__

_All_ 
make all

_Debug_
To enable debug (gdb): make debug

_Compress_
make tar

_Clean_
make clean


__Execution__

./version\_server : _END to kill the server, any other command to increase version number_

./server : _KILLS to kill the server, RESET to trigger the site upgrade (resets)_

./client 100 : _Start 100 Clients_

./table\_data : _Query Table Data_
