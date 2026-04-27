   struct package
   {
      open   <T> o;
      close  <T> c;
      write  <T> w;
      read   <T> r;
      getline<T> g;
      eof    <T> e;

      bool register_package(exprtk::symbol_table<T>& symtab)
      {
         #define exprtk_register_function(FunctionName, FunctionType)                   \
         if (!symtab.add_function(FunctionName,FunctionType))                           \
         {                                                                              \
            exprtk_debug((                                                              \
              "exprtk::rtl::io::file::register_package - Failed to add function: %s\n", \
              FunctionName));                                                           \
            return false;                                                               \
         }                                                                              \

         exprtk_register_function("open"    , o)
         exprtk_register_function("close"   , c)
         exprtk_register_function("write"   , w)
         exprtk_register_function("read"    , r)
         exprtk_register_function("getline" , g)
         exprtk_register_function("eof"     , e)
         #undef exprtk_register_function

         return true;
      }
   };

   } // namespace exprtk::rtl::io::file
   } // namespace exprtk::rtl::io
   } // namespace exprtk::rtl
}    // namespace exprtk
