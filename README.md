# server
this pet-project is synchronised c++11 server and a small website for this server.
## features
1. logs
2. exception handling
3. syncronised connection
4. get & head method support
5. fast and efficient c++ server
## installation
1. download the zip file
1e. <img width="1892" height="813" alt="image" src="https://github.com/user-attachments/assets/3f159cc0-a2cf-4363-8054-abcd92994ff3" />
2. extract its contents
2e. <img width="1159" height="577" alt="image" src="https://github.com/user-attachments/assets/2daf8c46-7419-4d60-9810-fe823d529339" />
3. open the folder and then start the server.sln
3e. <img width="787" height="595" alt="image" src="https://github.com/user-attachments/assets/881a7d90-4f4e-4a72-aee9-a4b6c84a49ee" />
4. build the code
4e. <img width="1451" height="1023" alt="image" src="https://github.com/user-attachments/assets/60f0b54e-3ce0-4341-9763-796d884c97c7" />
and you are done.
## usage
### server side
1. open your *desired* windows command line application
2. set path to the server executable `cd path\server`
server gets ip, port, root dir, through command line arguments\
so you have to specifie them when starting the executable.
4. type: `server.exe <ip> <port> <rootdir>`
3e. example `server.exe 127.0.0.1 8080 path\to_rootdir\`\
for a test you can use these args\
`server.exe 127.0.0.1 8080 papka\`
### client side
1. open any browser
2. in search bar type `http://<server_ip>:<server_port>`
