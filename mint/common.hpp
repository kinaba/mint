#ifndef KMONOS_NET_MINT_COMMON_HPP
#define KMONOS_NET_MINT_COMMON_HPP

#include <boost/iterator_adaptors.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/call_traits.hpp>
#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index/detail/index_node_base.hpp>

namespace mint {

template<class A, class B, class C, class D, class E>
struct custom_indirect_iterator : boost::indirect_iterator<A,B,C,D,E>
{
	custom_indirect_iterator(){}
	custom_indirect_iterator(A a):boost::indirect_iterator<A,B,C,D,E>(a){}
	typename A::value_type* get_node() { return &*this->base(); }
};

}      // end of namespace
#endif // include guard
