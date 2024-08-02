#pragma once

#define ARGTYPE(x) ARGTYPE_PASS2(ARGTYPE_PASS1 x,)
#define ARGTYPE_PASS1(...) (__VA_ARGS__),
#define ARGTYPE_PASS2(...) ARGTYPE_PASS3((__VA_ARGS__))
#define ARGTYPE_PASS3(x)   ARGTYPE_PASS4 x
#define ARGTYPE_PASS4(x, ...) REM x
#define REM(...) __VA_ARGS__

#define NARGS(...) NARGS_PASS2(NARGS_PASS1(__VA_ARGS__))
#define NARGS_PASS1(...) unused, __VA_ARGS__
#define NARGS_PASS2(...) NARGS_PASS4(NARGS_PASS3(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define NARGS_PASS3(_unused,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,VAL, ...) VAL
#define NARGS_PASS4(x) x
#define ARGPAIR(x) REM x

#define ARGNAME(x) EAT x
#define EAT(...)

#define ARGNAME_AS_STRING(x) TOSTRING(EAT x)
#define TOSTRING2(x)  #x
#define TOSTRING(x)   TOSTRING2(x)

#define CAT_TOKENS(x, y) CAT_PASS1((x, y))
#define CAT_PASS1(x) PRIMITIVE_CAT x
#define PRIMITIVE_CAT(x, y) x ## y
#define DOFOREACH_PASS1(m, x) m x

#define DOFOREACH_SEMICOLON(macro, ...) DOFOREACH_PASS1(CAT_TOKENS(DOFOREACH_SEMICOLON_, NARGS(__VA_ARGS__)), (macro, __VA_ARGS__))

#define DOFOREACH_SEMICOLON_0(m)
#define DOFOREACH_SEMICOLON_1(m, x1) m(x1);
#define DOFOREACH_SEMICOLON_2(m, x1, x2) m(x1); m(x2);
#define DOFOREACH_SEMICOLON_3(m, x1, x2, x3) m(x1); m(x2); m(x3);
#define DOFOREACH_SEMICOLON_4(m, x1, x2, x3, x4) m(x1); m(x2); m(x3); m(x4);
#define DOFOREACH_SEMICOLON_5(m, x1, x2, x3, x4, x5) m(x1); m(x2); m(x3); m(x4); m(x5);
#define DOFOREACH_SEMICOLON_6(m, x1, x2, x3, x4, x5, x6)  m(x1); m(x2); m(x3); m(x4); m(x5); m(x6);
#define DOFOREACH_SEMICOLON_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7);
#define DOFOREACH_SEMICOLON_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8);
#define DOFOREACH_SEMICOLON_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9);
#define DOFOREACH_SEMICOLON_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10);
#define DOFOREACH_SEMICOLON_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10); m(x11);
#define DOFOREACH_SEMICOLON_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10); m(x11); m(x12);
#define DOFOREACH_SEMICOLON_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10); m(x11); m(x12); m(x13);
#define DOFOREACH_SEMICOLON_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10); m(x11); m(x12); m(x13); m(x14);
#define DOFOREACH_SEMICOLON_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) m(x1); m(x2); m(x3); m(x4); m(x5); m(x6); m(x7); m(x8); m(x9); m(x10); m(x11); m(x12); m(x13); m(x14); m(x15);