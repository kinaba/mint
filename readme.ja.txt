-----------------------------------------------------------------------------
  mint  =  <M>ultiIndex-powered-by-<Int>rusive
                                             very early prototype version!

                               written by k.inaba (http://www.kmonos.net/)
                           under NYSL 0.9982 (http://www.kmonos.net/nysl/)
-----------------------------------------------------------------------------


Boost.�׋��� ( http://atnd.org/events/1839 ) �̔��\�l�^�ł��B

  sample.cpp           : �g�����T���v��
  mint/node.hpp        : ��r�I�ڍׂȃR�����g�������
  mint/splay.hpp       : ��r�I�ڍׂȃR�����g�������
  mint/splay_index.hpp : ��r�I�ڍׂȃR�����g�������
  mint/list.hpp        : �Ă��Ƃ�
                    �� : splay �̃R�s�|���u��

��������肩���ł��B



Todo:
  - impl �� const_iterator ����؎g�킸�S�� iterator �Œʂ��Ă���̂�
    const �֌W���j�ł��Ă��邽�߂ǂ��ɂ�����

  - set�n�C���f�b�N�X�� modify_ �̎������@���l����
      - intrusive_hook �̓���������G��Ȃ��ł����悤�ȕ��@

  - KeyExtractor ���g���悤�ɂ���

  - Boost.Serialization�֌W�̃��\�b�h�̐������������@���m�F

  - �R�s�y����Ȃ��āA1�̃e���v���[�g�őS���� intrusive::xxx_(multi)set ����
    index �𐶐��ł���悤�ɂ��ׂ�
