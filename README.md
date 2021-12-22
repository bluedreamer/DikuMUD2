# DikuMUD2
DikuMUD II (1997)
------------
This is the source of the never before released DikuMUD II. The source code is
released under LGPL (only) and originates from a running version from December 1997.

Check out history and notes on www.dikumud.com

March 3, 2020:
  *  It works. But not completely done. Waiting for Makefile from
     Ken Perry regarding PP. You can run the ppmake script though.

  * I've run into a number of bugs that were there for 5-7 years and never before
    caused issues. That's quite normal in my experience with radically new
    enviroments. So dont be suprised if there's a crash bug here or there. I'll
    try to help if I can.

  * Back when this version was retired it was quite stable.

  * It may be that the Perry brothers release Diku III (Valhalla MUD)
    under LGPL too. We're working together on it right now and looking
    into adding HTML5 / Websockets support.
    
  * @nerratt reported that it compiles on OpenSuse if you remove
    the declaration of gettimeofday in main.c.
    

-----------
February 2020, Michael Seifert

I've compiled this old venerable project for Ubuntu 19. Making the old wheels turn on a 64 bit architecture :) 

A few notes on getting it running.
Make sure you have cmake installed and preferably ninja (it builds faster than make)

1) Clone the repo

     mkdir debug_build
     cd debug_build

1.5) Be sure you have flex and bison installed. Look e.g. here for help:
       https://stackoverflow.com/questions/3871425/how-to-install-lex-and-yacc-in-ubuntu

2) Compile
     cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
     make
   
   or
   
     cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
     ninja

3) Make symbolic links (in dm-dist-ii/)
     cd ../dm-dist-ii
     ln -sf ../debug_build/dm-dist-ii/dmserver
     ln -sf ../../debug_build/dm-dist-ii/Mplex/mplex Mplex/mplex
     ln -sf ../debug_build/dm-dist-ii/PP/pp
     ln -sf ../../debug_build/dm-dist-ii/DMC/dmc DMC/dmc
     ln -sf ../../debug_build/dm-dist-ii/DIL/dil DIL/dil

4) when in the main dir (dm-dist-ii/) compile all zones:
      DMC/dmc zon/*.zon

------------

To run DikuMUD II:

1) Get the Mplex/mplex up and running.
      Mplex/mplex &
   This program accepts (telnet) connections and in turn communicates with
   the dmserver. Default port is 4242. There's a way to have one with color
   support and one without. The multiplexers communicate with the dmserver
   on port 4999

2) Run Diku II:

     ./dmserver
     (or gdb dmserver)

3) Login with the admin
     telnet localhost 4242
     username: admin
     password (CIA grade): ABC123

   Change the password, or advance a new character to level 255, then quit
   and delete the admin character from the ply/a/ directory

-----------------
Wish list

If I had real time to spend on this project my priority number one
would be to use SmartPtr with all unit_data and affects. Get rid of
all the complicated and not 100% reliable destructor code and replace it
then by freeing objects with a reference count of zero.

My second priority would be to make more use of C classes for strings, name lists and more to handle more memory in the class rather than juggling mallocs and frees.

Also, given we're in 2020. Why isn't there a pure html5/js client anywhere? :o)
Looks like html5/websockets would be the way to go and that would be quite easy to implement in the Diku II 
code because it's separated out into the Mplex. So you could run one Mplex for telnet, one for telnet with color
and one for web-sockets. And then add just a few lines of code to your webpage and you have a basic MUD client.
Which of course could be wonderfully expanded with a little meta-data
from the server.
