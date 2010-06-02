#include <Qsci/qscilexerasm.h>

QsciLexerASM::QsciLexerASM(QObject *parent) :
    QsciLexer(parent)
{
}

QsciLexerASM::~QsciLexerASM()
{
}

// Return the string of characters that comprise a word.
const char *QsciLexerASM::wordCharacters() const
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
}

// Return the list of characters that can start a block.
const char *QsciLexerASM::blockStart(int *style) const
{
    if (style)
        *style = SCE_ASM_OPERATOR;

    return "proc";
}


// Return the list of characters that can end a block.
const char *QsciLexerASM::blockEnd(int *style) const
{
    if (style)
        *style = SCE_ASM_OPERATOR;

    return "endp";
}

// Return the list of keywords that can start a block.
const char *QsciLexerASM::blockStartKeyword(int *style) const
{
    if (style)
        *style = SCE_ASM_DIRECTIVE;

    return
        "proc endp";
}

// Returns the set of keywords.
const char *QsciLexerASM::keywords(int set) const
{
  // CPU instructions
  if (set == 1)
    return
        // Original 8086/8088 instructions
        "aaa aad aam aas adc add and call cbw clc cld cli cmc cmp cmpsb cmpsw cwd "
        "daa das dec div esc hlt idiv imul in inc int into iret "
        "jmp ja jae jb jbe jc jcxz je jg jge jl jle jna jnae jnb jnbe jnc jne "
        "jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz "
        "lahf lds lea les lock lodsb lodsw loop loopne loopnz loopz "
        "mov movsb movsw mul neg nop not or out pop push popf pushf "
        "rcl rcr rep repe repne repnz repz ret retn retf rol ror "
        "sahf sal sar sbb scasb scasw shl shr stc std sti stosb stosw sub "
        "test wait xchg xlat xor "
        // Added with 80186/80188
        "bound ins leave outs popa pusha "
        // Added with 80286
        "arpl clts lar lgdt lidt lldt lmsw loadall lsl ltr sgdt sidt sldt smsw str verr verw "
        // Added with 80386
        "bsf bsr bt btc btr bts cdq cmpsd cwde insb insw insd "
        "iretd "
        "jcxz jecxz lfs lgs lss lodsd loopw loopd loopew looped loopzw loopzd "
        "loopnew loopned loopnzw loopnzd movsw movsd movsx movzx "
        "popad popfd pushad pushfd scasd "
        "seta setae setb setbe setc sete setg setge setl setle setna setnae setnb "
        "setnbe setnc setne setng setnge setnl setnle setno setnp setns setnz "
        "seto setp setpe setpo sets setz shld shrd stosb stosw stosd "
        // Added with 80486
        "bswap cmpxchg invd invlpg wbinvd xadd "
        // Added with Pentium
        "cpuid cmpxchg8b rdmsr rdtsc wrmsr rsm "
        // Added with Pentium MMX
        "rdpmc "
        // Added with Pentium Pro
        "cmova cmovae cmovb cmovbe cmovc cmove cmovg cmovge cmovl cmovle cmovna "
        "cmovnae cmovnb cmovnbe cmovnc cmovne cmovng cmovnge cmovnl cmovnle cmovno "
        "cmovnp cmovns cmovnz cmovo cmovp cmovpe cmovpo cmovs cmovz sysenter "
        "sysexit ud2 "
        // Added with AMD K6-2
        "syscall sysret "
        // Added with SSE
        "maskmovq movntps movntq prefetch0 prefetch1 prefetch2 prefetchnta sfence "
        // Added with SSE2
        "clflush lfence maskmovdqu mfence movntdq movnti movntpd pause "
        // Added with SSE3
        "lddqu monitor mwait "
        // Added with Intel VT
        "vmptrld vmptrst vmclear vmread vmwrite vmcall vmlaunch vmresume vmxoff vmxon "
        // Added with AMD-V
        "clgi skinit stgi vmload vmmcall vmrun vmsave "
        // Added with x86-64
        "cmpxchg16b rdtscp "
        // Added with SSE4a
        "lzcnt popcnt "
        ;

  // FPU instructions
  if (set == 2)
    return
        // x87 floating-point instructions
        "f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp fdisi fdiv fdivp fdivr fdivrp feni ffree fiadd ficom ficomp fidiv fidivr fild "
        "fimul fincstp finit fist fistp fisub fisubr fld fld1 fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul fmulp fnclex fndisi fneni fninit "
        "fnop fnsave fnsavew fnstcw fnstenv fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw fsave fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp "
        "fstsw fsub fsubp fsubr fsubrp ftst fwait fxam fxch fxtract fyl2x fyl2xp1 "
        // Added with 80287
        "fsetpm "
        // Added with 80387
        "fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin fsincos fstenvd fucom fucomp fucompp "
        // Added with 80487
        "fcmovb fcmovbe fcmove fcmovnb fcmovnbe fcmovne fcmovnu fcmovu fcomi fcomip fucomi fucomip "
        // Added with SSE
        "fxrstor fxsave "
        // Added with SSE3
        "fisttp "
        // Undocumented instructions
        "ffreep "
        ;

  // registers
  if (set == 3)
    return "ah al ax bh bl bp bx ch cl cr0 cr2 cr3 cr4 cs cx dh di dl dr0 dr1 dr2 dr3 dr6 dr7 ds dx eax ebp ebx ecx edi edx es esi esp fs gs si sp ss st tr3 tr4 tr5 tr6 tr7 st0 st1 st2 st3 st4 st5 st6 st7 mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7";

  // directives
  if (set == 4)
    return "186 .286 .286c .286p .287 .386 .386c .386p .387 .486 .486p .8086 .8087 .alpha .break .code .const .continue .cref .data .data? .dosseg .else .elseif .endif .endw .err .err1 .err2 .errb .errdef .errdif .errdifi .erre .erridn .erridni .errnb .errndef .errnz .exit .fardata .fardata? .if .lall .lfcond .list .listall .listif .listmacro .listmacroall .model .no87 .nocref .nolist .nolistif .nolistmacro .radix .repeat .sall .seq .sfcond .stack .startup .tfcond .type .until .untilcxz .while .xall .xcref .xlist alias align assume catstr comm comment db dd df dosseg dq dt dup dw echo else elseif elseif1 elseif2 elseifb elseifdef elseifdif elseifdifi elseife elseifidn elseifidni elseifnb elseifndef end endif endm endp ends eq equ even exitm extern externdef extrn for forc ge goto group gt high highword if if1 if2 ifb ifdef ifdif ifdifi ife ifidn ifidni ifnb ifndef include includelib instr invoke irp irpc label le length lengthof local low lowword lroffset lt macro mask mod .msfloat name ne offset opattr option org %out page popcontext proc proto ptr public purge pushcontext record repeat rept seg segment short size sizeof sizestr struc struct substr subtitle subttl textequ this title type typedef union while width db dw dd dq dt resb resw resd resq rest incbin equ times %define %idefine %xdefine %xidefine %undef %assign %iassign %strlen %substr %macro %imacro %endmacro %rotate .nolist %if %elif %else %endif %ifdef %ifndef %elifdef %elifndef %ifmacro %ifnmacro %elifmacro %elifnmacro %ifctk %ifnctk %elifctk %elifnctk %ifidn %ifnidn %elifidn %elifnidn %ifidni %ifnidni %elifidni %elifnidni %ifid %ifnid %elifid %elifnid %ifstr %ifnstr %elifstr %elifnstr %ifnum %ifnnum %elifnum %elifnnum %error %rep %endrep %exitrep %include %push %pop %repl struct endstruc istruc at iend align alignb %arg %stacksize %local %line bits use16 use32 section absolute extern global common cpu org section group import export ";

  // directives operands (?)
  if (set == 5)
    return         // modifiers
        "$ ? @b @f addr basic byte c carry? dword far far16 fortran fword near near16 "
        "overflow? parity? pascal qword real4 real8 real10 sbyte sdword sign? stdcall "
        "sword syscall tbyte vararg word zero? flat near32 far32 abs all assumes at "
        "casemap common compact cpu dotname emulator epilogue error export expr16 expr32 "
        "farstack flat forceframe huge language large listing ljmp loadds m510 medium "
        "memory nearstack nodotname noemulator nokeyword noljmp nom510 none nonunique "
        "nooldmacros nooldstructs noreadonly noscoped nosignextend nothing notpublic "
        "oldmacros oldstructs os_dos para private prologue radix readonly req scoped "
        "setif2 smallstack tiny use16 use32 uses a16 a32 o16 o32 byte word dword nosplit "
        "$ $$ seq wrt flat large small .text .data .bss near far %0 %1 %2 %3 %4 %5 %6 %7 %8 %9 ";


  // extended instructions
  if (set == 6)
    return
        // MMX instructions
        "emms movd movq packssdw packsswb packuswb paddb paddd paddsb paddsw paddusb paddusw paddw pand pandn pcmpeqb pcmpeqd pcmpeqw pcmpgtb pcmpgtd pcmpgtw "
        "pmaddwd pmulhw pmullw por pslld psllq psllw psrad psraw psrld psrlq psrlw psubb psubd psubsb psubsw psubusb psubusw psubw punpckhbw punpckhdq "
        "punpckhwd punpcklbw punpckldq punpcklwd pxor "
        // EMMX  instructions
        "paveb paddsiw pmagw pdistib psubsiw pmvzb pmulhrw pmvnzb pmvlzb pmvgezb pmulhriw pmachriw "
        // 3DNow! instructions
        "femms pavgusb pf2id pfacc pfadd pfcmpeq pfcmpge pfcmpgt pfmax pfmin pfmul pfrcp pfrcpit1 pfrcpit2 pfrsqit1 pfrsqrt pfsub pfsubr pi2fd pmulhrw prefetch prefetchw "
        // 3DNow!+ instructions
        "pf2iw pfnacc pfpnacc pi2fw pswapd pfrsqrtv pfrcpv "
        // SSE2 instructions
        "addpd addsd andnpd andpd cmppd cmpsd comisd cvtdq2pd cvtdq2ps cvtpd2dq cvtpd2pi cvtpd2ps cvtpi2pd cvtps2dq cvtps2pd cvtsd2si cvtsd2ss cvtsi2sd "
        "cvtss2sd cvttpd2dq cvttpd2pi cvttps2dq cvttsd2si divpd divsd maxpd maxsd minpd minsd movapd movhpd movlpd movmskpd movsd movupd mulpd mulsd orpd "
        "shufpd sqrtpd sqrtsd subpd subsd ucomisd unpckhpd unpcklpd xorpd movdq2q movdqa movdqu movq2dq paddq psubq pmuludq pshufhw pshuflw pshufd pslldq "
        "psrldq punpckhqdq punpcklqdq "
        // SSE3 instructions
        "addsubpd addsubps haddpd haddps hsubpd hsubps movddup movshdup movsldup psignw psignd psignb pshufb pmulhrsw pmaddubsw phsubw "
        "phsubsw phsubd phaddw phaddsw phaddd palignr pabsw pabsd pabsb "
        // SSE4 instructions
        "mpsadbw phminposuw pmulld pmuldq dpps dppd blendps blendpd blendvps blendvpd pblendvb pblendw pminsb pmaxsb pminuw pmaxuw pminud pmaxud pminsd pmaxsd "
        "roundps roundss roundpd roundsd insertps pinsrb pinsrd pinsrq extractps pextrb pextrw pextrd pextrq pmovsxbw pmovzxbw pmovsxbd pmovzxbd pmovsxbq "
        "pmovzxbq pmovsxwd pmovzxwd pmovsxwq pmovzxwq pmovsxdq pmovzxdq ptest pcmpeqq packusdw movntdqa lzcnt popcnt extrq insertq movntsd movntss crc32 "
        "pcmpestri pcmpestrm pcmpistri pcmpistrm pcmpgtq "
        ;

    return 0;
}


// Returns the user name of a style.
QString QsciLexerASM::description(int style) const
{
  switch (style)
  {
  case SCE_ASM_DEFAULT:
      return tr("Default");
  case SCE_ASM_COMMENT:
      return tr("Comment");
  case SCE_ASM_NUMBER:
      return tr("Number");
  case SCE_ASM_STRING:
      return tr("String");
  case SCE_ASM_OPERATOR:
      return tr("Operator");
  case SCE_ASM_IDENTIFIER:
      return tr("Identifier");
  case SCE_ASM_CPUINSTRUCTION:
      return tr("CPU Instruction");
  case SCE_ASM_MATHINSTRUCTION:
      return tr("Math Instruction");
  case SCE_ASM_REGISTER:
      return tr("Register");
  case SCE_ASM_DIRECTIVE:
      return tr("Directive");
  case SCE_ASM_DIRECTIVEOPERAND:
      return tr("Directive Operand");
  case SCE_ASM_COMMENTBLOCK:
      return tr("Comment Block");
  case SCE_ASM_CHARACTER:
      return tr("Character");
  case SCE_ASM_STRINGEOL:
      return tr("String EOL");
  case SCE_ASM_EXTINSTRUCTION:
      return tr("Extended Instruction");
  }

  return QString();
}

QColor QsciLexerASM::defaultColor(int style) const
{
  switch (style)
  {
  case SCE_ASM_DEFAULT:
    return Qt::black;
  case SCE_ASM_COMMENT:
  case SCE_ASM_COMMENTBLOCK:
    return Qt::darkGray;
  case SCE_ASM_NUMBER:
    return Qt::darkBlue;
  case SCE_ASM_STRING:
    return Qt::darkMagenta;
  case SCE_ASM_STRINGEOL:
    return Qt::red;
  case SCE_ASM_CHARACTER:
    return Qt::darkGreen;
  case SCE_ASM_OPERATOR:
    return Qt::magenta;
  case SCE_ASM_IDENTIFIER:
    return Qt::black;
  case SCE_ASM_CPUINSTRUCTION:
    return Qt::blue;
  case SCE_ASM_MATHINSTRUCTION:
    return Qt::darkCyan;
  case SCE_ASM_REGISTER:
    return Qt::red;
  case SCE_ASM_DIRECTIVE:
    return Qt::darkRed;
  case SCE_ASM_DIRECTIVEOPERAND:
    return Qt::darkGreen;
  case SCE_ASM_EXTINSTRUCTION:
    return Qt::darkYellow;
  }

  return Qt::black;
}
