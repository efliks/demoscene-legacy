# dos-gfx

Here is a collection of 30+ graphical effects, intros and games, 
written originally for MS-DOS.


## Running programs in a DOS emulator

Despite DOS being long gone, it is quite easy to run the programs in
an emulator, such as [DOSBox](http://www.dosbox.com/). If you are 
under Linux, enter each program's directory and type in, for example:

```
linux> dosbox 3DFLAG.COM
```

The program will now run. To exit, press [ESC] and type in "exit"
after returning to the DOSBox console. Some graphical effects also
react to pressing [SPACE] (change rendering mode, etc).

Usually, the programs will run very slow. It is possible to accelerate
them to some degree by repeatedly pressing [CTRL+SHIFT+F12], 
which will increase the number of "cycles" in DOSBox.

If you want to run them at original speed, it might be necessary 
to set up a dedicated old computer with something like
[FreeDOS](http://www.freedos.org/) on board. Since the development 
was originally done on a Pentium 133 MHz, any computer will do 
just fine.


## Running programs in protected mode

Some programs, both Assembler (*fogtro*, *3dfontz*) and C (*wheel2*,
*flatgrd2*), were written in protected mode and require a DOS extender.

Download the CWSDPMI extender from 
[here](https://web.archive.org/web/20151217064947/http://homer.rice.edu/~sandmann/cwsdpmi/index.html).
I have only used the
[binary distribution](https://web.archive.org/web/20151217064947/http://homer.rice.edu/~sandmann/cwsdpmi/csdpmi7b.zip)
of CWSDPMI. Unpack the ZIP file and proceed as follows:

```
linux> cp cwsdpmi/bin/CWSDPMI.EXE fogtro/
linux> cd fogtro/
linux> dosbox .
```

Once in DOSBox, start the extender first, followed by the actual
program:

```
dosbox> CWSDPMI.EXE
dosbox> FOGTRO!.EXE
```


## Compilation how-to

Executable files, COM and EXE, are attached. If you would like to 
recompile the programs, here is (more or less) how to do it.

The source codes, which are essentially Assembler X86 and 
sometimes C, may be a bit challenging to compile. Back in the day, 
I used compilers from Borland, TASM and Turbo C. In the case 
of Assembler codes, the problem seems to be with the "ideal" syntax 
of Borland, which is not recognized by modern compilers, such as
GNU Assembler or NASM.

Luckily, TASM seems to have become abandonware and is available 
on the web, for example [here](http://trimtab.ca/assets/files/tasm.zip). 
Unpack the ZIP archive in the main directory of dos-gfx and 
start DOSBox:

```
linux> cd dos-gfx/
linux> dosbox .
```

In the next step, enter a program's directory and compile it, 
for example:

```
dosbox> cd bline/
dosbox> c:\tasm\bin\tasm.exe b_line.asm
```

The above command will generate a "b\_line.com" executable that is ready
to run. Note the Microsoft-style backslashes.

Programs written for the protected mode have in general 
a BAT file with compilation instructions in it. The file is 
usually called "\_make.bat". I managed to build *fogtro* in 
the following way:

```
dosbox> cd fogtro/
dosbox> c:\tasm\bin\tasm.exe fogtro! /m /p /q
dosbox> c:\tasm\bin\tlink.exe fogtro! kernel\kernel,fogtro!.exe /3 /x
```

I have not tried to compile C programs written for DOS, but I
may try in the future.

Finally, two programs are unusual in the sense that they can 
be built directly under Linux. *mrt* is a console-based program 
written in standard C and compiles easily with GCC. *chemol* was 
my last Assembler program and it was written with NASM in mind:

```
linux> nasm -f bin chemol.asm -o chemol.com
```

NASM will generate a 16-bit binary that runs under DOSBox.


## Final notes

The reason for publishing these programs here is solely because I think
they may still have some educational, fun or nostalgy value for some 
people.

For any questions, please feel free to contact me by e-mail: 
<mikolaj [dot] feliks [at] gmail [dot] com>.
I am the only developer of the programs. Licensing information can be 
found in COPYING.
