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

      bool register_package(exprtk::symbol_table<T>& symtab)
      {
         #define exprtk_register_function(FunctionName, FunctionType)                 \
         if (!symtab.add_function(FunctionName,FunctionType))                         \
         {                                                                            \
            exprtk_debug((                                                            \
              "exprtk::rtl::vecops::register_package - Failed to add function: %s\n", \
              FunctionName));                                                         \
            return false;                                                             \
         }                                                                            \

         exprtk_register_function("all_true"        , at        )
         exprtk_register_function("all_false"       , af        )
         exprtk_register_function("any_true"        , nt        )
         exprtk_register_function("any_false"       , nf        )
         exprtk_register_function("count"           , c         )
         exprtk_register_function("copy"            , cp        )
         exprtk_register_function("rotate_left"     , rl        )
         exprtk_register_function("rol"             , rl        )
         exprtk_register_function("rotate_right"    , rr        )
         exprtk_register_function("ror"             , rr        )
         exprtk_register_function("reverse"         , rev       )
         exprtk_register_function("shftl"           , sl        )
         exprtk_register_function("shftr"           , sr        )
         exprtk_register_function("sort"            , st        )
         exprtk_register_function("nth_element"     , ne        )
         exprtk_register_function("assign"          , an        )
         exprtk_register_function("iota"            , ia        )
         exprtk_register_function("sumk"            , sk        )
         exprtk_register_function("axpy"            , b1_axpy   )
         exprtk_register_function("axpby"           , b1_axpby  )
         exprtk_register_function("axpyz"           , b1_axpyz  )
         exprtk_register_function("axpbyz"          , b1_axpbyz )
         exprtk_register_function("axpbsy"          , b1_axpbsy )
         exprtk_register_function("axpbsyz"         , b1_axpbsyz)
         exprtk_register_function("axpbz"           , b1_axpbz  )
         exprtk_register_function("diff"            , df        )
         exprtk_register_function("dot"             , dt        )
         exprtk_register_function("dotk"            , dtk       )
         exprtk_register_function("threshold_above" , ta        )
         exprtk_register_function("threshold_below" , tb        )
         exprtk_register_function("min_elemwise"    , miew      )
         exprtk_register_function("max_elemwise"    , maew      )
         exprtk_register_function("select"          , slct      )

         #undef exprtk_register_function

         return true;
      }
   };

   } // namespace exprtk::rtl::vecops
   } // namespace exprtk::rtl
}    // namespace exprtk
