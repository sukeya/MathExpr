   struct package
   {
      print  <T> p;
      println<T> pl;

      bool register_package(exprtk::symbol_table<T>& symtab)
      {
         #define exprtk_register_function(FunctionName, FunctionType)             \
         if (!symtab.add_function(FunctionName,FunctionType))                     \
         {                                                                        \
            exprtk_debug((                                                        \
              "exprtk::rtl::io::register_package - Failed to add function: %s\n", \
              FunctionName));                                                     \
            return false;                                                         \
         }                                                                        \

         exprtk_register_function("print"  , p )
         exprtk_register_function("println", pl)
         #undef exprtk_register_function

         return true;
      }
   };

   } // namespace exprtk::rtl::io
   } // namespace exprtk::rtl
}    // namespace exprtk
