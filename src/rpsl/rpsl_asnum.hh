#ifndef ASNUM_HH
#define ASNUM_HH 1

#ifdef DTAG
extern bool opt_asn16;
#endif /* DTAG */
extern bool opt_asdot;

inline int asnum_string(char *buf, unsigned int asno)
{
#ifdef DTAG
   if (asno > 65535) {
      if (opt_asn16)
         return sprintf(buf, "AS23456");
      if (opt_asdot)
         return sprintf(buf, "AS%d.%d", asno>>16, asno&0xffff);
   }
   return sprintf(buf, "AS%d", asno);
#else
   if (asno > 65535 && opt_asdot)
      return sprintf(buf, "AS%d.%d", asno>>16, asno&0xffff);
   else
      return sprintf(buf, "AS%d", asno);
#endif /* DTAG */
}

inline int asnum_string_dot(char *buf, unsigned int asno)
{
   if (asno > 65535)
      return sprintf(buf, "AS%d.%d", asno>>16, asno&0xffff);
   else
      return sprintf(buf, "AS%d", asno);
}

inline int asnum_string_plain(char *buf, unsigned int asno)
{
   return sprintf(buf, "AS%d", asno);
}

#ifdef DTAG
inline int asnum_string_asn16(char *buf, unsigned int asno)
{
   if (asno > 65535)
      return sprintf(buf, "AS23456");
   else
      return sprintf(buf, "AS%d", asno);
}
#endif /* DTAG */

#endif /* ASNUM_HH */

