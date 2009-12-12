#ifndef KMONOS_NET_MINT_AVL_INDEX_HPP
#define KMONOS_NET_MINT_AVL_INDEX_HPP

#include "common.hpp"
#include "node.hpp"
#include <algorithm>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/avl_set_hook.hpp>

//---------------------------------------------------------------------------
// MultiIndex 用 "インデックス" の実装例
//---------------------------------------------------------------------------

namespace mint { namespace detail {

template<typename Compare, typename SuperMeta, typename TagList>
class avl_index
	: protected SuperMeta::type
{
	// 例えばインデックス３個指定したコンテナなら
	//
	//   class multi_index_container<T, indexed_by<S1,S2,S3>>
	//      : public S1::index_class<SuperMeta1>::type
	//   {
	//      S1::index_class<SuperMeta1>::type& get<0>() { return *this; }
	//      S2::index_class<SuperMeta2>::type& get<1>() { return *this; }
	//      S3::index_class<SuperMeta3>::type& get<2>() { return *this; }
	//   }
	//
	// こんな風にインデックスが使われます。
	// SuperMeta には何かマジカルに必要な情報が色々入ってますが、重要なのは
	//
	//   SuperMeta::type
	//
	// だけです。インデックスを実装するときはこの型を protected 継承します。
	// こんな風な継承関係になります。
	//
	//   multi_index_container<...>
	//     : public S1::index_class<...>::type 
	//     : protected S2::index_class<...>::type 
	//     : protected S3::index_class<...>::type 
	//     : protected boost::multi_index::detail::index_base<...>
	//


	// 以下のコメントで
	//   [Impl]     は、今回の avl_set 用インデックスのための実装詳細
	//   [Custom]   は、実装したいインデックスに合わせて色々変えて使う関数/型定義
	//   [Required] は、他のインデックスからそのままコピペで使える関数/型定義
	//                  本当はちゃんとライブラリ化した方がいい

private:
	// [Impl] SuperMeta::type は頻繁に使うので typedef しておきます。
	typedef avl_index              self;
	typedef typename SuperMeta::type super;

protected:
	// [Custom]
	//    このインデックスで使うノード型を node_type と typedef
	typedef intrusive_hook_node<
		boost::intrusive::avl_set_member_hook<>, typename super::node_type
	> node_type;

public:
	// [Required] コンテナとして使うために必要な型定義
	typedef typename node_type::value_type                   value_type;
	typedef typename super::final_allocator_type             allocator_type;
	typedef typename allocator_type::reference               reference;
	typedef typename allocator_type::const_reference         const_reference;

private:
	// [Impl]
	//    intrusive の avl_set を使った内部実装

	typedef boost::intrusive::avl_set<node_type,
		boost::intrusive::member_hook<node_type,
			boost::intrusive::avl_set_member_hook<>, &node_type::hook_>,
		boost::intrusive::compare< typename node_type::template comparator<Compare> >
	> impl_type;

	// splay は中で勝手に動くのでconstだとfindができない…
	mutable impl_type impl_;

	typedef typename impl_type::iterator internal_iterator;

	// [Impl]
	//     node_type や value_type から Boost.Intrusive データ構造のイテレータを取得する関数
	//     node_type からイテレータへの変換は Boost.Intrusive の機能
	//     value_type への参照から、それが入ってるノードのアドレスを得るのは MultiIndex の機能
	internal_iterator internal_iterator_to( node_type& x ) { return impl_type::s_iterator_to(x); }
	internal_iterator internal_iterator_to( node_type* p ) { return impl_type::s_iterator_to(*p); }
	internal_iterator internal_iterator_to( const value_type& x )
		{ return internal_iterator_to(boost::multi_index::detail::node_from_value<node_type>(&x)); }

public:
	// [Custom]
	//     ここをコンストラクタに必要な引数リストに置き換える
	//     今回はゼロ引数でいいので null_type で
	typedef boost::tuples::null_type ctor_args;

	// [Custom]
	//     イテレータの定義
	//     お手軽実装ということで Boost.Iterators ライブラリでサクッと作ります
	//     (ただしget_node メンバ関数を追加: see common.hpp)
	//
	//     注意点として、std::set 等と同じく、常に const 参照を返す（変更不可の）
	//     イテレータだけを作ります。値を変更するときは二分木のバランスを取ったり
	//     しないといけないかもしれないので、専用の関数 modify() や replace() を
	//     呼んでもらいます。
	typedef custom_indirect_iterator<
		internal_iterator,
		value_type, boost::use_default, const value_type&, ptrdiff_t> iterator;

	// [Required] コンテナとして使うために必要な型定義
	typedef iterator                                         const_iterator;
	typedef std::size_t                                      size_type;      
	typedef std::ptrdiff_t                                   difference_type;
	typedef typename allocator_type::pointer                 pointer;
	typedef typename allocator_type::const_pointer           const_pointer;
	typedef typename boost::reverse_iterator<iterator>       reverse_iterator;
	typedef typename boost::reverse_iterator<const_iterator> const_reverse_iterator;

	// [Required] multi_index が使う型定義
	typedef TagList                                          tag_list;

protected:
	// [Required] multi_index が使う型定義
	typedef typename super::final_node_type                             final_node_type;
	typedef boost::tuples::cons<ctor_args,
		typename super::ctor_args_list>                                 ctor_args_list;
	typedef typename boost::mpl::push_front<
		typename super::index_type_list, self>::type                    index_type_list;
	typedef typename boost::mpl::push_front<
		typename super::iterator_type_list, iterator>::type             iterator_type_list;
	typedef typename boost::mpl::push_front<
		typename super::const_iterator_type_list, const_iterator>::type const_iterator_type_list;
	typedef typename super::copy_map_type                               copy_map_type;

public:
	// [Custom]
	//   コンストラクタとデストラクタとコピー等
	avl_index( const avl_index& rhs ) : super(rhs) {} // あとで copy_ が呼ばれる
	avl_index( const ctor_args_list& args_list, const allocator_type& al )
		: super( args_list.get_tail(), al ) {}
	~avl_index() {}
	self& operator=( self& rhs ) { this->final() = rhs.final(); return *this; }
	allocator_type get_allocator() const { return this->final().get_allocator(); }

public:
	// [Custom]
	//    イテレータを作る系メソッド
	iterator               begin()         {return iterator(impl_.begin()); }
	const_iterator         begin()   const {return const_iterator(const_cast<impl_type&>(impl_).begin()); }
	iterator               end()           {return iterator(impl_.end());}
	const_iterator         end()     const {return const_iterator(const_cast<impl_type&>(impl_).end());}

	// [Required]
	reverse_iterator       rbegin()        {return boost::make_reverse_iterator(end());}
	const_reverse_iterator rbegin()  const {return boost::make_reverse_iterator(end());}
	reverse_iterator       rend()          {return boost::make_reverse_iterator(begin());}
	const_reverse_iterator rend()    const {return boost::make_reverse_iterator(begin());}
	const_iterator         cbegin()  const {return begin();}
	const_iterator         cend()    const {return end();}
	const_reverse_iterator crbegin() const {return rbegin();}
	const_reverse_iterator crend()   const {return rend();}

	// [Custom]
	//   格納されている値への参照から逆算して、それを指すイテレータを作れる、
	//   というのは普通の標準コンテナにはない機能だったりします。
	iterator       iterator_to(const value_type& x)       {return iterator(internal_iterator_to(x));}
	const_iterator iterator_to(const value_type& x) const {return const_iterator(internal_iterator_to(x));}

public:
	//------------------------------------------------------------------------------------
	// [Custom]
	//    ここから、MutliIndex コンテナの操作のための内部プリミティブの実装。
	//    ユーザーに使ってもらうためのAPI関数 (push_back とか) は、
	//    この辺のプリミティブを使って実装する。
	//
	//    pop_back() の実装を例として説明します。使うプリミティブは erase_()。
	//    multi_index_container<T,indexed_by<S1,MyIndex,S2> という例で考えると
	//
	//      class multi_index_container<T,indexed_by<S1,MyIndex,S2>>
	//         : public S1::index_class<...>::type
	//      {
	//         void erase_(node_type* p) { ... super::erase_(p); }
	//      }
	//
	//      class S1::index_class<...>::type : protected MyIndex::index_class<...>::type
	//      {
	//         void erase_(node_type* p) { ... super::erase_(p); }
	//      }
	//
	//      class MyIndex::index_class<...>::type : protected S3::index_class<...>::type
	//      {
	//         void erase_(node_type* p) { ... super::erase_(p); }
	//         void pop_back() { p=end(); final_erase_(p); }
	//      }
	//
	//      class S3::index_class<...>::type : protected index_base
	//      {
	//         void erase_(node_type* p) { ... super::erase_(p); }
	//      }
	//
	//      class index_base
	//      {
	//         void erase_(node_type* p) { deallocate(p); }
	//         void final_erase_(node_type* p) {
	//            ((multi_index_container<T,indexed_by<S1,MyIndex,S2>>*)this)->erase_(p);
	//         }
	//      }
	//
	//    こう、
	//      - 最後を指すノードを（自分のインデックス情報を利用して）見つける
	//      - final_erase_() を呼ぶ
	//      - final_erase_() は全部のインデックスの erase_() を呼んで回る
	//      - 各インデックスは erase_() が呼ばれたら自分のところのリンクを外す
	//      - index_base::erase_ がノードのメモリを解放
	//    こんな流れになってます。要は
	//      * インデックスとして提供したいメソッドを final_xxx_ を使って実装する
	//	    * 他のインデックス(含む自分)の変更操作を反映するための内部関数 xxx_ を作っておく
	//    となります。
	//------------------------------------------------------------------------------------

	void copy_( const self& x, const copy_map_type& map )
	{
		// [Custom]
		//   x のすべてのノードをコピーする
		//
		//   map.find(...) は、そのノードが既に他のインデックスでコピーされてたら
		//   そのノードを、されてなければ新しく割り当てたノードを返す。
		//   その帰ってきたノードを適宜こちらのインデックスで繋ぎ直せばよい。

		for(const_iterator from=x.begin(); from!=x.end(); ++from)
		{
			node_type* p = map.find(static_cast<final_node_type*>(&*from.base()));
			p->construct_();
			impl_.insert(*p);
		}

		//   最後に、親のインデックスでもリンク構造のコピーをしないといけないので
		//   忘れず super::copy_()

		super::copy_(x, map);
	}

	node_type* insert_( const value_type& v, node_type* x )
	{
		// [Custom]
		//   値 v の入るノード x を新しく挿入する。
		//   デフォルトの挿入位置（set系なら順序的に考えて正しい位置、list系なら末尾等）
		//   に突っ込めばよい。
		//
		//   手順は、
		//     1. まず、自分のインデックス的に考えて挿入可能かどうか判断
		//        setの重複判定など。
		//        不可能だったら重複してる元ノードを返す
		//        元ノードとかそういうのがなければ多分 null を返すのかな←調べてない

		internal_iterator it = impl_.find(v, impl_.value_comp());
		if( it != impl_.end() )
			return &*it;

		//     2. 自分に挿入可能なら、まずは落ち着いて親を呼ぶ

		node_type* res = static_cast<node_type*>(super::insert_(v,x));

		//     3. 親およびご先祖様インデックス全部で挿入成功してたら、
		//        そこではじめて自分のリンクに繋ぐ

		if( res == x ) {
			x->construct_(); // xは新しいノードなはずなので初期化
			impl_.insert(*x);
		}
		return res;
	}

	node_type* insert_( const value_type& v, node_type* position, node_type* x )
	{
		// [Custom]
		//    めんどくさいのでヒントはガン無視
		//    あとでちゃんと書く

		return insert_( v, x );
	}

	void erase_(node_type* x)
	{
		// [Custom]
		//    xを削除する。つまり自分のリンクから外す
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();

		// 親のリンクからもはずす
		super::erase_(x);
	}

	void delete_node_(node_type* x)
	{
		// [Custom]
		//    たぶんdelete_all_nodes_のときだけ呼ばれる
		//    ので全消えがわかってるのでたいていのindex実装はなにもしてない？
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();

		// 親
		super::delete_node_(x);
	}

	void delete_all_nodes_()
	{
		// [Custom]
		//    index_base はこれをできないからどれかがやってやらないといけない
		//    全部のノードを消して回る
		for(internal_iterator it=impl_.begin(); it!=impl_.end(); )
		{
			internal_iterator jt = it++;
			impl_.erase(jt);
			this->final_delete_node_( static_cast<final_node_type*>(&*jt) );
		}
	}

	void clear_()
	{
		// [Custom]
		//   くりあー
		impl_.clear();
		super::clear_();
	}

	void swap_( avl_index& x )
	{
		// [Custom]
		//   スワップ
		impl_.swap(x.impl_);
		super::swap_(x);
	}

	bool replace_( const value_type& v, node_type* x )
	{
		// [Custom]
		//   ノード x に入ってる値を v に変更する
		//
		//   手順は、
		//     1. まず、自分のインデックス的に考えて変更できるかどうか判断
		//        setの重複判定など。
		//        変更しちゃったらマズいなら false を返す

		internal_iterator it = impl_.find(v, impl_.value_comp());
		if( it != impl_.end() )
			return false;

		//     2. 自分に挿入可能なら、親にもお伺いを立てる

		if( !super::replace_(v, x) )
			return false;

		//     3. 親を一番上まで登り切ったらご先祖様が x に実際に v を
		//        入れておいてくれるので、後は自分用リンクを繋ぎ直す

		impl_.erase(*x);
		impl_.insert(*x);
		return true;
	}

	bool modify_rollback_( node_type* x )
	{
		// [Custom]
		//   ノード x に入ってる値がすでに変わってるので
		//   必要ならなんとか正しい位置に動かしてくださいというメソッド

		// Intrusive で綺麗な実装を思いつかないのでとりあえず保留・・・
		return false;


		// 実装手順は、replace_ と同じ
		//   1. 自分にとってOKか確認
		//   2. 親にもお伺いを立てる
		//   3. 親が全員OKならリンク繋ぎ直し
		// falseを返すと変更がロールバックされるので、
		// x がマズい変わり方をしていても単にfalseを返しておけば安全
	}

	bool modify_(node_type* x)
	{
		// [Custom]
		//   ノード x に入ってる値がすでに変わってるので
		//   必要ならなんとか正しい位置に動かしてくださいというメソッド
		//   modify_rollback_ との違いは、falseを返すとロールバックできずに
		//   ノード x が削除されること。
		
		// 保留
		return false;

		// 実装手順は、modify_rollback_ に加えて、false を返したり例外で抜けたりする場合は
		// erase_ 相当の手順で x のリンクを外しておかないといけない。
		//   try {
		//     if( 自分がダメ || !super::modify_(x) ){
		//       impl_.erase( internal_iterator_to(*x) );
		//       return false;
		//     }
		//   } catch( ... ) {
		//     impl_.erase( internal_iterator_to(*x) );
		//     throw;
		//   }
		//   return true;
		// こんな順番になる
	}


public:
	//------------------------------------------------------------------------------------
	// [Custom]
	//    ここから、インデックスの提供するいろいろなメンバ関数の実装
	//------------------------------------------------------------------------------------

	void  swap(avl_index& x)    {this->final_swap_(x.final());}
	void  clear()                 {this->final_clear_();}
	const_reference front() const {return *begin();}
	const_reference back()  const {return *--end();}

	bool      empty()    const {return this->final_empty_();}
	size_type size()     const {return this->final_size_();}
	size_type max_size() const {return this->final_max_size_();}


/*
  template <class InputIterator>
  void assign(InputIterator first,InputIterator last)
  {
    assign_iter(first,last,mpl::not_<is_integral<InputIterator> >());
  }

  void assign(size_type n,const value_type& value)
  {
    BOOST_MULTI_INDEX_SEQ_INDEX_CHECK_INVARIANT;
    clear();
    for(size_type i=0;i<n;++i)push_back(value);
  }    
*/

	// insert
	template<typename InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	{
		for(; first!=last; ++first)
			insert(position, *first);
	}
	void insert(iterator position, size_type n, const value_type& x)
	{
		for(size_type i=0; i<n; ++i)
			insert(position, x);
	}
	std::pair<iterator,bool> insert(iterator position, const value_type& x)
	{
		return insert(x); // tenuki, ignoring the hint
	}
	std::pair<iterator,bool> insert(const value_type& x)
	{
		std::pair<final_node_type*,bool> p = this->final_insert_(x);
		return std::pair<iterator,bool>(internal_iterator_to(&*p.first),p.second);
	}

	// erase
	iterator erase(iterator position)
	{
		this->final_erase_(static_cast<final_node_type*>( &*(position++).base() ));
		return position;
	}
	iterator erase(const value_type& v)
	{
		internal_iterator it = impl_.find(v, impl_.value_comp());
		if( it == impl_.end() )
			return end();
		return erase( it );
	}
	iterator erase(iterator first,iterator last)
	{
		while( first != last )
			first=erase(first);
		return first;
	}

	// modify
	bool replace(iterator position,const value_type& x)
	{
		return this->final_replace_(x,static_cast<final_node_type*>(position.base()));
	}
	template<typename Modifier>
	bool modify(iterator position,Modifier mod)
	{
    	return this->final_modify_(mod,static_cast<final_node_type*>(position.base()));
	}
	template<typename Modifier,typename Rollback>
	bool modify(iterator position,Modifier mod,Rollback back)
	{
		return this->final_modify_(mod,back,static_cast<final_node_type*>(position.get_node()));
	}


	// find
	size_t count(const value_type& x) const
	{
		return impl_.count(x, impl_.value_comp());
	}
	iterator find(const value_type& x) const
	{
		return impl_.find(x, impl_.value_comp());
	}
	iterator lower_bound(const value_type& x) const
	{
		return impl_.lower_bound(x, impl_.value_comp());
	}
	iterator upper_bound(const value_type& x) const
	{
		return impl_.upper_bound(x, impl_.value_comp());
	}
	pair<iterator, iterator> equal_range(const value_type& x) const
	{
		return range(x,x);
	}
	pair<iterator, iterator> range(const value_type& x, const value_type& y) const
	{
		return make_pair(lower_bound(x), upper_bound(y));
	}

	bool operator==( const self& rhs ) const
	{
		return this->size() == rhs.size()
		    && std::equal(begin(), end(), rhs.begin());
	}
	bool operator<( const self& rhs ) const
	{
		return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
	}
	bool operator!=( const self& rhs ) const
		{ return !(*this == rhs); }
	bool operator>( const self& rhs ) const
		{ return rhs < *this; }
	bool operator<=( const self& rhs ) const
		{ return !(*this>rhs); }
	bool operator>=( const self& rhs ) const
		{ return !(*this<rhs); }
};

template<typename C, typename S, typename T>
void swap( avl_index<C,S,T>& lhs, avl_index<C,S,T>& rhs )
{
	lhs.swap(rhs);
}


}}     // end of namespace
#endif // include guard
