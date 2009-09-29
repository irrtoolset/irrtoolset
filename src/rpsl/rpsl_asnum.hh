#ifndef ASNUM_HH
#define ASNUM_HH 1

extern bool opt_asdot;

inline int asnum_string(char *buf, unsigned int asno)
{
   if (asno > 65535 && opt_asdot)
      return sprintf(buf, "AS%d.%d", asno>>16, asno&0xffff);
   else
      return sprintf(buf, "AS%d", asno);
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

#endif

