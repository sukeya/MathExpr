   struct package
   {
      all_true       <T> at;
      all_false      <T> af;
      any_true       <T> nt;
      any_false      <T> nf;
      count          <T>  c;
      copy           <T> cp;
      rol            <T> rl;
      ror            <T> rr;
      reverse        <T> rev;
      shift_left     <T> sl;
      shift_right    <T> sr;
      sort           <T> st;
      nthelement     <T> ne;
      assign         <T> an;
      iota           <T> ia;
      sumk           <T> sk;
      axpy           <T> b1_axpy;
      axpby          <T> b1_axpby;
      axpyz          <T> b1_axpyz;
      axpbyz         <T> b1_axpbyz;
      axpbsy         <T> b1_axpbsy;
      axpbsyz        <T> b1_axpbsyz;
      axpbz          <T> b1_axpbz;
      diff           <T> df;
      dot            <T> dt;
      dotk           <T> dtk;
      threshold_above<T> ta;
      threshold_below<T> tb;
      min_elemwise<T>    miew;
      max_elemwise<T>    maew;
      select<T>          slct;

      bool register_package(math_expr::symbol_table<T>& symtab)
      {
         #define math_expr_register_function(FunctionName, FunctionType)                 \
         if (!symtab.add_function(FunctionName,FunctionType))                         \
         {                                                                            \
            math_expr_debug((                                                            \
              "math_expr::rtl::vecops::register_package - Failed to add function: %s\n", \
              FunctionName));                                                         \
            return false;                                                             \
         }                                                                            \

         math_expr_register_function("all_true"        , at        )
         math_expr_register_function("all_false"       , af        )
         math_expr_register_function("any_true"        , nt        )
         math_expr_register_function("any_false"       , nf        )
         math_expr_register_function("count"           , c         )
         math_expr_register_function("copy"            , cp        )
         math_expr_register_function("rotate_left"     , rl        )
         math_expr_register_function("rol"             , rl        )
         math_expr_register_function("rotate_right"    , rr        )
         math_expr_register_function("ror"             , rr        )
         math_expr_register_function("reverse"         , rev       )
         math_expr_register_function("shftl"           , sl        )
         math_expr_register_function("shftr"           , sr        )
         math_expr_register_function("sort"            , st        )
         math_expr_register_function("nth_element"     , ne        )
         math_expr_register_function("assign"          , an        )
         math_expr_register_function("iota"            , ia        )
         math_expr_register_function("sumk"            , sk        )
         math_expr_register_function("axpy"            , b1_axpy   )
         math_expr_register_function("axpby"           , b1_axpby  )
         math_expr_register_function("axpyz"           , b1_axpyz  )
         math_expr_register_function("axpbyz"          , b1_axpbyz )
         math_expr_register_function("axpbsy"          , b1_axpbsy )
         math_expr_register_function("axpbsyz"         , b1_axpbsyz)
         math_expr_register_function("axpbz"           , b1_axpbz  )
         math_expr_register_function("diff"            , df        )
         math_expr_register_function("dot"             , dt        )
         math_expr_register_function("dotk"            , dtk       )
         math_expr_register_function("threshold_above" , ta        )
         math_expr_register_function("threshold_below" , tb        )
         math_expr_register_function("min_elemwise"    , miew      )
         math_expr_register_function("max_elemwise"    , maew      )
         math_expr_register_function("select"          , slct      )

         #undef math_expr_register_function

         return true;
      }
   };

   } // namespace math_expr::rtl::vecops
   } // namespace math_expr::rtl
}    // namespace math_expr
