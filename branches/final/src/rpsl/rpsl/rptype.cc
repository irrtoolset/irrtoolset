//  $Id$
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and its
//  documentation in source and binary forms for lawful non-commercial
//  purposes and without fee is hereby granted, provided that the above
//  copyright notice appear in all copies and that both the copyright
//  notice and this permission notice appear in supporting documentation,
//  and that any documentation, advertising materials, and other materials
//  related to such distribution and use acknowledge that the software was
//  developed by the University of Southern California, Information
//  Sciences Institute. The name of the USC may not be used to endorse or
//  promote products derived from this software without specific prior
//  written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY
//  REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY
//  PURPOSE.  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
//  TITLE, AND NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT, TORT,
//  OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH, THE USE
//  OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  ratoolset@isi.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

#include "config.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <typeinfo>
#include "util/List.hh"

class Attribute;
#include "rptype.hh"
#include "schema.hh"

extern "C" {
#ifndef HAVE_STRCASECMP_DEFINITION
int strcasecmp(...);
#endif // HAVE_STRCASECMP_DEFINITION
}

////////////////////////////// RPType ////////////////////

RPType *RPType::newRPType(char *name) {
   if (!strcasecmp(name, "integer"))
      return new RPTypeInt;
   if (!strcasecmp(name, "date"))
      return new RPTypeDate;
   if (!strcasecmp(name, "real"))
      return new RPTypeReal;
   if (!strcasecmp(name, "rpsl_word"))
      return new RPTypeWord;
   //   if (!strcasecmp(name, "blob"))
   //      return new RPTypeBlob;
   if (!strcasecmp(name, "as_number"))
      return new RPTypeASNumber;
   if (!strcasecmp(name, "ipv4_address"))
      return new RPTypeIPv4Address;
   if (!strcasecmp(name, "address_prefix"))
      return new RPTypeIPv4Prefix;
   if (!strcasecmp(name, "address_prefix_range"))
      return new RPTypeIPv4PrefixRange;
   if (!strcasecmp(name, "connection"))
      return new RPTypeConnection;
   if (!strcasecmp(name, "dns_name"))
      return new RPTypeDNSName;
   if (!strcasecmp(name, "filter"))
      return new RPTypeFilter;
   if (!strcasecmp(name, "as_set_name"))
      return new RPTypeASName;
   if (!strcasecmp(name, "route_set_name"))
      return new RPTypeRSName;
   if (!strcasecmp(name, "rtr_set_name"))
      return new RPTypeRtrsName;
   if (!strcasecmp(name, "peering_set_name"))
      return new RPTypePrngName;
   if (!strcasecmp(name, "filter_set_name"))
      return new RPTypeFltrName;
   if (!strcasecmp(name, "string"))
      return new RPTypeString;
   if (!strcasecmp(name, "time_stamp"))
      return new RPTypeTimeStamp;
   if (!strcasecmp(name, "boolean"))
      return new RPTypeBoolean;
   if (!strcasecmp(name, "free_text"))
      return new RPTypeFreeText;
   if (!strcasecmp(name, "email"))
      return new RPTypeEMail;

   return schema.searchTypedef(name)->dup();
}

RPType *RPType::newRPType(char *name, long long int min, long long int max) {
   if (!strcasecmp(name, "integer"))
      return new RPTypeInt(min, max);
   if (!strcasecmp(name, "real"))
      return new RPTypeReal(min, max);

   return NULL;
}

RPType *RPType::newRPType(char *name, double min, double max) {
   if (!strcasecmp(name, "real"))
      return new RPTypeReal(min, max);

   return NULL;
}

RPType *RPType::newRPType(char *name, List<WordNode> *words) {
   if (!strcasecmp(name, "enum"))
      return new RPTypeEnum(words);

   return NULL;
}

RPType *RPType::newRPType(char *name, List<RPTypeNode> *types) {
   if (types && !types->isEmpty())
      if (!strcasecmp(name, "union"))
	 return new RPTypeUnion(types);

   return NULL;
}

bool RPType::validate(const Item *item) const {
   return false;
}
Item *RPType::typeCast(const Item *item) const {
   return NULL;
}
bool RPType::validate(ItemList *list) const {
   if (! list->isSingleton())
      return false;
   if (validate(list->head()))
      return true;
   
   Item *w = typeCast(list->head());
   if (!w)
      return false;
   
   list->clear();
   list->append(w);

   return true;
}

////////////////////////////// RPTypeInt ////////////////////

bool RPTypeInt::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemINT) 
	   && min <= ((ItemINT *) item)->i
	   && max >= ((ItemINT *) item)->i);
}

////////////////////////////// RPTypeDate ////////////////////

bool RPTypeDate::validate(const Item *item) const {
   static int days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   if (typeid(*item) != typeid(ItemINT))
      return false;

   int year = ((ItemINT *) item)->i / 10000;
   int month = (((ItemINT *) item)->i % 10000) / 100;
   int day = ((ItemINT *) item)->i % 100;
   return (year >= 1990 
	   && month >= 01 && month <= 12
	   && day >= 01 && day <= days[month - 1]);
}

////////////////////////////// RPTypeReal ////////////////////

bool RPTypeReal::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemREAL) 
	   && min <= ((ItemREAL *) item)->real
	   && max >= ((ItemREAL *) item)->real);
}

Item *RPTypeReal::typeCast(const Item  *item) const {
   if (typeid(*item) == typeid(ItemINT)  
       && min <= ((ItemINT *) item)->i
       && max >= ((ItemINT *) item)->i)
      return new ItemREAL(((ItemINT *) item)->i);

   return NULL;
}

////////////////////////////// RPTypeString ////////////////////

bool RPTypeString::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemSTRING));
}

////////////////////////////// RPTypeTimeStamp ////////////////////

bool RPTypeTimeStamp::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemTimeStamp));
}

////////////////////////////// RPTypeWORD ////////////////////

bool RPTypeWord::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemWORD));
}
Item *RPTypeWord::typeCast(const Item  *item) const {
   if (typeid(*item) == typeid(ItemASNAME))
      return new ItemWORD(strdup(((ItemASNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRSNAME))
      return new ItemWORD(strdup(((ItemRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRTRSNAME))
      return new ItemWORD(strdup(((ItemRTRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemPRNGNAME))
      return new ItemWORD(strdup(((ItemPRNGNAME *) item)->name));
   if (typeid(*item) == typeid(ItemFLTRNAME))
      return new ItemWORD(strdup(((ItemFLTRNAME *) item)->name));
   if (typeid(*item) == typeid(ItemBOOLEAN))
      if (((ItemBOOLEAN *) item)->i)
	 return new ItemWORD(strdup("true"));
      else
	 return new ItemWORD(strdup("false"));
   if (typeid(*item) == typeid(ItemASNO)) {
      char buffer[64];
      sprintf(buffer, "AS%d", ((ItemASNO *) item)->asno);
      return new ItemWORD(strdup(buffer));
   }

   return NULL;
}

////////////////////////////// RPTypeBlob ////////////////////

bool RPTypeBlob::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemBLOB));
}

Item *RPTypeBlob::typeCast(const Item  *item) const {
   if (typeid(*item) == typeid(ItemASNAME))
      return new ItemBLOB(strdup(((ItemASNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRSNAME))
      return new ItemBLOB(strdup(((ItemRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRTRSNAME))
      return new ItemBLOB(strdup(((ItemRTRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemPRNGNAME))
      return new ItemBLOB(strdup(((ItemPRNGNAME *) item)->name));
   if (typeid(*item) == typeid(ItemFLTRNAME))
      return new ItemBLOB(strdup(((ItemFLTRNAME *) item)->name));
   if (typeid(*item) == typeid(ItemBOOLEAN))
      if (((ItemBOOLEAN *) item)->i)
	 return new ItemBLOB(strdup("true"));
      else
	 return new ItemBLOB(strdup("false"));
   if (typeid(*item) == typeid(ItemASNO)) {
      char buffer[64];
      sprintf(buffer, "AS%d", ((ItemASNO *) item)->asno);
      return new ItemWORD(strdup(buffer));
   }

   return NULL;
}

////////////////////////////// RPTypeASName ////////////////////

bool RPTypeASName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemASNAME));
}

////////////////////////////// RPTypeRSName ////////////////////

bool RPTypeRSName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemRSNAME));
}

////////////////////////////// RPTypeRTRSName ////////////////////

bool RPTypeRtrsName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemRTRSNAME));
}

////////////////////////////// RPTypePrngName ////////////////////

bool RPTypePrngName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemPRNGNAME));
}

////////////////////////////// RPTypeFltrName ////////////////////

bool RPTypeFltrName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemFLTRNAME));
}

////////////////////////////// RPTypeEMail ////////////////////

bool RPTypeEMail::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemEMAIL));
}

////////////////////////////// RPTypeFreeText ////////////////////

bool RPTypeFreeText::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemFREETEXT));
}

////////////////////////////// RPTypeEnum ////////////////////

bool RPTypeEnum::validate(const Item *item) const {
   if (typeid(*item) != typeid(ItemWORD))
	 return false;
   
   for (WordNode* p = words->head(); p; p = words->next(p))
	 if (!strcasecmp(((ItemWORD *) item)->word, p->word))
	    return true;

   return false;
}

Item *RPTypeEnum::typeCast(const Item  *item) const {
   ItemWORD *w = NULL;

   if (typeid(*item) == typeid(ItemASNAME))
      w = new ItemWORD(strdup(((ItemASNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRSNAME))
      w = new ItemWORD(strdup(((ItemRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRTRSNAME))
      w = new ItemWORD(strdup(((ItemRTRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemPRNGNAME))
      w = new ItemWORD(strdup(((ItemPRNGNAME *) item)->name));
   if (typeid(*item) == typeid(ItemFLTRNAME))
      w = new ItemWORD(strdup(((ItemFLTRNAME *) item)->name));
   if (typeid(*item) == typeid(ItemBOOLEAN))
      if (((ItemBOOLEAN *) item)->i)
	 w = new ItemWORD(strdup("true"));
      else
	 w = new ItemWORD(strdup("false"));
   if (typeid(*item) == typeid(ItemASNO)) {
      char buffer[64];
      sprintf(buffer, "AS%d", ((ItemASNO *) item)->asno);
      w = new ItemWORD(strdup(buffer));
   }

   if (w && validate(w))
      return w;
   if (w)
      delete w;

   return NULL;
}

////////////////////////////// RPTypeBoolean ////////////////////

bool RPTypeBoolean::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemBOOLEAN));
}

////////////////////////////// RPTypeASNumber ////////////////////

bool RPTypeASNumber::validate(const Item *item) const {
   return ((typeid(*item) == typeid(ItemASNO))
	   && ((ItemASNO *) item)->asno <= 65535);
}

////////////////////////////// RPTypeIPv4Address ////////////////////

bool RPTypeIPv4Address::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemIPV4));
}

////////////////////////////// RPTypeIPv4Prefix ////////////////////

bool RPTypeIPv4Prefix::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemPRFXV4));
}

////////////////////////////// RPTypeIPv4PrefixRange ////////////////////

bool RPTypeIPv4PrefixRange::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemPRFXV4Range));
}

Item *RPTypeIPv4PrefixRange::typeCast(const Item *item) const {
   if (typeid(*item) == typeid(ItemPRFXV4))
      return new ItemPRFXV4Range
	 (new PrefixRange(*((ItemPRFXV4 *) item)->prfxv4));
   return NULL;
}

////////////////////////////// RPTypeConnection ////////////////////

bool RPTypeConnection::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemConnection));
}

////////////////////////////// RPTypeDNSName ////////////////////

bool RPTypeDNSName::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemDNS));
}

////////////////////////////// RPTypeFilter ////////////////////

bool RPTypeFilter::validate(const Item *item) const {
   return (typeid(*item) == typeid(ItemFilter));
}

Item *RPTypeFilter::typeCast(const Item  *item) const {
   if (typeid(*item) == typeid(ItemASNAME))
      return new ItemFilter(new FilterASNAME(((ItemASNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRSNAME))
      return new ItemFilter(new FilterRSNAME(((ItemRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemRTRSNAME))
      return new ItemFilter(new FilterRTRSNAME(((ItemRTRSNAME *) item)->name));
   if (typeid(*item) == typeid(ItemFLTRNAME))
      return new ItemFilter(new FilterFLTRNAME(((ItemFLTRNAME *) item)->name));
   if (typeid(*item) == typeid(ItemASNO))
      return new ItemFilter(new FilterASNO(((ItemASNO *) item)->asno));

   return NULL;
}

////////////////////////////// RPTypeUnion ////////////////////

bool RPTypeUnion::validate(const Item *item) const {
   for (RPTypeNode* p = types->head(); p; p = types->next(p))
      if (p->type->validate(item))
	 return true;
   
   return false;
}

Item *RPTypeUnion::typeCast(const Item *item) const {
   Item *w;
   for (RPTypeNode* p = types->head(); p; p = types->next(p)) {
      w = p->type->typeCast(item);
      if (w)
	 return w;
   }
   
   return NULL;
}

const char *RPTypeUnion::name() {
   if (!_name) {
      char buffer[1024];
      strcpy(buffer, "union ");
      for (RPTypeNode *n = types->head(); n; ) {
	 strcat(buffer, n->type->name());
	 n = types->next(n);
	 if (n)
	    strcat(buffer, ", ");
      }
      _name = strdup(buffer);
   }   
   return _name;
}

////////////////////////////// RPTypeRange ////////////////////

bool RPTypeRange::validate(const Item *item) const {
   if (typeid(*item) == typeid(ItemRange))
      return type->validate(((ItemRange*) item)->begin)
	 && type->validate(((ItemRange*) item)->end)
	 && (*((ItemRange*) item)->begin) <= (*((ItemRange*) item)->end);
   return false;
}

Item *RPTypeRange::typeCast(const Item *item) const {
   Item *w = NULL;

   if (type->validate(item))
      w = item->dup();
   else
      w = type->typeCast(item);

   if (w)
      return new ItemRange(w, w->dup());
   
   return NULL;
}

////////////////////////////// RPTypeList ////////////////////

bool RPTypeList::validate(const Item *item) const {
   if (typeid(*item) == typeid(ItemList))
      return validate((ItemList*) item);
   return false;
}
bool RPTypeList::validate(ItemList *l) const {
   Item *w;

   if (! (min_elms <= l->size() && l->size() <= max_elms))
      return false;

   for (Item *item = l->head(); item; item = l->next(item))
      if (! type->validate(item)) {
	 w = type->typeCast(item);
	 if (w) {
	    l->insertAfter(item, w);
	    l->remove(item);
	    delete item;
	    item = w;
	 } else
	    return false;
      }

   return true;
}

