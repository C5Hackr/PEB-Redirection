# PEB-Redirection (PEBD)

PEB-Redirection (PEBD) is a proof of concept for doing full dll hooking or swapping via the PEB. In theory you can completely hook or swap out a dll in memory with your own, for example you can swap ntdll.dll with kernel32.dll as done in the example code to effectivly switch the context of ntdll.dll (GetProcAddress, GetModuleHandle, IAT, EAT) with kernel32.dll's context, this may be useful for auth patching, game cheats, etc.

## Quick Note
Starring the repo helps a lot!

## Screenshots

![image](https://raw.githubusercontent.com/C5Hackr/PEB-Redirection/main/Images/POC.png)

## Disclaimer
This project was made for educational purposes only. I am not responsible if you choose to use this illegally/maliciously.
