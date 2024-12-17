# TCP Concurrent Server 👩‍👩‍👦‍👦

[![Developer](https://img.shields.io/badge/Developer-Basilabt-crimson)](https://github.com/Basilabt)  
![Version](https://img.shields.io/badge/version-1.0-brightgreen)  
![Language](https://img.shields.io/badge/language-C-blue)


## 🔍 Preview
![Network_Github](https://github.com/user-attachments/assets/207f0cab-c5bf-47c4-b701-d256df9284b3)


## ⚙️ Requirements
To use this tool, you need the following:
- A Linux/Unix-based operating system.
- **GCC** (GNU Compiler Collection) or another compatible C compiler to compile the code.
- Basic knowledge of socket programming in C.

## 📝 Description
This **Multi-Client TCP Server** allows clients to connect to a server and perform a series of string manipulation operations. It supports concurrent connections, client-specific operation requests, and operation logging. The server handles the following operations:
- **Count Words** in a string.
- **Count Vowels** (a, e, i, o, u) in a string.
- **Count Special Characters** (non-alphanumeric characters).
- **Find the Longest Word** in a string.
- **Find repeated Words** in a string.
- **Log Operations** with timestamps and client information.

### Key Features:
- **Multi-Client Support:** Handles multiple concurrent client connections with a limit on the number of concurrent clients.
- **String Operations:** Supports various string manipulation operations like word counting, vowel counting, special character counting, and more.
- **Child Process Management:** Uses `fork()` to spawn child processes for handling clients and avoids zombie processes with `waitpid()`.
- **Operation Logging:** Tracks each operation performed by a client along with the status and client information.

## 🛠 Installation & Usage

- Installation
```
git clone https://github.com/Basilabt/TCP_Concurrent_Server
```

 - Compile & Run 
```
gcc server.c -o server 
./server [Ip Address] [Port Number] [In.txt]
```


## 📞 Contact
If you have any questions, suggestions, or feedback

- **Email:** [baabutaleb@gmail.com](mailto:baabutaleb@gmail.com)
