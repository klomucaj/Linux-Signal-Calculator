# LinuxSignals
## Summary
Client-Server for arithmetic calculations. The communication is through signals and files in Linux.

## Server
1. The server deletes the last file used for communication with a client - ```to_srv.txt``` (if it exists) to enable new connections.  
2. The server wait for a new signal from clients.
3. The server creates a child to handle the client from whom it got the signal.  
   The child then will - 
    - read the data from the file the client made for it.
    - delete the ```to_srv.txt``` file (to enable new connections).
    - create a file with the result of the client exercise with the name ```to_client_xxxx.txt``` (xxxx = client pid).
    - send a signal to the client that the result is ready.
    The server will return to wait for new clients while the child is running.

## Client
The client -
1. create the file for communication with the server - ```to_srv.txt```.  
    If the file exists, the client will try ten times to create a new one, and between each try, it will wait for one to five seconds (randomly).
    After ten tries, if the client has not succeeded, it will close with an appropriate message.
2. add the relevant arguments from the terminal to the file.  
    The arguments are -
    - ```arg[0]``` - the program name.
    - ```arg[1]``` - the server pid.
    - ```arg[2]``` - the first number.
    - ```arg[3]``` - the arithmetic operation (1-4) - 
      - 1 is +
      - 2 is -
      - 3 is *
      - 4 is /
    -  ```arg[4]``` - the second number.
    The client adds ```arg[2], arg[3], arg[4]``` to file, and also its pid.
3. send a signal to the server and modify it that the file is ready.
4. wait for a signal from the server.
5. after getting the signal from the server, the client will read the data from the file had made for the client - ```to_client_xxxx.txt``` (xxxx = client pid), will write it to the screen and delete the file.
    

## How to run?
There are a few steps -
1. Clone into the project using git bush or terminal with this commend -
    ```
    git clone https://github.com/klomucaj/Linux-Signal-Calculator.git
    ```
2. Open the terminal in the code directory.
3. Run those commands to create executable files from the C files - 
    ```
    gcc -o server.out server.c
    gcc -o client.out client.c
    ```
    You can use any name you want instead of ```server.out``` and ```client.out```.
4. Run the server in the background with this command - 
    ``` 
    ./server.out&
    ```
    This command will show the process id of the server.
5. Run the client -
    ```
    ./client.out p1 p2 p3 p4
    ```
    - p1 - the server's process id
    - p2 - the first number
    - p3 - the arithmetic operation (1-4) - 
      - 1 is +
      - 2 is -
      - 3 is *
      - 4 is /
    - p4 - the second number
