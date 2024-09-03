Assignment 4 Writeup
=============

My name: Bae Jaeryun

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 18 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [0.69, 0.73]

Program Structure and Design of the TCPConnection:
	If SYN signal is detected from received or sent segments, set connection
 to true, which makes program send segments and receive segments.
	In receiving function, it checks if RST is set and passes the received
segments to sender and receiver. If ackno is available or it has to send
keep-alive signal, call fill_window function of the sender. Also, linger or not
variable is checked.
	In tick function, it checks if RST flag is set so it has to shutdown and
it is okay to deactivate the connection. After that, fill_window function is
called and if sending segments is not empty or RST flag is on, send segments.
	In sending to the world function, it attaches information of ACK flag
and ackno, and check if FIN signal is sent or not. If it is not duplicated
segments without retransmission, push the segment to a outside sending queue and
 pop the sender's queue.

Implementation Challenges:
	It was too slow if sender checks FIN is now available or not, so I
removed the checking and let it send in retransmission, which leads to remaining
bugs. Also, it was still too slow, so I had to change reassembler to faster one
which uses substr function.
	tcp_connection checks availableness of passive close and it works well,
but I don't know why but it does not send FIN signal right after close() is
called. Therefore I manually set to send FIN signal, but the passive close test
does not detect the segment with FIN signal, which leads to remaining bugs.

Remaining Bugs:
	Passive close does not send multiple FIN in a row (which is the correct
answer of passive close test) But tcp_connection sends FIN anyway.
	tcp_sender sends FIN if the receiver cannot take the signal because of
its window size is full. But it successfully turns off the connection anyway.

- Optional: I had unexpected difficulty with: passive close test

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
