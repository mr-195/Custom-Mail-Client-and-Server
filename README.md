# networks_assign1
- 21CS10087 Utsav Dhanuka
- 21CS10088 Manaswi Raj

This assignment is Based on SMTP Protocol and POP3 protocols

Guide Lines For Compiling in 3 separate Terminals

1) compile the mailclient.c with gcc mailclient.c -o client
2) compile the smtpmail.c with gcc smtpmail.c -o server
3) compile the popserver.c with gcc popserver.c -o popserver

For Running follow the steps in order 
1) ./popserver 120
2) ./server  8000
3) ./client 127.0.0.1 8000 120

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
   - .
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
