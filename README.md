# Custom Mail Client and Server

This application is Based on SMTP(RFC 5321) Protocol and POP3(RFC 821) protocols

## Installation

Install gcc and all the libraries to support multithreading and POSIX in general

## Usage
To run the program, follow the steps below 😊:
```bash
git clone https://github.com/mr-195/Custom-Mail-Client-and-Server
cd Custom-Mail-Client-and-Server
```
Now follow the steps below:
compile the mailclient.c with 
```bash 
gcc mailclient.c -o client
```
compile the smtpmail.c with 
```bash 
gcc smtpmail.c -o server
```
compile the popserver.c with 
```bash 
gcc popserver.c -o popserver
```

This will produce all the executables.

For Running follow the steps in order 
Open separate terminals and type the commands below
```bash
./popserver 120
```
```bash
./server  8000
```
```bash
./client 127.0.0.1 8000 120
```
## Illustrations
Look at the client Terminal 
1) First it will ask for Username and Password enter them using user.txt provided to you .
2) Then it will show three Options like this
   Enter option:
1. Manage Mail
2. Send Mail
3. Quit

3) For Sending Mails press 2
4) Then enter the mail message in the appropriate format
   - From: <SendersMail>
   - To: <username@blahblah>
   - Subject: <Enter Subject>
   - Body
   - . (this dot is necessary)
5) After this press enter You will SMTP protocols messages both in the client and server side
6) If you press option 1 for manage mail
7) You will see the list of message list in the mail box in the format
   MessageID <sendersMAil> RecieveTime <Subject>
8) It will also ask you to provide the message number to read
9) Enter the correct message id from the list
10) Then it will shows you that entire message
11) After that it will ask you Enter a character to delete the message: 
12) Enter d to delete it or press q to exit
    Thats it !!

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
