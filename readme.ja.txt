-----------------------------------------------------------------------------
  mint  =  <M>ultiIndex-powered-by-<Int>rusive
                                             very early prototype version!

                               written by k.inaba (http://www.kmonos.net/)
                           under NYSL 0.9982 (http://www.kmonos.net/nysl/)
-----------------------------------------------------------------------------


Boost.勉強会 ( http://atnd.org/events/1839 ) の発表ネタです。

  sample.cpp           : 使い方サンプル
  mint/node.hpp        : 比較的詳細なコメント入り実装
  mint/splay.hpp       : 比較的詳細なコメント入り実装
  mint/splay_index.hpp : 比較的詳細なコメント入り実装
  mint/list.hpp        : てきとう
                    他 : splay のコピ－＆置換

すごく作りかけです。



Todo:
  - impl の const_iterator を一切使わず全部 iterator で通しているので
    const 関係が破滅しているためどうにかする

  - set系インデックスの modify_ の実装方法を考える
      - intrusive_hook の内部実装を触らないでいいような方法

  - KeyExtractor を使うようにする

  - Boost.Serialization関係のメソッドの正しい実装方法を確認

  - コピペじゃなくて、1個のテンプレートで全部の intrusive::xxx_(multi)set から
    index を生成できるようにすべき
