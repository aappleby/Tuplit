lua parse function tree

statlist
  statement
    ;
    if
    while
    do
    for
    repeat
    function declaration
      funcstat
    local
    ::
    return
    break
    goto
    function call or assignment
      exprstat
        function call?
          getcode
        assignment?
          assignment2
            comma-separated lhs?
              primaryexp
              assignment2
            single lhs?
              explist
                comma-separated list of expressions
                  expr
                    subexpr
                      unary op?
                        subexpr
                        luaK_prefix
                          op = '-'?
                          fold if lhs is a constant
                          alloc reg
                          codearith
                            generate UNM code for exp into reg
                      binary op?
                        
              nexps != nvars?
                adjust_assign
              else
                setoneret
                storevar
      

luaK_exp2anyreg(fs, e)
  luaK_dischargevars(fs, e)

// Code generator for arithmetic
codearith
  if we can fold constants, do so and return.
  reg2 = luaK_exp2RK (right hand side)
  reg1 = luaK_exp2RK (left hand side)
  free reg 1 and 2
  generate code for op
    luaK_codeABC
  change expdesc 1 to pc + VRELOCABLE
   




typedef enum {
  VVOID,	    /* no value */
  VNIL,
  VTRUE,
  VFALSE,
  VK,		      /* info = index of constant in `k' */
  VKNUM,	    /* nval = numerical value */
  VNONRELOC,	/* info = result register */
  VLOCAL,	    /* info = local register */
  VUPVAL,     /* info = index of upvalue in 'upvalues' */
  VINDEXED,	  /* t = table register/upvalue; idx = index R/K */
  VJMP,		    /* info = instruction pc */
  VRELOCABLE,	/* info = instruction pc */
  VCALL,	    /* info = instruction pc */
  VVARARG	    /* info = instruction pc */
} expkind;


struct expdesc {
  expkind k;
  int idx;      /* index (R/K) */
  int tr;       /* table (register or upvalue) */
  int vt;       /* whether 't' is register (VLOCAL) or upvalue (VUPVAL) */
  int info;     /* for generic use */
  double nval;  /* for VKNUM */
  int t;        /* patch list of `exit when true' */
  int f;        /* patch list of `exit when false' */
};
