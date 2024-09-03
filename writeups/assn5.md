Assignment 5 Writeup
=============

My name: Bae Jaeryun

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 3.5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
	In the sending datagram function, it checks if it has sent datagram
to give ip address. If it has, push the given datagram to cache and send it. 
If it has not and if the given ip address has never got sent the ARP or timeout 
is set, it makes broadcast sending to find destination.
	In the receiving datagram function, it only takes if the given frame's
destination is its address or broadcast. If so, it checks the type of the frame.
When the type is IPv4, it is easy. It just read the payload of the frame.
When the type is ARP, it is hard. It unfolds the payload and find sender's
address. If the address cannot be found in cache, put it in the caches. After
that, send all waiting datagrams which were not sent because its destination
is not found. Lastly, it send reply of the given ARP message which contains
information of this side.
	In tick function, it set time variable and remove old caches whose
age is over 30000ms. 

Implementation Challenges:
	It was not hard to write code, but it was hard to study what to send
and finding class functions in sponge standards.

Remaining Bugs:
	Bugs in assn4 are not fixed yet...

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
