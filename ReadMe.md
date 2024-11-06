# WOL (Wake On Lan)


Check file hashes from PowerShell with `Get-FileHash -Path j2iwol.exe`

![Wake On Lan Logo][1]

For the signed executables:
* j2iwol.exe - `B4DDEE42C6C294AFB1E93CC120BB934F232DED74E309514CA44654006074F4AB`
* j2iwol.net.exe - `6D4AB076AD895F4D815289E4514D610E4A123D1C2CC3081066E62074C7319830`
For the unsigned executables:
* j2iwol.exe - `39CD1563B470FDC4CBF911B4CFDF4F54D9EE2F85919C6566C1F00FD226290D91`
* j2iwol.net.exe - `6B850FB5208F4B63BBE367CE9107E913CC5E237CA93146CD98F7AC040EB3A6E2`


I've created this program because at times, a computer has been turned off or has gone to sleep 
and has needed to be stated while no one was available to physically activate the computer. Windows
didn't have a built in WOL signal sender that I could find. So I made one. The signal that gets
sent for WOL is simple.  It is the byte value `0xFF` repeated 6 times followed by the MAC Address
repeated 16 times. This results in a packet that is 102 bytes. 


![WOL Packet Structure][2]


While it doesn't matter what port the message is sent on generally, I'm sending the message on 
port 9 because many routers are preconfigured to allow UDP packets on this port and won't deny
it. The message is sent via broadcast since a computer that is turned off might not have an IP 
address (yet).

More information on this application can be found on https://j2i.net/apps/wol


[1]:assets/wol.png
[2]:assets/packetStructure.png