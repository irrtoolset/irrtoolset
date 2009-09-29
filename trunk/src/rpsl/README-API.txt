I do not for see any major library/api changes for couple of months (this
may be the last one ever). I am now pretty satisfied how simple it got
with the new AttrGenericIterator. For example, to get an origin of a route
object r:

	AttrGenericIterator<ItemASNO> itr(r, "origin");
	ASt origin = itr()->asno;

Or to walk through all the rs-sets a route belongs:
	AttrGenericIterator<ItemRSNAME> itr(r, "member-of");
either:
	for (ItemRSNAME *rsname = itr.first(); rsname; rsname = itr.next())
		cout << rsname->rsname;
or
	while (itr) {
		cout << itr()->rsname;
		itr++;
	}

There is also
	AttrIterator<AttrImport> itr(autnum, "import");

The difference betweek AttrIterator and AttrGenericIterator is that 
AttrIterator works for any attribute, and walks over each attribute
definition with given name. AttrGenericIterator on the other hand only 
works for generic atttributes (most rpsl attributes are in this class, 
e.g. origin, member, member-of, mnt-by, ...), and it walks through
items in the values of attributes with given name. I.e.
	route:
	origin:
	member-of: rs-foo, rs-bar
	member-of: rs-car, rs-cdr

AttrIterator<AttrGeneric> itr(route, "member-of") will return twice, one
for each member-of line. AttrGenericIterator<ItemRSNAME> itr(route,
"member-of") will return 4 times, returning rs-foo, rs-bar, rs-car,
rs-cdr...  

ptree.cc and ptree.hh are gone. They are replaced with several files:
	rpsl_item.*	useful for generic attributes (see above)
	rpsl_filter.*	rpsl filter parse tree
	rpsl_policy.*   actions, peerings, except/refine etc...
	rpsl_attr.*     attributes, import, export, default, generic, ...

List Class:
To make a list of class Foo:
	class Foo : public ListNode {
		int a;
	...  // no member ListNode l needed (and wont work)
	};

	List<Foo> l;

	for (Foo *f = l.head(); f; f = l.next(f)) 
		...

	or
	ListIterator<Foo> itr(l);
	while (itr) {
		Foo *f = itr;
		cout << itr()->a;
		itr++;
	}

There are also special purpose iterators.
AutNumPeeringIterator:

	#include "irr/autnum.hh"

  AutNumPeeringIterator itr(o); // o is const AutNum *
  for (const Peering *peering = itr.first(); itr; peering = itr.next()) {
     cout << "AS" << peering->peerAS;
     if (!peering->peerIP.isNull())
	cout << " " << peering->peerIP;
     if (!peering->localIP.isNull()) 
	cout << " at " << peering->localIP;
     cout << "\n";
  }

or the ++ method of iteration

	for (AutNumPeeringIterator itr(o); itr; itr++) {
		cout << itr()->peerAS;
		...
	}

This will work, even if the peering specification is complicated, e.g.
	from AS-foo and not AS1 or AS3
		1.1.1.1 or 2.2.2.2 or rtrs-foo and 3.3.3.3
		at 5.5.5.5 or 6.6.6.6

It will evaluate these expression and return, one by one, (peerAS, peerIP,
localIP) triplets. Note that peerIP and localIP can be NULL.
