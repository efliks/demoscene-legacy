<img src="./sample.png" alt="Sample image" width="750" />

# demoscene-legacy

Back in the early 2000s, I was part of a Polish online group of
Assembler enthusiasts called New Age Assembler Group or NAAG 
(now defunct). Some of our works made it to the broader demoscene. 
For example, in June 2002 we released our only zine that can still
be found [here](http://www.pouet.net/prod.php?which=50666).

Here is a collection of graphical effects and intros written originally 
for MS-DOS in x86 Assembly language. Some of these effects were published on 
[Demozoo](https://demozoo.org/sceners/70017/), a demoscene site.

This collection also contained effects written in C, but I have made 
a [separate repository](https://github.com/efliks/doseffects) for them.

## Running programs in a DOS emulator

Despite DOS being long gone, it is quite easy to run the programs in
an emulator, such as [DOSBox](http://www.dosbox.com/). Under Linux, 
enter each program's directory and type in, for example:

```
linux> dosbox 3DFLAG.COM
```

The program will now run. To exit, press [ESC] and type in "exit"
after returning to the DOSBox console. Some graphical effects also
react to pressing [SPACE] (change rendering mode, etc).

Usually, the programs will run very slow. It is possible to accelerate
them to some degree by repeatedly pressing [CTRL+SHIFT+F12], 
which will increase the number of "cycles" in DOSBox.

To run them at their original speed, it might be necessary to set up 
a dedicated old computer with something like
[FreeDOS](http://www.freedos.org/) on board. Since the development 
was originally done on a Pentium 133 MHz system, any computer will do 
just fine.


## Running programs using protected mode

Some programs (*fogtro*, *3dfontz*) were written in protected mode 
and require a DOS extender.

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

The source codes may be a bit challenging to compile. Back in the day, 
I used compilers from Borland, TASM and Turbo C. Today, the problem seems 
to be with the "ideal" syntax of Borland, which is not recognized by 
modern compilers, such as GNU Assembler or NASM.

Luckily, TASM seems to have become abandonware and is available 
on the web, for example [here](http://trimtab.ca/assets/files/tasm.zip). 
Unpack the ZIP archive in the main directory and start DOSBox:

```
linux> cd demoscene-legacy/
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

Programs written for protected mode have in general 
a BAT file with compilation instructions in it. The file is 
usually called "\_make.bat". I managed to build *fogtro* in 
the following way:

```
dosbox> cd fogtro/
dosbox> c:\tasm\bin\tasm.exe fogtro! /m /p /q
dosbox> c:\tasm\bin\tlink.exe fogtro! kernel\kernel,fogtro!.exe /3 /x
```

Finally, one program *chemol* is unusual in that it can be built 
directly under Linux with NASM:

```
linux> nasm -f bin chemol.asm -o chemol.com
```

NASM will generate a 16-bit binary that runs under DOSBox.


## Final notes

The reason for publishing these programs here is solely because I think
they may still have some educational, fun or nostalgy value for some 
people.

I am the only developer of the programs. Licensing information can be 
found in COPYING.
