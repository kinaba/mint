#ifndef KMONOS_NET_MINT_AVL_INDEX_HPP
#define KMONOS_NET_MINT_AVL_INDEX_HPP

#include "common.hpp"
#include "node.hpp"
#include <algorithm>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/avl_set_hook.hpp>

//---------------------------------------------------------------------------
// MultiIndex �p "�C���f�b�N�X" �̎�����
//---------------------------------------------------------------------------

namespace mint { namespace detail {

template<typename Compare, typename SuperMeta, typename TagList>
class avl_index
	: protected SuperMeta::type
{
	// �Ⴆ�΃C���f�b�N�X�R�w�肵���R���e�i�Ȃ�
	//
	//   class multi_index_container<T, indexed_by<S1,S2,S3>>
	//      : public S1::index_class<SuperMeta1>::type
	//   {
	//      S1::index_class<SuperMeta1>::type& get<0>() { return *this; }
	//      S2::index_class<SuperMeta2>::type& get<1>() { return *this; }
	//      S3::index_class<SuperMeta3>::type& get<2>() { return *this; }
	//   }
	//
	// ����ȕ��ɃC���f�b�N�X���g���܂��B
	// SuperMeta �ɂ͉����}�W�J���ɕK�v�ȏ�񂪐F�X�����Ă܂����A�d�v�Ȃ̂�
	//
	//   SuperMeta::type
	//
	// �����ł��B�C���f�b�N�X����������Ƃ��͂��̌^�� protected �p�����܂��B
	// ����ȕ��Ȍp���֌W�ɂȂ�܂��B
	//
	//   multi_index_container<...>
	//     : public S1::index_class<...>::type 
	//     : protected S2::index_class<...>::type 
	//     : protected S3::index_class<...>::type 
	//     : protected boost::multi_index::detail::index_base<...>
	//


	// �ȉ��̃R�����g��
	//   [Impl]     �́A����� avl_set �p�C���f�b�N�X�̂��߂̎����ڍ�
	//   [Custom]   �́A�����������C���f�b�N�X�ɍ��킹�ĐF�X�ς��Ďg���֐�/�^��`
	//   [Required] �́A���̃C���f�b�N�X���炻�̂܂܃R�s�y�Ŏg����֐�/�^��`
	//                  �{���͂����ƃ��C�u������������������

private:
	// [Impl] SuperMeta::type �͕p�ɂɎg���̂� typedef ���Ă����܂��B
	typedef avl_index              self;
	typedef typename SuperMeta::type super;

protected:
	// [Custom]
	//    ���̃C���f�b�N�X�Ŏg���m�[�h�^�� node_type �� typedef
	typedef intrusive_hook_node<
		boost::intrusive::avl_set_member_hook<>, typename super::node_type
	> node_type;

public:
	// [Required] �R���e�i�Ƃ��Ďg�����߂ɕK�v�Ȍ^��`
	typedef typename node_type::value_type                   value_type;
	typedef typename super::final_allocator_type             allocator_type;
	typedef typename allocator_type::reference               reference;
	typedef typename allocator_type::const_reference         const_reference;

private:
	// [Impl]
	//    intrusive �� avl_set ���g������������

	typedef boost::intrusive::avl_set<node_type,
		boost::intrusive::member_hook<node_type,
			boost::intrusive::avl_set_member_hook<>, &node_type::hook_>,
		boost::intrusive::compare< typename node_type::template comparator<Compare> >
	> impl_type;

	// splay �͒��ŏ���ɓ����̂�const����find���ł��Ȃ��c
	mutable impl_type impl_;

	typedef typename impl_type::iterator internal_iterator;

	// [Impl]
	//     node_type �� value_type ���� Boost.Intrusive �f�[�^�\���̃C�e���[�^���擾����֐�
	//     node_type ����C�e���[�^�ւ̕ϊ��� Boost.Intrusive �̋@�\
	//     value_type �ւ̎Q�Ƃ���A���ꂪ�����Ă�m�[�h�̃A�h���X�𓾂�̂� MultiIndex �̋@�\
	internal_iterator internal_iterator_to( node_type& x ) { return impl_type::s_iterator_to(x); }
	internal_iterator internal_iterator_to( node_type* p ) { return impl_type::s_iterator_to(*p); }
	internal_iterator internal_iterator_to( const value_type& x )
		{ return internal_iterator_to(boost::multi_index::detail::node_from_value<node_type>(&x)); }

public:
	// [Custom]
	//     �������R���X�g���N�^�ɕK�v�Ȉ������X�g�ɒu��������
	//     ����̓[�������ł����̂� null_type ��
	typedef boost::tuples::null_type ctor_args;

	// [Custom]
	//     �C�e���[�^�̒�`
	//     ����y�����Ƃ������Ƃ� Boost.Iterators ���C�u�����ŃT�N�b�ƍ��܂�
	//     (������get_node �����o�֐���ǉ�: see common.hpp)
	//
	//     ���ӓ_�Ƃ��āAstd::set ���Ɠ������A��� const �Q�Ƃ�Ԃ��i�ύX�s�́j
	//     �C�e���[�^���������܂��B�l��ύX����Ƃ��͓񕪖؂̃o�����X���������
	//     ���Ȃ��Ƃ����Ȃ���������Ȃ��̂ŁA��p�̊֐� modify() �� replace() ��
	//     �Ă�ł��炢�܂��B
	typedef custom_indirect_iterator<
		internal_iterator,
		value_type, boost::use_default, const value_type&, ptrdiff_t> iterator;

	// [Required] �R���e�i�Ƃ��Ďg�����߂ɕK�v�Ȍ^��`
	typedef iterator                                         const_iterator;
	typedef std::size_t                                      size_type;      
	typedef std::ptrdiff_t                                   difference_type;
	typedef typename allocator_type::pointer                 pointer;
	typedef typename allocator_type::const_pointer           const_pointer;
	typedef typename boost::reverse_iterator<iterator>       reverse_iterator;
	typedef typename boost::reverse_iterator<const_iterator> const_reverse_iterator;

	// [Required] multi_index ���g���^��`
	typedef TagList                                          tag_list;

protected:
	// [Required] multi_index ���g���^��`
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
	//   �R���X�g���N�^�ƃf�X�g���N�^�ƃR�s�[��
	avl_index( const avl_index& rhs ) : super(rhs) {} // ���Ƃ� copy_ ���Ă΂��
	avl_index( const ctor_args_list& args_list, const allocator_type& al )
		: super( args_list.get_tail(), al ) {}
	~avl_index() {}
	self& operator=( self& rhs ) { this->final() = rhs.final(); return *this; }
	allocator_type get_allocator() const { return this->final().get_allocator(); }

public:
	// [Custom]
	//    �C�e���[�^�����n���\�b�h
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
	//   �i�[����Ă���l�ւ̎Q�Ƃ���t�Z���āA������w���C�e���[�^������A
	//   �Ƃ����͕̂��ʂ̕W���R���e�i�ɂ͂Ȃ��@�\�������肵�܂��B
	iterator       iterator_to(const value_type& x)       {return iterator(internal_iterator_to(x));}
	const_iterator iterator_to(const value_type& x) const {return const_iterator(internal_iterator_to(x));}

public:
	//------------------------------------------------------------------------------------
	// [Custom]
	//    ��������AMutliIndex �R���e�i�̑���̂��߂̓����v���~�e�B�u�̎����B
	//    ���[�U�[�Ɏg���Ă��炤���߂�API�֐� (push_back �Ƃ�) �́A
	//    ���̕ӂ̃v���~�e�B�u���g���Ď�������B
	//
	//    pop_back() �̎������Ƃ��Đ������܂��B�g���v���~�e�B�u�� erase_()�B
	//    multi_index_container<T,indexed_by<S1,MyIndex,S2> �Ƃ�����ōl�����
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
	//    �����A
	//      - �Ō���w���m�[�h���i�����̃C���f�b�N�X���𗘗p���āj������
	//      - final_erase_() ���Ă�
	//      - final_erase_() �͑S���̃C���f�b�N�X�� erase_() ���Ă�ŉ��
	//      - �e�C���f�b�N�X�� erase_() ���Ă΂ꂽ�玩���̂Ƃ���̃����N���O��
	//      - index_base::erase_ ���m�[�h�̃����������
	//    ����ȗ���ɂȂ��Ă܂��B�v��
	//      * �C���f�b�N�X�Ƃ��Ē񋟂��������\�b�h�� final_xxx_ ���g���Ď�������
	//	    * ���̃C���f�b�N�X(�܂ގ���)�̕ύX����𔽉f���邽�߂̓����֐� xxx_ ������Ă���
	//    �ƂȂ�܂��B
	//------------------------------------------------------------------------------------

	void copy_( const self& x, const copy_map_type& map )
	{
		// [Custom]
		//   x �̂��ׂẴm�[�h���R�s�[����
		//
		//   map.find(...) �́A���̃m�[�h�����ɑ��̃C���f�b�N�X�ŃR�s�[����Ă���
		//   ���̃m�[�h���A����ĂȂ���ΐV�������蓖�Ă��m�[�h��Ԃ��B
		//   ���̋A���Ă����m�[�h��K�X������̃C���f�b�N�X�Ōq�������΂悢�B

		for(const_iterator from=x.begin(); from!=x.end(); ++from)
		{
			node_type* p = map.find(static_cast<final_node_type*>(&*from.base()));
			p->construct_();
			impl_.insert(*p);
		}

		//   �Ō�ɁA�e�̃C���f�b�N�X�ł������N�\���̃R�s�[�����Ȃ��Ƃ����Ȃ��̂�
		//   �Y�ꂸ super::copy_()

		super::copy_(x, map);
	}

	node_type* insert_( const value_type& v, node_type* x )
	{
		// [Custom]
		//   �l v �̓���m�[�h x ��V�����}������B
		//   �f�t�H���g�̑}���ʒu�iset�n�Ȃ珇���I�ɍl���Đ������ʒu�Alist�n�Ȃ疖�����j
		//   �ɓ˂����߂΂悢�B
		//
		//   �菇�́A
		//     1. �܂��A�����̃C���f�b�N�X�I�ɍl���đ}���\���ǂ������f
		//        set�̏d������ȂǁB
		//        �s�\��������d�����Ă錳�m�[�h��Ԃ�
		//        ���m�[�h�Ƃ����������̂��Ȃ���Α��� null ��Ԃ��̂��ȁ����ׂĂȂ�

		internal_iterator it = impl_.find(v, impl_.value_comp());
		if( it != impl_.end() )
			return &*it;

		//     2. �����ɑ}���\�Ȃ�A�܂��͗��������Đe���Ă�

		node_type* res = static_cast<node_type*>(super::insert_(v,x));

		//     3. �e����т���c�l�C���f�b�N�X�S���ő}���������Ă���A
		//        �����ł͂��߂Ď����̃����N�Ɍq��

		if( res == x ) {
			x->construct_(); // x�͐V�����m�[�h�Ȃ͂��Ȃ̂ŏ�����
			impl_.insert(*x);
		}
		return res;
	}

	node_type* insert_( const value_type& v, node_type* position, node_type* x )
	{
		// [Custom]
		//    �߂�ǂ������̂Ńq���g�̓K������
		//    ���Ƃł����Ə���

		return insert_( v, x );
	}

	void erase_(node_type* x)
	{
		// [Custom]
		//    x���폜����B�܂莩���̃����N����O��
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();

		// �e�̃����N������͂���
		super::erase_(x);
	}

	void delete_node_(node_type* x)
	{
		// [Custom]
		//    ���Ԃ�delete_all_nodes_�̂Ƃ������Ă΂��
		//    �̂őS�������킩���Ă�̂ł����Ă���index�����͂Ȃɂ����ĂȂ��H
		impl_.erase( impl_type::s_iterator_to(*x) );
		x->destruct_();

		// �e
		super::delete_node_(x);
	}

	void delete_all_nodes_()
	{
		// [Custom]
		//    index_base �͂�����ł��Ȃ�����ǂꂩ������Ă��Ȃ��Ƃ����Ȃ�
		//    �S���̃m�[�h�������ĉ��
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
		//   ���肠�[
		impl_.clear();
		super::clear_();
	}

	void swap_( avl_index& x )
	{
		// [Custom]
		//   �X���b�v
		impl_.swap(x.impl_);
		super::swap_(x);
	}

	bool replace_( const value_type& v, node_type* x )
	{
		// [Custom]
		//   �m�[�h x �ɓ����Ă�l�� v �ɕύX����
		//
		//   �菇�́A
		//     1. �܂��A�����̃C���f�b�N�X�I�ɍl���ĕύX�ł��邩�ǂ������f
		//        set�̏d������ȂǁB
		//        �ύX�����������}�Y���Ȃ� false ��Ԃ�

		internal_iterator it = impl_.find(v, impl_.value_comp());
		if( it != impl_.end() )
			return false;

		//     2. �����ɑ}���\�Ȃ�A�e�ɂ����f���𗧂Ă�

		if( !super::replace_(v, x) )
			return false;

		//     3. �e����ԏ�܂œo��؂����炲��c�l�� x �Ɏ��ۂ� v ��
		//        ����Ă����Ă����̂ŁA��͎����p�����N���q������

		impl_.erase(*x);
		impl_.insert(*x);
		return true;
	}

	bool modify_rollback_( node_type* x )
	{
		// [Custom]
		//   �m�[�h x �ɓ����Ă�l�����łɕς���Ă�̂�
		//   �K�v�Ȃ�Ȃ�Ƃ��������ʒu�ɓ������Ă��������Ƃ������\�b�h

		// Intrusive ���Y��Ȏ������v�����Ȃ��̂łƂ肠�����ۗ��E�E�E
		return false;


		// �����菇�́Areplace_ �Ɠ���
		//   1. �����ɂƂ���OK���m�F
		//   2. �e�ɂ����f���𗧂Ă�
		//   3. �e���S��OK�Ȃ烊���N�q������
		// false��Ԃ��ƕύX�����[���o�b�N�����̂ŁA
		// x ���}�Y���ς��������Ă��Ă��P��false��Ԃ��Ă����Έ��S
	}

	bool modify_(node_type* x)
	{
		// [Custom]
		//   �m�[�h x �ɓ����Ă�l�����łɕς���Ă�̂�
		//   �K�v�Ȃ�Ȃ�Ƃ��������ʒu�ɓ������Ă��������Ƃ������\�b�h
		//   modify_rollback_ �Ƃ̈Ⴂ�́Afalse��Ԃ��ƃ��[���o�b�N�ł�����
		//   �m�[�h x ���폜����邱�ƁB
		
		// �ۗ�
		return false;

		// �����菇�́Amodify_rollback_ �ɉ����āAfalse ��Ԃ������O�Ŕ������肷��ꍇ��
		// erase_ �����̎菇�� x �̃����N���O���Ă����Ȃ��Ƃ����Ȃ��B
		//   try {
		//     if( �������_�� || !super::modify_(x) ){
		//       impl_.erase( internal_iterator_to(*x) );
		//       return false;
		//     }
		//   } catch( ... ) {
		//     impl_.erase( internal_iterator_to(*x) );
		//     throw;
		//   }
		//   return true;
		// ����ȏ��ԂɂȂ�
	}


public:
	//------------------------------------------------------------------------------------
	// [Custom]
	//    ��������A�C���f�b�N�X�̒񋟂��邢�낢��ȃ����o�֐��̎���
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
