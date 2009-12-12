#ifndef KMONOS_NET_MINT_NODE_HPP
#define KMONOS_NET_MINT_NODE_HPP

//---------------------------------------------------------------------------
// Boost.Intrusive のフックを使った、MultiIndex 用 "ノード" の実装例
//---------------------------------------------------------------------------

namespace mint { namespace detail {

template<typename Hook, typename Super, typename Priority = void>
struct intrusive_hook_node
	: public Super
{
	// ノード型は、例えばインデックス３個指定したコンテナなら
	//
	//   class multi_index_container<T, indexed_by<S1,S2,S3>>
	//   {
	//     typedef S1::node_class<
	//               S2::node_class<
	//                 S3::node_class<
	//                   index_base_node<T>>::type>::type>::type
	//             node_type;
	//
	//     node_type* header;
	//   }
	//
	// こんな風な typedef で定義されます（実際のtypedefは再帰テンプレートで実装）。
	//
	// ノード型を実装する側は node_class に渡される引数 Super を public 継承します。
	//
	//   node_type =
	//      S1::node_class<...>::type
	//        : public S2::node_class<...>::type
	//        : public S3::node_class<...>::type
	//        : public boost::multi_index::detail::index_base_node<T>
	//
	// こんな風な継承関係になります。



	// この継承さえすれば、あとは何でも好きに実装してよさそう。
	// index_base_node に T& value() というメンバがあって、
	// ノードに入っている値が取れるので、あとは煮るなり焼くなり。



	// ただし、Allocator::allocate() で生のメモリとして直接確保されて
	// コンストラクタやデストラクタは呼ばれないので注意。
	//
	// インデックスの insert_() メンバ関数が呼ばれたときに allocate されて、
	// erase_() または delete_node_() がメンバ関数が呼ばれた後に deallocate されるので
	// 必要ならそのタイミングで自前で初期化/終了処理をかけましょう。



	// 以下、Boost.Intrusive を使うための実装詳細

	Hook hook_;                              // Boost.Intrusive のフックを再利用
	void construct_() { new (&hook_) Hook; } // したいので placement new で
	void destruct_()  { hook_.~Hook(); }     // 無理矢理初期化(= コンストラクタ呼出)

	// イテレータを boost::indirect_iterator でお手軽実装するための補助関数
	const typename Super::value_type& operator*() const { return this->value(); }

	// boost::intrusive::xxx_set に突っ込むための比較関数オブジェクト
	template<typename Comp>
	struct comparator
	{
		Comp cmp;
		comparator(const Comp& cmp = Comp()) : cmp(cmp) {}

		bool operator()(const intrusive_hook_node& lhs, const intrusive_hook_node& rhs) const
		{
			return cmp(lhs.value(), rhs.value());
		}

		bool operator()(const intrusive_hook_node& lhs, const typename Super::value_type& v) const
		{
			return cmp(lhs.value(), v);
		}

		bool operator()(const typename Super::value_type& v, const intrusive_hook_node& rhs) const
		{
			return cmp(v, rhs.value());
		}
	};

	// treap 用
	friend bool priority_order(const intrusive_hook_node& a, const intrusive_hook_node& b)
	{
		return Priority()(a.value(), b.value());
	}
};




}}     // end of namespace
#endif // include guard
