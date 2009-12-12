#ifndef KMONOS_NET_MINT_AVL_HPP
#define KMONOS_NET_MINT_AVL_HPP

#include "common.hpp"
#include "node.hpp"
#include "avl_index.hpp"
#include <functional>
#include <boost/intrusive/avl_set_hook.hpp>

//---------------------------------------------------------------------------
// MultiIndex �p"�C���f�b�N�X�w��q"�̎�����
//---------------------------------------------------------------------------

namespace mint {

template< typename Compare, // �Ƃ肠�����蔲���B�{����KeyExtractor�g����悤�ɂ������B���ƃR���X�g���N�^�����c
          typename TagList=boost::multi_index::tag<> >
struct avl
{
	// multi_index_container ���g���Ƃ��̃C���f�b�N�X�w��
	//
	//   multi_index_container<T, indexed_by<����, ����, ����>>
	//
	// �ɂ� "�C���f�b�N�X�w��q (index specifier)" ��
	// �Ă΂���ނ̃N���X��n���܂��B
	// ���Ȃ݂ɁAindexed_by ��Boost.MPL�Ŏg����V�[�P���X�Ȃ�Ȃ�ł������ł��B
	//   multi_index_container<T, mpl::list<����, ����, ����>>
	// �Ƃ��B
	//
	//
	// �C���f�b�N�X�w��q�����삷��ɂ�
	//   - ����m�[�h�̎�����Ԃ�        node_class  (�ڍׂ� node.hpp ��)
	//   - ����C���f�b�N�X�̎�����Ԃ� index_class  (�ڍׂ� list_index.hpp ��)
	// �̓�̃��^�֐��i�����o�N���X�e���v���[�g�j���`����΂悢�ł��B
	// ������Ȋ����B

	template<typename Super>
	struct node_class
	{
		typedef detail::intrusive_hook_node<
		            boost::intrusive::avl_set_member_hook<>, Super
		        > type;
	};

	template<typename SuperMeta>
	struct index_class
	{
		typedef detail::avl_index<
		            Compare, SuperMeta, typename TagList::type
		        > type;
	};
};

}      // end of namespace
#endif // include guard
