#ifndef KMONOS_NET_MINT_impl_type_INDEX_HPP
#define KMONOS_NET_MINT_impl_type_INDEX_HPP
#include <iostream>
using namespace std;
#include "common.hpp"
#include "node.hpp"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>

namespace mint { namespace detail {





template<typename SuperMeta, typename TagList>
class list_index : protected SuperMeta::type
{
private:
	typedef list_index               self;
	typedef typename SuperMeta::type super;

protected:
	typedef intrusive_hook_node<
		boost::intrusive::list_member_hook<>, typename super::node_type
	> node_type;

public:
	typedef typename node_type::value_type                   value_type;
	typedef typename super::final_allocator_type             allocator_type;
	typedef typename allocator_type::reference               reference;
	typedef typename allocator_type::const_reference         const_reference;

private:
	typedef boost::intrusive::list<node_type,
		boost::intrusive::member_hook<node_type,
			boost::intrusive::list_member_hook<>, &node_type::hook_>
	> impl_type;

	impl_type impl_;

	typedef typename impl_type::iterator internal_iterator;

	internal_iterator internal_iterator_to( node_type* p ) { return impl_type::s_iterator_to(*p); }
	internal_iterator internal_iterator_to( node_type& x ) { return impl_type::s_iterator_to(x); }
	internal_iterator internal_iterator_to( const value_type& x )
		{ return internal_iterator_to(boost::multi_index::detail::node_from_value<node_type>(&x)); }

public:
	typedef boost::tuples::null_type ctor_args;

	typedef custom_indirect_iterator<
		internal_iterator,
		value_type, boost::use_default, const value_type&, ptrdiff_t> iterator;

	typedef iterator                                         const_iterator;
	typedef std::size_t                                      size_type;      
	typedef std::ptrdiff_t                                   difference_type;
	typedef typename allocator_type::pointer                 pointer;
	typedef typename allocator_type::const_pointer           const_pointer;
	typedef typename boost::reverse_iterator<iterator>       reverse_iterator;
	typedef typename boost::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef TagList                                          tag_list;

protected:
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
	// [Custom] コンストラクタとデストラクタ
	list_index(const list_index& x) : super(x) {}
	list_index(const ctor_args_list& args_list,const allocator_type& al)
		: super(args_list.get_tail(), al) {}
	~list_index() {}
	self& operator=( self& rhs ) { this->final() = rhs.final(); return *this; }
	allocator_type get_allocator() const { return this->final().get_allocator(); }

public:
	// イテレータを作る系メソッド
	iterator               begin()         {return iterator(impl_.begin()); }
	const_iterator         begin()   const {return const_iterator(const_cast<impl_type&>(impl_).begin()); }
	iterator               end()           {return iterator(impl_.end());}
	const_iterator         end()     const {return const_iterator(const_cast<impl_type&>(impl_).end());}

	reverse_iterator       rbegin()        {return boost::make_reverse_iterator(end());}
	const_reverse_iterator rbegin()  const {return boost::make_reverse_iterator(end());}
	reverse_iterator       rend()          {return boost::make_reverse_iterator(begin());}
	const_reverse_iterator rend()    const {return boost::make_reverse_iterator(begin());}
	const_iterator         cbegin()  const {return begin();}
	const_iterator         cend()    const {return end();}
	const_reverse_iterator crbegin() const {return rbegin();}
	const_reverse_iterator crend()   const {return rend();}

	iterator       iterator_to(const value_type& x)       {return iterator(internal_iterator_to(x));}
	const_iterator iterator_to(const value_type& x) const {return const_iterator(internal_iterator_to(x));}

public:
	void copy_( const self& x, const copy_map_type& map )
	{
		for(const_iterator from=x.begin(); from!=x.end(); ++from)
		{
			node_type* p = map.find(static_cast<final_node_type*>(&*from.base()));
			p->construct_();
			impl_.push_back(*p);
		}
		super::copy_(x, map);
	}

	node_type* insert_( const value_type& v, node_type* x )
	{
		node_type* res = static_cast<node_type*>(super::insert_(v,x));
		if(res==x) {x->construct_(); impl_.push_back(*x);}
		return res;
	}

	node_type* insert_( const value_type& v, node_type* position, node_type* x )
	{
		return insert_(v, x);
	}

	void erase_(node_type* x)
	{
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();
		super::erase_(x);
	}

	void delete_node_(node_type* x)
	{
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();
		super::delete_node_(x);
	}

	void delete_all_nodes_()
	{
		for(internal_iterator it=impl_.begin(); it!=impl_.end(); )
		{
			node_type* p = &*it++;
			this->final_delete_node_( static_cast<final_node_type*>(p) );
		}
	}

	void clear_()
	{
		impl_.clear();
		super::clear_();
	}

	void swap_( list_index& x )
	{
		impl_.swap(x.impl_);
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
				impl_.erase( internal_iterator_to(*x) );
				return false;
			}
		} catch( ... ) {
			impl_.erase( internal_iterator_to(*x) );
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
			impl_.splice( position.base(), impl_, internal_iterator_to(*p.first) );
		return std::pair<iterator,bool>(internal_iterator_to(*p.first),p.second);
	}
};



}}     // end of namespace
#endif // include guard
