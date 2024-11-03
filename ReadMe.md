# WOL (Wake On Lan)

SHA Hash of signed Binary: 

I've created this program because at times, a computer has been turned off or has gone to sleep 
and has needed to be stated while no one was available to physically activate the computer. Windows
didn't have a built in WOL signal sender that I could find. So I made one. The signal that gets
sent for WOL is simple.  It is the byte value `0xFF` repeated 6 times followed by the MAC Address
repeated 16 times. This results in a packet that is 102 bytes. 

While it doesn't matter what port the message is sent on generally, I'm sending the message on 
port 9 because many routers are preconfigured to allow UDP packets on this port and won't deny
it. The message is sent via broadcast since a computer that is turned off might not have an IP 
address (yet).

More information on this application can be found on https://j2i.net/apps/wol
