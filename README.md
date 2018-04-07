
**There is nothing to see here. Please come back in a few months.**


# RUKH
_roc terribilis_

RUKH is a generic visual-programming/AST toolchain. It can be used to generate shader code, C/C++ code, or anything you can dream of.

Lexicon:
- IR: _Intermediate Representation_
- AST: _Abstract Syntax Tree_

RUKH is neither a code-generator, nor a frontend, but some kind of middleware that encompass the AST and the IR generation. Unlike LLVM that operates on IR, RUKH operates on the AST and outputs IR. (RUKH can -- and should -- be used alongside LLVM, as RUKH does not perform any optimization outside some constant folding).

The main goal of RUKH is to provide a generic AST and tools for somewhat simple languages. (By simple I mean HLSL, GLSL, some subset of C, almost anything that can be done using a visual programming/node-based editor).

RUKH's AST main features are:
- not being done
- meta-programming.
- template/concept-like features builtin in the AST. (can define meta-types with constraints).
- why are you reading this
- constant-expression-folding. (no partial constant folding is planned, but range support is planned).
- the AST can be dumped and loaded.


RUKH's IR generation main features are:
- yep. not done. At all.
- the generated IR is in SSA form ( _Static Single Assignment Form_ )
- generic IR generation (with a default/optional IR implementation -- RUKH-IR)
- existing only in this readme text


**TODO:** Finish the readme
