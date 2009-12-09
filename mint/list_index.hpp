#ifndef KMONOS_NET_MINT_LIST_INDEX_HPP
#define KMONOS_NET_MINT_LIST_INDEX_HPP

#include "common.hpp"
#include "node.hpp"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>

namespace mint { namespace detail {





template<typename SuperMeta, typename TagList>
class list_index : protected SuperMeta::type
{
private:
	typedef typename SuperMeta::type super;

protected:
	// [Custom] ここを自分のnode_typeに置き換えよう！
	typedef intrusive_hook_node<boost::intrusive::list_member_hook<>,typename super::node_type> node_type;

private:
	// 内部実装
	typedef boost::intrusive::list<node_type,
		boost::intrusive::member_hook<node_type, boost::intrusive::list_member_hook<>, &node_type::hook_>
	> LIST;

	LIST lst;

	typedef typename LIST::iterator internal_iterator;


public:
	// [Custom] ここをコンストラクタに必要な引数リストに置き換えよう！
	typedef boost::tuples::null_type ctor_args;

	// コンテナとして使うために必要な型定義
	typedef typename node_type::value_type                   value_type;
	typedef typename super::final_allocator_type             allocator_type;
	typedef typename allocator_type::reference               reference;
	typedef typename allocator_type::const_reference         const_reference;

	// イテレータの定義
	typedef boost::indirect_iterator<
		internal_iterator,
		value_type, boost::use_default, const value_type&, ptrdiff_t> iterator;

	// コンテナとして使うために必要な型定義
	typedef iterator                                         const_iterator;
	typedef std::size_t                                      size_type;      
	typedef std::ptrdiff_t                                   difference_type;
	typedef typename allocator_type::pointer                 pointer;
	typedef typename allocator_type::const_pointer           const_pointer;
	typedef typename boost::reverse_iterator<iterator>       reverse_iterator;
	typedef typename boost::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef TagList                                          tag_list;

protected:
	// multi_index で必要な型定義
	typedef typename super::final_node_type                             final_node_type;
	typedef boost::tuples::cons<ctor_args,
		typename super::ctor_args_list>                                 ctor_args_list;
	typedef typename boost::mpl::push_front<
		typename super::index_type_list, list_index>::type                    index_type_list;
	typedef typename boost::mpl::push_front<
		typename super::iterator_type_list, iterator>::type             iterator_type_list;
	typedef typename boost::mpl::push_front<
		typename super::const_iterator_type_list, const_iterator>::type const_iterator_type_list;
	typedef typename super::copy_map_type                               copy_map_type;

private:
	typedef typename boost::call_traits<value_type>::param_type value_param_type;
	internal_iterator internal_iterator_to( node_type& x )
	{
		return LIST::s_iterator_to(x);
	}

public:
	// [Custom] コンストラクタとデストラクタ
	list_index(const list_index& x) : super(x) {}
	list_index(const ctor_args_list& args_list,const allocator_type& al)
		: super(args_list.get_tail(), al) {}
	~list_index() {}

public:
	// イテレータを作る系メソッド
	iterator               begin()       {return iterator(lst.begin()); }
	const_iterator         begin()  const{return const_iterator(const_cast<LIST&>(lst).begin()); }
	iterator               end()         {return iterator(lst.end());}
	const_iterator         end()    const{return const_iterator(const_cast<LIST&>(lst).end());}
	reverse_iterator       rbegin()      {return boost::make_reverse_iterator(end());}
	const_reverse_iterator rbegin() const{return boost::make_reverse_iterator(end());}
	reverse_iterator       rend()        {return boost::make_reverse_iterator(begin());}
	const_reverse_iterator rend()   const{return boost::make_reverse_iterator(begin());}
	const_iterator         cbegin() const{return begin();}
	const_iterator         cend()   const{return end();}
	const_reverse_iterator crbegin()const{return rbegin();}
	const_reverse_iterator crend()  const{return rend();}
	iterator iterator_to(const value_type& x) { return iterator(internal_iterator_to(boost::multi_index::detail::node_from_value<node_type>(&x))); }
	const_iterator iterator_to(const value_type& x)const {return const_iterator(internal_iterator_to(boost::multi_index::detail::node_from_value<node_type>(&x)));}

public:

	void copy_( const list_index& x, const copy_map_type& map )
	{
		// [Custom]
		for(const_iterator from=x.begin(); from!=x.end(); ++from)
		{
			node_type* p = map.find(static_cast<final_node_type*>(&*from.base()));
			p->construct_();
			lst.push_back(*p);
		}

		super::copy_(x, map);
	}

	node_type* insert_( value_param_type v, node_type* x )
	{
		node_type* res = static_cast<node_type*>(super::insert_(v,x));
		// [Custom]
		//   - v に値、x に新しく用意されたノード
		//   1. ordered_index等で既にinsert済みだったらinsert済みのノードを返す
		//   2. そうでなければ、親を呼ぶ
		//   3. 帰ってきたのがxそのものなら、自分のリンクに繋ぐ
		if(res==x) {x->construct_(); lst.push_back(*x);}
		return res;
	}

	node_type* insert_( value_param_type v, node_type* position, node_type* x )
	{
		node_type* res = static_cast<node_type*>(super::insert_(v,position,x));
		// [Custom]
		//   - v に値、x に新しく用意されたノード、position はただのヒント
		//   1. ordered_index等で既にinsert済みだったらinsert済みのノードを返す
		//   2. そうでなければ、親を呼ぶ
		//   3. 帰ってきたのがxそのものなら、自分のリンクに繋ぐ
		if(res==x) {x->construct_(); lst.push_back(*x);}
		return res;
	}

	void erase_(node_type* x)
	{
		// [Custom]
		lst.erase( LIST::s_iterator_to(*x) );
		x->destruct_();
		super::erase_(x);
	}

	void delete_node_(node_type* x)
	{
		// たぶんdelete_all_nodes_のときだけ呼ばれる
		// [Custom]
		lst.erase( LIST::s_iterator_to(*x) );
		x->destruct_();
		super::delete_node_(x);
	}

	void delete_all_nodes_()
	{
		// [Custom]
		// そうか index_base はこれをできないからどれかがやってやらないといけない
		for(internal_iterator it=lst.begin(); it!=lst.end(); )
		{
			internal_iterator jt = it++;
			lst.erase(jt);
			this->final_delete_node_( static_cast<final_node_type*>(&*jt) );
		}
	}

	void clear_()
	{
		// [Custom]
		super::clear_();
		lst.clear();
	}

	void swap_( list_index& x )
	{
		// [Custom]
		lst.swap(x.lst);
		super::swap_(x);
	}

	bool replace_( value_param_type v, node_type* x )
	{
		return super::replace_(v, x);
	}

	bool modify_(node_type* x)
	{
		try {
			if( !super::modify_(x) ){
				lst.erase( internal_iterator_to(*x) );
				return false;
			}
		} catch( ... ) {
			lst.erase( internal_iterator_to(*x) );
			throw;
		}
		return true;
	}

	bool modify_rollback_( node_type* x )
	{
		return super::modify_rollback_(x);
	}

public:
	// コンテナの実装 (1)
	bool      empty()    const {return this->final_empty_();}
	size_type size()     const {return this->final_size_();}
	size_type max_size() const {return this->final_max_size_();}
	void      swap(list_index& x)    {this->final_swap_(x.final());}
	void      clear()          {this->final_clear_();}
	const_reference front() const {return *begin();}
	const_reference back()  const {return *--end();}

	// コンテナの実装 (2)
	std::pair<iterator,bool> push_front(value_param_type x) {return insert(begin(),x);}
	void                     pop_front(){erase(begin());}
	std::pair<iterator,bool> push_back(value_param_type x)  {return insert(end(),x);}
	void                     pop_back() {erase(--end());}

	// コンテナの実装 (3)
	template<typename InputIterator>
	void insert(iterator position,InputIterator first,InputIterator last)
	{
		// 本来は first, last が value, num の場合と区別する場合に頑張るべきだが知らん
		for(; first!=last; ++first)
			insert(position, *first);
	}
	void insert(iterator position, size_type n, value_param_type x)
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
	bool replace(iterator position,value_param_type x)
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
	std::pair<iterator,bool> insert(iterator position, value_param_type x)
	{
		std::pair<final_node_type*,bool> p = this->final_insert_(x);
		// とりあえず末尾に挿入されてるのでpositionに動かす
		if( p.second )
			lst.splice( position.base(), lst, internal_iterator_to(*p.first) );
		return std::pair<iterator,bool>(internal_iterator_to(*p.first),p.second);
	}
/*
  void resize(size_type n,value_param_type x=value_type())
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
