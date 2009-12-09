#ifndef KMONOS_NET_MINT_SPLAY_INDEX_HPP
#define KMONOS_NET_MINT_SPLAY_INDEX_HPP

#include "common.hpp"
#include "node.hpp"
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/splay_set_hook.hpp>

//---------------------------------------------------------------------------
// MultiIndex 用 "インデックス" の実装例
//---------------------------------------------------------------------------

namespace mint { namespace detail {

template<typename SuperMeta, typename TagList>
class splay_index
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
	//   [Impl]     は、今回の splay_set 用インデックスのための実装詳細
	//   [Custom]   は、実装したいインデックスに合わせて色々変えて使う関数/型定義
	//   [Required] は、他のインデックスからそのままコピペで使える関数/型定義
	//                  本当はちゃんとライブラリ化した方がいい

private:
	// [Impl] SuperMeta::type は頻繁に使うので typedef しておきます。
	typedef splay_index              self;
	typedef typename SuperMeta::type super;

protected:
	// [Custom]
	//    このインデックスで使うノード型を node_type と typedef
	typedef intrusive_hook_node<
		boost::intrusive::splay_set_member_hook<>, typename super::node_type
	> node_type;

public:
	// [Required] コンテナとして使うために必要な型定義
	typedef typename node_type::value_type                   value_type;
	typedef typename super::final_allocator_type             allocator_type;
	typedef typename allocator_type::reference               reference;
	typedef typename allocator_type::const_reference         const_reference;

private:
	// [Impl]
	//    intrusive の splay_set を使った内部実装

	typedef boost::intrusive::splay_set<node_type,
		boost::intrusive::member_hook<node_type,
			boost::intrusive::splay_set_member_hook<>, &node_type::hook_>
	> impl_type;

	impl_type impl;

	typedef typename impl_type::iterator internal_iterator;

	// [Impl]
	//     node_type や value_type から Boost.Intrusive データ構造のイテレータを取得する関数
	//     node_type からイテレータへの変換は Boost.Intrusive の機能
	//     value_type への参照から、それが入ってるノードのアドレスを得るのは MultiIndex の機能
	internal_iterator internal_iterator_to( node_type* p ) { return impl_type::s_iterator_to(*p); }
	internal_iterator internal_iterator_to( node_type& x ) { return impl_type::s_iterator_to(x); }
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
	//
	//     注意点として、std::set 等と同じく、常に const 参照を返す（変更不可の）
	//     イテレータだけを作ります。値を変更するときは二分木のバランスを取ったり
	//     しないといけないかもしれないので、専用の関数 modify() や replace() を
	//     呼んでもらいます。
	typedef boost::indirect_iterator<
		internal_iterator,
		value_type, boost::use_default, const value_type&, ptrdiff_t
	> iterator;

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
	splay_index( const splay_index& rhs ) : super(rhs) {} // あとで copy_ が呼ばれる
	splay_index( const ctor_args_list& args_list, const allocator_type& al )
		: super( args_list.get_tail(), al ) {}
	~splay_index() {}
	splay_index& operator=( splay_index& rhs ) { this->final() = rhs.final(); return *this; }
	allocator_type get_allocator() const { return this->final().get_allocator(); }

public:
	// [Custom]
	//    イテレータを作る系メソッド
	iterator               begin()         {return iterator(impl.begin()); }
	const_iterator         begin()   const {return const_iterator(const_cast<impl_type&>(impl).begin()); }
	iterator               end()           {return iterator(impl.end());}
	const_iterator         end()     const {return const_iterator(const_cast<impl_type&>(impl).end());}

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
	//         void erase_(node_type* p) { ... super::erase_(p); }
	//         void final_erase_(node_type* p) {
	//            ((multi_index_container<T,indexed_by<S1,MyIndex,S2>>*)this)->erase_(p);
	//         }
	//      }
	//
	//------------------------------------------------------------------------------------

	void copy_( const splay_index& x, const copy_map_type& map )
	{
		// [Custom]
		for(const_iterator from=x.begin(); from!=x.end(); ++from)
		{
			node_type* p = map.find(static_cast<final_node_type*>(&*from.base()));
			p->construct_();
			impl.push_back(*p);
		}

		super::copy_(x, map);
	}

	node_type* insert_( const value_type& v, node_type* x )
	{
		node_type* res = static_cast<node_type*>(super::insert_(v,x));
		// [Custom]
		//   - v に値、x に新しく用意されたノード
		//   1. ordered_index等で既にinsert済みだったらinsert済みのノードを返す
		//   2. そうでなければ、親を呼ぶ
		//   3. 帰ってきたのがxそのものなら、自分のリンクに繋ぐ
		if(res==x) {x->construct_(); impl.push_back(*x);}
		return res;
	}

	node_type* insert_( const value_type& v, node_type* position, node_type* x )
	{
		node_type* res = static_cast<node_type*>(super::insert_(v,position,x));
		// [Custom]
		//   - v に値、x に新しく用意されたノード、position はただのヒント
		//   1. ordered_index等で既にinsert済みだったらinsert済みのノードを返す
		//   2. そうでなければ、親を呼ぶ
		//   3. 帰ってきたのがxそのものなら、自分のリンクに繋ぐ
		if(res==x) {x->construct_(); impl.push_back(*x);}
		return res;
	}

	void erase_(node_type* x)
	{
		// [Custom]
		impl.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();
		super::erase_(x);
	}

	void delete_node_(node_type* x)
	{
		// たぶんdelete_all_nodes_のときだけ呼ばれる
		// [Custom]
		impl.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();
		super::delete_node_(x);
	}

	void delete_all_nodes_()
	{
		// [Custom]
		// そうか index_base はこれをできないからどれかがやってやらないといけない
		for(internal_iterator it=impl.begin(); it!=impl.end(); )
		{
			internal_iterator jt = it++;
			impl.erase(jt);
			this->final_delete_node_( static_cast<final_node_type*>(&*jt) );
		}
	}

	void clear_()
	{
		// [Custom]
		super::clear_();
		impl.clear();
	}

	void swap_( splay_index& x )
	{
		// [Custom]
		impl.swap(x.impl);
		super::swap_(x);
	}

	bool replace_( const value_type& v, node_type* x )
	{
		return super::replace_(v, x);
	}

	bool modify_(node_type* x)
	{
		try {
			if( !super::modify_(x) ){
				impl.erase( internal_iterator_to(*x) );
				return false;
			}
		} catch( ... ) {
			impl.erase( internal_iterator_to(*x) );
			throw;
		}
		return true;
	}

	bool modify_rollback_( node_type* x )
	{
		return super::modify_rollback_(x);
	}

public:
	//------------------------------------------------------------------------------------
	// [Custom]
	//    ここから、MutliIndex コンテナの操作のための内部プリミティブの実装
	//------------------------------------------------------------------------------------
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

	// コンテナの実装 (1)
	bool      empty()    const {return this->final_empty_();}
	size_type size()     const {return this->final_size_();}
	size_type max_size() const {return this->final_max_size_();}
	void      swap(splay_index& x)    {this->final_swap_(x.final());}
	void      clear()          {this->final_clear_();}
	const_reference front() const {return *begin();}
	const_reference back()  const {return *--end();}

	// コンテナの実装 (2)
	std::pair<iterator,bool> push_front(const value_type& x) {return insert(begin(),x);}
	void                     pop_front(){erase(begin());}
	std::pair<iterator,bool> push_back(const value_type& x)  {return insert(end(),x);}
	void                     pop_back() {erase(--end());}

	// コンテナの実装 (3)
	template<typename InputIterator>
	void insert(iterator position,InputIterator first,InputIterator last)
	{
		// 本来は first, last が value, num の場合と区別する場合に頑張るべきだが知らん
		for(; first!=last; ++first)
			insert(position, *first);
	}
	void insert(iterator position, size_type n, const value_type& x)
	{
		for(size_type i=0; i<n; ++i)
			insert(position, x);
	}
	iterator erase(iterator position)
	{
		this->final_erase_(static_cast<final_node_type*>( (position++).base() ));
		return position;
	}
	iterator erase(iterator first,iterator last)
	{
		while( first != last )
			first=erase(first);
		return first;
	}
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

	// コンテナの実装 (4)
	std::pair<iterator,bool> insert(iterator position, const value_type& x)
	{
		std::pair<final_node_type*,bool> p = this->final_insert_(x);
		// とりあえず末尾に挿入されてるのでpositionに動かす
		if( p.second )
			impl.splice( position.base(), impl, internal_iterator_to(*p.first) );
		return std::pair<iterator,bool>(internal_iterator_to(*p.first),p.second);
	}
/*
  void resize(size_type n,const value_type& x=value_type())
  {
    BOOST_MULTI_INDEX_SEQ_INDEX_CHECK_INVARIANT;
    if(n>size())insert(end(),n-size(),x);
    else if(n<size()){
      iterator it;
      if(n<=size()/2){
        it=begin();
        std::advance(it,n);
      }
      else{
        it=end();
        for(size_type m=size()-n;m--;--it){}
      }
      erase(it,end());
    }   
  }
*/
};



}}     // end of namespace
#endif // include guard
