## **Program 4 – CS 344**

## Overview

In this assignment, I created five small programs that encrypt and decrypt information using a one-time pad-like system.

These programs serve as a capstone to what I have been learning in operating system I, and combine the multi-processing code I have been learning with socket-based inter-process communication. My programs also accessible from the command line using standard UNIX features like input/output redirection, and job control. I wrote a short compilation script.

## Assignment Specifications

All execution, compiling, and testing of this program should ONLY be done in the bash prompt on our class server!

Use the following link as your primary reference on One-Time Pads (OTP):

[http://en.wikipedia.org/wiki/One-time_pad (Links to an external site.)](http://en.wikipedia.org/wiki/One-time_pad)

The following definitions will be important:

**Plaintext** is the term for the information that you wish to encrypt and protect. It is human readable.

**Ciphertext** is the term for the plaintext after it has been encrypted by your programs. Ciphertext is not human-readable, and in fact cannot be cracked, if the OTP system is used correctly.

A **Key** is the random sequence of characters that will be used to convert Plaintext to Ciphertext, and back again. It must not be re-used, or else the encryption is in danger of being broken.

The following excerpt from this Wikipedia article was captured on 2/21/2015:

“Suppose Alice wishes to send the message "HELLO" to Bob. Assume two pads of paper containing identical random sequences of letters were somehow previously produced and securely issued to both. Alice chooses the appropriate unused page from the pad. The way to do this is normally arranged for in advance, as for instance 'use the 12th sheet on 1 May', or 'use the next available sheet for the next message'.

The material on the selected sheet is the key for this message. Each letter from the pad will be combined in a predetermined way with one letter of the message. (It is common, but not required, to assign each letter a numerical value, e.g., "A" is 0, "B" is 1, and so on.)

In this example, the technique is to combine the key and the message using modular addition. The numerical values of corresponding message and key letters are *added* together, modulo 26. So, if key material begins with "XMCKL" and the message is "HELLO", then the coding would be done as follows:

```
      H       E       L       L       O  message
   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) message
+ 23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= 30      16      13      21      25     message + key
=  4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) message + key (mod 26)
      E       Q       N       V       Z  → ciphertext
```

If a number is larger than 26, then the remainder, after *subtraction* of 26, is taken [as the result]. This simply means that if the computations "go past" Z, the sequence starts again at A.

The ciphertext to be sent to Bob is thus "EQNVZ". Bob uses the matching key page and the same process, but in reverse, to obtain the plaintext. Here the key is *subtracted* from the ciphertext, again using modular arithmetic:

```
       E       Q       N       V       Z  ciphertext
    4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) ciphertext
-  23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= -19       4      11      11      14     ciphertext – key
=   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) ciphertext – key (mod 26)
       H       E       L       L       O  → message
```

Similar to the above, if a number is negative then 26 is *added* to make the number zero or higher.

Thus Bob recovers Alice's plaintext, the message "HELLO". Both Alice and Bob destroy the key sheet immediately after use, thus preventing reuse and an attack against the cipher.”

Your program will encrypt and decrypt plaintext into ciphertext, using a key, in exactly the same fashion as above, except it will be using modulo 27 operations: your 27 characters are the 26 capital letters, and the space character ( ). All 27 characters will be encrypted and decrypted as above.

To do this, you will be creating five small programs in C. Two of these will function like "daemons" (but aren't actually daemons), and will be accessed using network sockets. Two will use the daemons to perform work, and the last is a standalone utility.

Your programs must use the network calls we've discussed in class (send(), recv(), socket(), bind(), listen(), & accept()) to send and receive sequences of bytes for the purposes of encryption and decryption by the appropriate daemons. The whole point is to use the network, even though for testing purposes we're using the same machine: if you just open() the datafiles from the server without using the network calls, you'll receive 0 points on the assignment.

Here are the specifications of the five programs:

**otp_enc_d:** This program will run in the background as a daemon. Upon execution, otp_enc_d must output an error if it cannot be run due to a network error, such as the ports being unavailable. Its function is to perform the actual encoding, as described above in the Wikipedia quote. This program will listen on a particular port/socket, assigned when it is first ran (see syntax below). When a connection is made, otp_enc_d must call accept() to generate the socket used for actual communication, and then use a separate process to handle the rest of the transaction (see below), which will occur on the newly accepted socket.

This child process of otp_enc_d must first check to make sure it is communicating with otp_enc (see otp_enc, below). After verifying that the connection to otp_enc_d is coming from otp_enc, then this child receives from otp_enc plaintext and a key via the communication socket (not the original listen socket). The otp_enc_d child will then write back the ciphertext to the otp_enc process that it is connected to via the same communication socket. Note that the key passed in must be at least as big as the plaintext.

Your version of otp_enc_d must support up to five *concurrent* socket connections running at the same time; this is different than the number of processes that could queue up on your listening socket (which is specified in the second parameter of the listen() call). Again, only in the child process will the actual encryption take place, and the ciphertext be written back: the original server daemon process continues listening for new connections, not encrypting data.

In terms of creating that child process as described above, you may either create with fork() a *new* process every time a connection is made, *or* set up a pool of five processes at the beginning of the program, before connections are allowed, to handle your encryption tasks. As above, your system must be able to do five separate encryptions at once, using either method you choose.

Use this syntax for otp_enc_d:

```
otp_enc_d listening_port
```

*listening_port* is the port that otp_enc_d should listen on. You will always start otp_enc_d in the background, as follows (the port 57171 is just an example; yours should be able to use any port):

```
$ otp_enc_d 57171 &
```

In all error situations, this program must output errors to stderr as appropriate (see grading script below for details), but should not crash or otherwise exit, unless the errors happen when the program is starting up (i.e. are part of the networking start up protocols like bind()). If given bad input, once running, otp_enc_d should recognize the bad input, report an error to stderr, and continue to run. Generally speaking, though, this daemon shouldn't receive bad input, since that should be discovered and handled in the client first. All error text must be output to *stderr*.

This program, and the other 3 network programs, should use "localhost" as the target IP address/host. This makes them use the actual computer they all share as the target for the networking connections.

**otp_enc:** This program connects to otp_enc_d, and asks it to perform a one-time pad style encryption as detailed above. By itself, otp_enc doesn’t do the encryption - otp_enc_d does. The syntax of otp_enc is as follows:

```
otp_enc plaintext key port
```

In this syntax, *plaintext* is the name of a file in the current directory that contains the plaintext you wish to encrypt. Similarly, *key* contains the encryption key you wish to use to encrypt the text. Finally, *port* is the port that otp_enc should attempt to connect to otp_enc_d on.

When otp_enc receives the ciphertext back from otp_enc_d, it should output it to *stdout*. Thus, otp_enc can be launched in any of the following methods, and should send its output appropriately:

```
$ otp_enc myplaintext mykey 57171
$ otp_enc myplaintext mykey 57171 > myciphertext
$ otp_enc myplaintext mykey 57171 > myciphertext &
```

If otp_enc receives key or plaintext files with ANY bad characters in them, or the key file is shorter than the plaintext, then it should terminate, send appropriate error text to stderr, and set the exit value to 1.

otp_enc should NOT be able to connect to otp_dec_d, even if it tries to connect on the correct port - you'll need to have the programs reject each other. If this happens, otp_enc should report the rejection to stderr and then terminate itself. In more detail: if otp_enc cannot connect to the otp_enc_d server, for any reason (including that it has accidentally tried to connect to the otp_dec_d server), it should report this error to stderr with the attempted port, and set the exit value to 2. Otherwise, upon successfully running and terminating, otp_enc should set the exit value to 0.

Again, any and all error text must be output to stderr (not into the plaintext or ciphertext files).

**otp_dec_d:** This program performs exactly like otp_enc_d, in syntax and usage. In this case, however, otp_dec_d will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to otp_dec.

**otp_dec:** Similarly, this program will connect to otp_dec_d and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like otp_enc, and must be runnable in the same three ways. otp_dec should NOT be able to connect to otp_enc_d, even if it tries to connect on the correct port - you'll need to have the programs reject each other, as described in otp_enc.

**keygen:** This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard UNIX randomization methods. Do not create spaces every five characters, as has been historically done. Note that you specifically do not have to do any fancy random number generation: we’re not looking for cryptographically secure random number generation! rand() is just fine. The last character keygen outputs should be a newline. All error text must be output to *stderr*, if any.

The syntax for keygen is as follows:

```
keygen keylength
```

Where *keylength* is the length of the key file in characters. keygen outputs to stdout. Here is an example run, which redirects stdout to a key file of 256 characters called “mykey” (note that mykey is 257 characters long because of the newline):

```
$ keygen 256 > mykey
```