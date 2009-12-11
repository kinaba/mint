#include <boost/test/minimal.hpp>

#include "mint/list.hpp"
#include "mint/splay.hpp"


#include <iostream>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::multi_index;





int test_main( int argc, char* argv[] )
{
	typedef multi_index_container<string,
	   indexed_by<
         mint::list<>
         ,mint::splay< std::less<string> >
	> > container;



	container c;
	c.push_back("This");
	BOOST_CHECK( c.size() == 1 );
	c.push_back("Is");
	BOOST_CHECK( c.size() == 2 );
	c.push_back("A");
	BOOST_CHECK( c.size() == 3 );
	c.push_back("Pen");
	BOOST_CHECK( c.size() == 4 );
	c.push_back("That");
	BOOST_CHECK( c.size() == 5 );
	c.push_back("Is");
	BOOST_CHECK( c.size() == 5 );
	c.push_back("Also");
	BOOST_CHECK( c.size() == 6 );
	c.push_back("A");
	BOOST_CHECK( c.size() == 6 );
	c.push_back("Pen");
	BOOST_CHECK( c.size() == 6 );

	container d = c;
	c.clear();
	BOOST_CHECK( c.size() == 0 );

	c.swap(d);
	BOOST_CHECK( d.size() == 0 );
	BOOST_CHECK( c.size() == 6 );

	container::nth_index<1>::type& c2 = c.get<1>();
	BOOST_CHECK( c2.front() == "A" );
	BOOST_CHECK( c2.back() == "This" );

	d = c;

	c2.erase("Is");
	BOOST_CHECK( c.size() == 5 );
	BOOST_CHECK( c2.size() == 5 );
	BOOST_CHECK( d.size() == 6 );
	BOOST_CHECK( d.get<1>().size() == 6 );


	string data[] = {"This", "A", "Pen", "That", "Also"};
	BOOST_CHECK( std::equal(data+0, data+5, c.begin()) );
	BOOST_CHECK( std::equal(data+0, data+5, c.get<0>().begin()) );
	BOOST_CHECK( !std::equal(data+0, data+5, d.begin()) );
	sort(data+0, data+5);
	BOOST_CHECK( std::equal(data+0, data+5, c.get<1>().begin()) );

	container::iterator it =
		get<0>(c).iterator_to( *get<1>(c).find("Pen") );
	BOOST_CHECK( *it == "Pen" );
	++it;
	BOOST_CHECK( *it == "That" );
	--it; --it; --it;
	BOOST_CHECK( *it == "This" );

	container::nth_index<1>::type::iterator jt = get<1>(c).iterator_to( *it );
	return 0;
}
