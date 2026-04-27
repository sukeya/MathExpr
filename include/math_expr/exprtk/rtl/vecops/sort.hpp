   class sort exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      sort()
      : exprtk::igeneric_function<T>("V|VTT|VS|VSTT")
        /*
           Overloads:
           0. V    - vector
           1. VTT  - vector, r0, r1
           2. VS   - vector, string
           3. VSTT - vector, string, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         vector_t vec(parameters[0]);

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0))
            return T(0);
         if ((3 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
            return T(0);

         bool ascending = true;

         if ((2 == ps_index) || (3 == ps_index))
         {
            if (exprtk::details::imatch(to_str(string_t(parameters[1])),"ascending"))
               ascending = true;
            else if (exprtk::details::imatch(to_str(string_t(parameters[1])),"descending"))
               ascending = false;
            else
               return T(0);
         }

         if (ascending)
            std::sort(
               vec.begin() + r0,
               vec.begin() + r1 + 1,
               std::less<T>());
         else
            std::sort(
               vec.begin() + r0,
               vec.begin() + r1 + 1,
               std::greater<T>());

         return T(1);
      }
   };

   template <typename T>
