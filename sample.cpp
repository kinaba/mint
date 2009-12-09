#include "mint/list.hpp"
#include "mint/splay.hpp"



#include <iostream>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/foreach.hpp>
#include <typeinfo>
#include <boost/test/minimal.hpp>


using namespace std;
using namespace boost;
using namespace boost::multi_index;
using namespace boost::multi_index::detail;







int test_main( int argc, char* argv[] )
{
	typedef multi_index_container<string,
	   indexed_by<
	     sequenced<>,
	     ordered_non_unique< identity<string> >
         ,mint::list<>
	> > container;

	// まず、１つめに指定したアクセス手段（sequenced<>）を持つコンテナとして普通に使える
	container c;
	c.push_back("This");
	c.push_back("Is");
	c.push_back("A");
	c.push_back("Pen");
	c.push_back("That");
	c.push_back("Is");
	c.push_back("Also");
	c.push_back("A");
	c.push_back("Pen");
container d = c;
	container::nth_index<2>::type& c2 = d.get<2>(); //←これがまさにmy_index型
	cout << "c2: ";
	BOOST_FOREACH( const string& s, c2 )
		cout << s << ' ';
	cout << endl;
	c2.push_back("This");
	c2.push_back("Is");
	c2.push_back("A");
	c2.push_back("Pen");
	c2.push_back("That");
	c2.push_back("Is");
	c2.push_back("Also");
	c2.push_back("A");
	c2.push_back("Penny");

	cout << "c0: ";
	BOOST_FOREACH( const string& s, d )
		cout << s << ' ';
	cout << endl;

	// 別のindex（ordered_non_unique...）でアクセスしたい場合は、get<番号>
	container::nth_index<1>::type& c1 = c.get<1>();
	cout << "c1: ";
	BOOST_FOREACH( const string& s, c1 )
		cout << s << ' ';
	cout << endl;

	// set風に使用（O(logN)で実行される）
	cout << c1.count("Pen") << endl;

	// set風に使用（O(logN)で実行される）
	c1.erase("Is");

	// あるindexから実行した操作は、別のindexにも反映されてる
	BOOST_FOREACH( const string& s, c )
		cout << s << ' ';
	cout << endl;

	return 0;
}
