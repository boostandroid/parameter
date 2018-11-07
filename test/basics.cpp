// Copyright David Abrahams, Daniel Wallin 2003.
// Copyright Cromwell D. Enage 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/parameter.hpp>
#include <boost/bind.hpp>
#include "basics.hpp"

namespace test {

    // A separate function for getting the "value" key, so we can deduce F
    // and use lazy_binding on it.
    template <typename Params, typename F>
    typename boost::parameter::lazy_binding<Params,tag::value,F>::type
        extract_value(Params const& p, F const& f)
    {
        typename boost::parameter::lazy_binding<
            Params,test::tag::value,F
        >::type v = p[test::_value || f];
        return v;
    }

    template <typename Params>
    int f_impl(Params const& p)
    {
        typename boost::parameter::binding<Params,test::tag::name>::type
            n = p[test::_name];

        typename boost::parameter::binding<
            Params,test::tag::value,double
        >::type v = test::extract_value(p, boost::bind(&test::value_default));

        typename boost::parameter::binding<Params,test::tag::index,int>::type
            i = p[test::_index | 999];

        p[test::_tester](n, v, i);

        return 1;
    }

    template <typename ...Args>
    int f(Args const&... args)
    {
        return test::f_impl(test::f_parameters()(args...));
    }

    template <typename Params>
    int f_list(Params const& params)
    {
        return test::f_impl(params);
    }
}

#include <boost/ref.hpp>
#include <boost/config/workaround.hpp>
#include <string>

int main()
{
    test::f(
        test::values(
            std::string("foo")
          , std::string("bar")
          , std::string("baz")
        )
      , std::string("foo")
      , std::string("bar")
      , std::string("baz")
    );

    int x = 56;
    test::f(
        test::values(std::string("foo"), 666.222, 56)
      , test::_index = boost::ref(x)
      , test::_name = std::string("foo")
    );

#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
    x = 56;
    test::f_list((
        test::_tester = test::values(std::string("foo"), 666.222, 56)
      , test::_index = boost::ref(x)
      , test::_name = std::string("foo")
    ));
#endif  // No comma operator available on Borland.

#if defined(LIBS_PARAMETER_TEST_COMPILE_FAILURE)
    test::f(test::_index = 56, test::_name = 55); // won't compile
#endif

    return boost::report_errors();
}

