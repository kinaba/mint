#ifndef KMONOS_NET_MINT_LIST_HPP
#define KMONOS_NET_MINT_LIST_HPP

#include "common.hpp"
#include "node.hpp"
#include "list_index.hpp"
#include <boost/intrusive/list_hook.hpp>

namespace mint {

template< typename TagList = boost::multi_index::tag<> >
struct list
{
	template<typename Super>
	struct node_class
	{
		typedef detail::intrusive_hook_node<
		            boost::intrusive::list_member_hook<>, Super
		        > type;
	};

	template<typename SuperMeta>
	struct index_class
	{
		typedef detail::list_index<
		            SuperMeta, typename TagList::type
		        > type;
	};
};

}      // end of namespace
#endif // include guard
