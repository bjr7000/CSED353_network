Assignment 3 Writeup
=============

My name: Bae Jaeryun 

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 6 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
	The function fill_window checks SYN signal first. If the signal never
sent, send it first. If SYN is sent and ack from recently sent packet is 
well-arrived, make packets until the receiver's window is full or the sender's
stream is empty. After putting all packet to segment_out by send_seg function,
if the receiver's window is not full although the sender's stream is empty and 
EOF is set, the last packet will get FIN flag. Once a FIN flagged packet is
sent, the function fill_window never do something.
	The function send_seg takes a TCPsegment argument, and it just attaches
the segment its seqno, increases bytes_in_flight, next_seqno and decreases
the left receiver window size variable. After that, it puts the segment variable
with its seqno to out buffer and outstanding buffer, and it turns on and resets
timer if it is not turned on.
	The function ack_received ignores ack if its ackno precedes next_seqno
because it means the ack is from the future. If receiver's window size is not 0,
the fuction sets it to window size, but if it is 0, the function sets window 
size to 1 and makes zero window size flag true to distinguish with window size 1
. Until the outstanding buffer is empty, the function does things I wrote in
Implementation Challenges. Also, if the ack is well-arrived-ack, the function
resets timer and consec. sending variable, and decreases bytes_in_flight. If
the outstanding queue is empty, the timer is turned off.
	The function send_empty makes empty TCPSegment and put it to send_seg.
	The function for timer checks timer and its limit, and if the timer hits
its limit, resend packet from outstanding queue and sets timer 0. If ack is not
sent not because of the receiver's window size is 0, the function doulbes the 
timer limit and increases consec. sending variable.	

Implementation Challenges:
	It was hard to implement deciding when to send or resend segments. To 
implement it, a bool variable "ack_arrived" takes a role for deciding it. At 
first, if well-arrived ack whose ackno is right after sender's seqno, the 
variable is set to true and pops outstanding queue. If not, or the ack is not 
arrived yet, the variable is set to false. 
	After making the variable, the tcp sender program looks fine, but if
ackno of the arrived ack is not right after sender's seqno, but its ack+window
size is over the sender's seqno, sender can send a new packet which has more
information than packet sent right before (but never pop the outstanding queue.)
	Therefore, I made a new stage for the situation. If the situation
happens, ack_arrived is set to true to allow sending a new packet, but it is
not allowed to delete outstanding buffer. By doing this, some drawbacks could
happens, but they could be ignored. First of all, a collision between a packet 
which sent before and a packet which is sending now with more information does
not happen because in any order they arrive, reassembler cuts collisionable
data. Second, whether the ack is distorted to the received form or not, the
receiver will send a new normal ack with ackno made by newly sent packet. The
ack will plush all outstanding buffer because the ackno will precede to last
seqno of the buffers. Lastly, if timer hits limit so the sender sends packet
from outstanding buffer, it is okay because the receiver's reassembler ignores
the data and ack by the packet has ackno which makes the sender plush the
outstanding buffer. So, I added the situation, and the tcp sender works well.

Remaining Bugs:
None

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
