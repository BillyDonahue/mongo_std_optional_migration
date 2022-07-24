#include <boost/optional.hpp>

struct X { void get() const; };

boost::optional<int> oi;
auto oip = &oi;
X x;
X* xp = &x;

void foo() {
  x.get();  // unchanged
  xp->get();  // unchanged

  oi.get();  // change to oi.value()
  oi.is_initialized();  // change to oi.has_value()

  oip->get();  // change to oip->value()
  oip->is_initialized();  // change to oip->has_value()
}

#if 0
Binding for "root":
|-CXXMemberCallExpr 0x7fbb988b5980 <line:10:3, col:10> 'const int':'const int' lvalue
| `-MemberExpr 0x7fbb988b5950 <col:3, col:6> '<bound member function type>' .get 0x7fbb988afff8
|   `-DeclRefExpr 0x7fbb988b58d0 <col:3> 'const boost::optional<int>':'const boost::optional<int>' lvalue ParmVar 0x7fbb980b83d0 'oi' 'const boost::optional<int> &'
|-CXXMemberCallExpr 0x7fbb988b5c40 <line:11:3, col:21> 'bool'
| `-MemberExpr 0x7fbb988b5c10 <col:3, col:6> '<bound member function type>' .is_initialized 0x7fbb980bf098
|   `-ImplicitCastExpr 0x7fbb988b5c88 <col:3> 'const boost::optional_detail::tc_optional_base<int>' lvalue <UncheckedDerivedToBase (tc_optional_base)>
|     `-DeclRefExpr 0x7fbb988b5bf0 <col:3> 'const boost::optional<int>':'const boost::optional<int>' lvalue ParmVar 0x7fbb980b83d0 'oi' 'const boost::optional<int> &'
|-CXXMemberCallExpr 0x7fbb988b5d70 <line:13:3, col:12> 'const int':'const int' lvalue
| `-MemberExpr 0x7fbb988b5d40 <col:3, col:8> '<bound member function type>' ->get 0x7fbb988afff8
|   `-ImplicitCastExpr 0x7fbb988b5cc8 <col:3> 'const boost::optional<int> *':'const boost::optional<int> *' <LValueToRValue>
|     `-DeclRefExpr 0x7fbb988b5ca8 <col:3> 'const boost::optional<int> *':'const boost::optional<int> *' lvalue Var 0x7fbb980b85b0 'oip' 'const boost::optional<int> *':'const boost::optional<int> *'
`-CXXMemberCallExpr 0x7fbb988b5df8 <line:14:3, col:23> 'bool'
  `-MemberExpr 0x7fbb988b5dc8 <col:3, col:8> '<bound member function type>' ->is_initialized 0x7fbb980bf098
    `-ImplicitCastExpr 0x7fbb988b5e18 <col:3> 'const boost::optional_detail::tc_optional_base<int> *' <UncheckedDerivedToBase (tc_optional_base)>
      `-ImplicitCastExpr 0x7fbb988b5db0 <col:3> 'const boost::optional<int> *':'const boost::optional<int> *' <LValueToRValue>
        `-DeclRefExpr 0x7fbb988b5d90 <col:3> 'const boost::optional<int> *':'const boost::optional<int> *' lvalue Var 0x7fbb980b85b0 'oip' 'const boost::optional<int> *':'const boost::optional<int> *'
#endif
