#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
   long long int      i;
   double             real;
   char              *string;
   void              *ptr;
   time_t             time;
   Item              *item;
   ItemList          *list;
   regexp            *re;
   SymID              sid;
   IPAddr            *ip;
   Prefix            *prfx;
   PrefixRange       *prfxrng;
   IPv6Addr          *ipv6;
   IPv6Prefix        *prfxv6;
   IPv6PrefixRange   *prfxv6rng;
   AddressFamily     *afi;
   RPType            *typenode;
   MPPrefix          *mpprefix;
   Tunnel            *tunnel;

   Filter                    *filter;
   FilterMS                  *moreSpecOp;
   PolicyPeering             *peering;
   PolicyPeering             *mp_peering;
   PolicyActionList          *actionList;
   PolicyAction              *actionNode;
   PolicyFactor              *policyFactor;
   PolicyTerm                *policyTerm;
   PolicyExpr                *policyExpr;
   List<PolicyPeeringAction> *peeringActionList;

   Attr               *attr;
   AttrAttr           *rpslattr;
   AttrMethod         *method;
   const AttrRPAttr   *rp_attr;
   const AttrProtocol *protocol;
   AttrProtocolOption *protocol_option;
   List<AttrProtocolOption> *protocol_option_list;

   AttrPeerOption       *peer_option;
   List<AttrPeerOption> *peer_option_list;

   List<RPTypeNode> *typelist;
   List<AttrMethod> *methodlist;
   List<WordNode>   *wordlist;

   List<AttrMntRoutes::MntPrfxPair> *listMntPrfxPair;
   AttrMntRoutes::MntPrfxPair       *mntPrfxPair;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	KEYW_TRUE	257
# define	KEYW_FALSE	258
# define	KEYW_ACTION	259
# define	KEYW_ACCEPT	260
# define	KEYW_ANNOUNCE	261
# define	KEYW_FROM	262
# define	KEYW_TO	263
# define	KEYW_AT	264
# define	KEYW_ANY	265
# define	KEYW_REFINE	266
# define	KEYW_EXCEPT	267
# define	KEYW_STATIC	268
# define	KEYW_NETWORKS	269
# define	KEYW_MASKLEN	270
# define	KEYW_UNION	271
# define	KEYW_RANGE	272
# define	KEYW_LIST	273
# define	KEYW_OF	274
# define	KEYW_OPERATOR	275
# define	KEYW_SYNTAX	276
# define	KEYW_SPECIAL	277
# define	KEYW_REGEXP	278
# define	KEYW_OPTIONAL	279
# define	KEYW_MANDATORY	280
# define	KEYW_INTERNAL	281
# define	KEYW_SINGLEVALUED	282
# define	KEYW_MULTIVALUED	283
# define	KEYW_LOOKUP	284
# define	KEYW_KEY	285
# define	KEYW_DELETED	286
# define	KEYW_OBSOLETE	287
# define	KEYW_PEERAS	288
# define	KEYW_PROTOCOL	289
# define	KEYW_INTO	290
# define	KEYW_ATOMIC	291
# define	KEYW_INBOUND	292
# define	KEYW_OUTBOUND	293
# define	KEYW_UPON	294
# define	KEYW_HAVE_COMPONENTS	295
# define	KEYW_EXCLUDE	296
# define	KEYW_AFI	297
# define	KEYW_TUNNEL	298
# define	TKN_ERROR	299
# define	TKN_UNKNOWN_CLASS	300
# define	TKN_EOA	301
# define	TKN_EOO	302
# define	TKN_FREETEXT	303
# define	TKN_INT	304
# define	TKN_REAL	305
# define	TKN_STRING	306
# define	TKN_TIMESTAMP	307
# define	TKN_BLOB	308
# define	TKN_IPV4	309
# define	TKN_PRFXV4	310
# define	TKN_PRFXV4RNG	311
# define	TKN_IPV6	312
# define	TKN_PRFXV6	313
# define	TKN_PRFXV6RNG	314
# define	TKN_ASNO	315
# define	TKN_ASNAME	316
# define	TKN_RSNAME	317
# define	TKN_RTRSNAME	318
# define	TKN_PRNGNAME	319
# define	TKN_FLTRNAME	320
# define	TKN_BOOLEAN	321
# define	TKN_WORD	322
# define	TKN_RP_ATTR	323
# define	TKN_DNS	324
# define	TKN_EMAIL	325
# define	TKN_3DOTS	326
# define	TKN_AFI	327
# define	ATTR_GENERIC	328
# define	ATTR_BLOBS	329
# define	ATTR_REGEXP	330
# define	ATTR_IMPORT	331
# define	ATTR_MP_IMPORT	332
# define	ATTR_EXPORT	333
# define	ATTR_MP_EXPORT	334
# define	ATTR_DEFAULT	335
# define	ATTR_MP_DEFAULT	336
# define	ATTR_FREETEXT	337
# define	ATTR_CHANGED	338
# define	ATTR_IFADDR	339
# define	ATTR_INTERFACE	340
# define	ATTR_PEER	341
# define	ATTR_MP_PEER	342
# define	ATTR_INJECT	343
# define	ATTR_V6_INJECT	344
# define	ATTR_COMPONENTS	345
# define	ATTR_V6_COMPONENTS	346
# define	ATTR_AGGR_MTD	347
# define	ATTR_AGGR_BNDRY	348
# define	ATTR_RS_MEMBERS	349
# define	ATTR_RS_MP_MEMBERS	350
# define	ATTR_RTR_MP_MEMBERS	351
# define	ATTR_RP_ATTR	352
# define	ATTR_TYPEDEF	353
# define	ATTR_PROTOCOL	354
# define	ATTR_FILTER	355
# define	ATTR_V6_FILTER	356
# define	ATTR_MP_FILTER	357
# define	ATTR_MP_PEVAL	358
# define	ATTR_PEERING	359
# define	ATTR_MP_PEERING	360
# define	ATTR_ATTR	361
# define	ATTR_MNT_ROUTES	362
# define	ATTR_MNT_ROUTES6	363
# define	ATTR_MNT_ROUTES_MP	364
# define	OP_OR	365
# define	OP_AND	366
# define	OP_NOT	367
# define	OP_MS	368
# define	TKN_OPERATOR	369


extern YYSTYPE rpsllval;

#endif /* not BISON_Y_TAB_H */
