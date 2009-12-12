#ifndef KMONOS_NET_MINT_NODE_HPP
#define KMONOS_NET_MINT_NODE_HPP

//---------------------------------------------------------------------------
// Boost.Intrusive �̃t�b�N���g�����AMultiIndex �p "�m�[�h" �̎�����
//---------------------------------------------------------------------------

namespace mint { namespace detail {

template<typename Hook, typename Super, typename Priority = void>
struct intrusive_hook_node
	: public Super
{
	// �m�[�h�^�́A�Ⴆ�΃C���f�b�N�X�R�w�肵���R���e�i�Ȃ�
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
	// ����ȕ��� typedef �Œ�`����܂��i���ۂ�typedef�͍ċA�e���v���[�g�Ŏ����j�B
	//
	// �m�[�h�^���������鑤�� node_class �ɓn�������� Super �� public �p�����܂��B
	//
	//   node_type =
	//      S1::node_class<...>::type
	//        : public S2::node_class<...>::type
	//        : public S3::node_class<...>::type
	//        : public boost::multi_index::detail::index_base_node<T>
	//
	// ����ȕ��Ȍp���֌W�ɂȂ�܂��B



	// ���̌p����������΁A���Ƃ͉��ł��D���Ɏ������Ă悳�����B
	// index_base_node �� T& value() �Ƃ��������o�������āA
	// �m�[�h�ɓ����Ă���l������̂ŁA���Ƃ͎ς�Ȃ�Ă��Ȃ�B



	// �������AAllocator::allocate() �Ő��̃������Ƃ��Ē��ڊm�ۂ����
	// �R���X�g���N�^��f�X�g���N�^�͌Ă΂�Ȃ��̂Œ��ӁB
	//
	// �C���f�b�N�X�� insert_() �����o�֐����Ă΂ꂽ�Ƃ��� allocate ����āA
	// erase_() �܂��� delete_node_() �������o�֐����Ă΂ꂽ��� deallocate �����̂�
	// �K�v�Ȃ炻�̃^�C�~���O�Ŏ��O�ŏ�����/�I�������������܂��傤�B



	// �ȉ��ABoost.Intrusive ���g�����߂̎����ڍ�

	Hook hook_;                              // Boost.Intrusive �̃t�b�N���ė��p
	void construct_() { new (&hook_) Hook; } // �������̂� placement new ��
	void destruct_()  { hook_.~Hook(); }     // �����������(= �R���X�g���N�^�ďo)

	// �C�e���[�^�� boost::indirect_iterator �ł���y�������邽�߂̕⏕�֐�
	const typename Super::value_type& operator*() const { return this->value(); }

	// boost::intrusive::xxx_set �ɓ˂����ނ��߂̔�r�֐��I�u�W�F�N�g
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

	// treap �p
	friend bool priority_order(const intrusive_hook_node& a, const intrusive_hook_node& b)
	{
		return Priority()(a.value(), b.value());
	}
};




}}     // end of namespace
#endif // include guard
