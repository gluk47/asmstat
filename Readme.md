# What is it
This is a tool created to help analyzing assembler dumps as produced by either `objdump -Cd` (or alike) or `gdb disassemble`.
This tool draws all jumps in pseudographics to help understand code flow and possible inefficiencies.
Eventually this tool will allow to gather some other statistics such as: how many fills/spills are there in the code, how many XMM instructions... eventually.

# How to build
1. `mkdir build`
1. `cd build`
1. `cmake ..`
1. `make -j$(nproc)`

To build, you need a fresh C++14-enabled compiler, like GCC 5 or clang 3.6.

# How to run

Assuming that `gdb.txt` is the dump, and `asmstat` is the binary produced by make, here you go:

`./asmstat gdb.txt`.

Examples:

```
0000000000408c96 <run(int)>:
      408c96:   55                      push   %rbp
      408c97:   48 89 e5                mov    %rsp,%rbp
      408c9a:   89 7d ec                mov    %edi,-0x14(%rbp)
┌────→408c9d:   83 7d ec 00             cmpl   $0x0,-0x14(%rbp)
│┌───←408ca1:   7e 30                   jle    408cd3 <run(int)+0x3d
││    408ca3:   8b 45 ec                mov    -0x14(%rbp),%eax
││    408ca6:   83 e0 01                and    $0x1,%eax
││    408ca9:   85 c0                   test   %eax,%eax
││┌──←408cab:   74 11                   je     408cbe <run(int)+0x28
│││   408cad:   8b 55 ec                mov    -0x14(%rbp),%edx
        <... stripped out ...>
│││   408cb9:   89 45 ec                mov    %eax,-0x14(%rbp)
│││┌─←408cbc:   eb 0f                   jmp    408ccd <run(int)+0x37
││└──→408cbe:   8b 45 ec                mov    -0x14(%rbp),%eax
││ │  408cc1:   89 c2                   mov    %eax,%edx
        <... stripped out ...>
││ │  408cca:   89 45 ec                mov    %eax,-0x14(%rbp)
││ └─→408ccd:   83 45 fc 01             addl   $0x1,-0x4(%rbp)
└────←408cd1:   eb ca                   jmp    408c9d <run(int)+0x7
 └───→408cd3:   8b 45 fc                mov    -0x4(%rbp),%eax
      408cd6:   5d                      pop    %rbp
      408cd7:   c3                      retq
```

```
      408f7c:   83 7d fc 01             cmpl   $0x1,-0x4(%rbp)
┌────←408f80:   75 27                   jne    408fa9 <__static_initialization_and_destruction_0(int, int)+0x3b
│     408f82:   81 7d f8 ff ff 00 00    cmpl   $0xffff,-0x8(%rbp)
├────←408f89:   75 1e                   jne    408fa9 <__static_initialization_and_destruction_0(int, int)+0x3b
│     408f8b:   bf b1 f3 60 00          mov    $0x60f3b1,%edi
│     408f90:   e8 ab f9 ff ff          callq  408940 <std::ios_base::Init::Init()@plt>
│     408f95:   ba 78 f1 60 00          mov    $0x60f178,%edx
│     408f9a:   be b1 f3 60 00          mov    $0x60f3b1,%esi
│     408f9f:   bf 80 89 40 00          mov    $0x408980,%edi
│     408fa4:   e8 c7 f9 ff ff          callq  408970 <__cxa_atexit@plt>
└────→408fa9:   90                      nop
      408faa:   c9                      leaveq
      408fab:   c3                      retq
```
