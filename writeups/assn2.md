Assignment 2 Writeup
=============

My name: Bae Jaeryun

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 8 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
	For wrap/unwrap routines, I made wrap and unwrap functions. Wrap
function is just adding isn and a given absolute seqNo with type casting of
the absolute seqNo. Unwrapping function makes index by subtraction between
isn and a given seqNo, and makes temp-seqNo by adding checkpoint's 64-32th bits
and index. Then, if checkpoint is closer to temp-seqNo plus or minus 0x100000000
than temp-seqNo, absolute seqNo is added or substracted temp-seqNo, or just
temp-seqNo is absolute seqNo.
	In TCPReceiver, SYN flag is false at first. If the data has SYN, set
the flag true and set isn with its seqNo. If the flag is false and the signal
is not SYN, the data is ignored. Also, seqNo of the data should not be isn if
the data has SYN signal. Or, the data is ignored to prevent the program from
referencing wrong string index.
	After setting basic variables with SYN packet, the recevier only takes
packet when its remaining capacity is bigger than the packet's size. Or the
packet is ignored in reassembler and bytestream.
	The reciver's window size is remaining capacity form bytestream.
	The reciver's ackNo is bytestream's written bytes plus 1 for SYN signal
and plus end signal flag for FIN signal.

Implementation Challenges:
	I was really excited because I made code that theoretically perfect code
in 2 hours, so what I needed to do was only typing make file and watching the 
result without any failure. However, a weird bug call which never give what is
wrong with my code happened. Its bug call is basic_string::_M_replace_aux.
	Without the bug call in the test in recv_window, everything works well.
I spent 6 hours to find the origin of the bug call, but the bug never give any
clue where it came out, so I typed the name on Google.
	Google said that it is because of referencing minus index of a string
variable. I searched for every single line of my codes including byte stream and
reassembler. While searching, I could make a little advancement on reassembler,
but I could not find the origin of the bug.
	Finally, I opened the test case of recv_window and made my code give
its excution log. I could find the evil origin of the bug, which gives an
exceptional case, data with wrong seqNo. It becomes data with 0 absSeqNo, which
becomes -1 to reassembler. I have to add -1 to make string index because of SYN
signal, but in this case, it became a disaster.
	At the end of the long long wandering of bug searching, I could finally
make well-working code which has an exception handling for data with wrong seqNo.

Remaining Bugs:
None

- Optional: I had unexpected difficulty with: The weird bug call, _M_replace_aux, which I wrote on the challenges.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
