#ifndef KMONOS_NET_MINT_SPLAY_HPP
#define KMONOS_NET_MINT_SPLAY_HPP

#include "common.hpp"
#include "node.hpp"
#include "splay_index.hpp"
#include <boost/intrusive/splay_set_hook.hpp>

//---------------------------------------------------------------------------
// MultiIndex 用"インデックス指定子"の実装例
//---------------------------------------------------------------------------

namespace mint {

template< typename TagList = boost::multi_index::tag<> >
struct splay
{
	// multi_index_container を使うときのインデックス指定
	//
	//   multi_index_container<T, indexed_by<ここ, ここ, ここ>>
	//
	// には "インデックス指定子 (index specifier)" と
	// 呼ばれる種類のクラスを渡します。
	// ちなみに、indexed_by はBoost.MPLで使えるシーケンスならなんでもいいです。
	//   multi_index_container<T, mpl::list<ここ, ここ, ここ>>
	// とか。
	//
	//
	// インデックス指定子を自作するには
	//   - 自作ノードの実装を返す        node_class  (詳細は node.hpp で)
	//   - 自作インデックスの実装を返す index_class  (詳細は list_index.hpp で)
	// の二つのメタ関数（メンバクラステンプレート）を定義すればよいです。
	// ↓こんな感じ。

	template<typename Super>
	struct node_class
	{
		typedef detail::intrusive_hook_node<
		            boost::intrusive::splay_set_member_hook<>, Super
		        > type;
	};

	template<typename SuperMeta>
	struct index_class
	{
		typedef detail::splay_index<
		            SuperMeta, typename TagList::type
		        > type;
	};
};

}      // end of namespace
#endif // include guard
