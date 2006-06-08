#!/nfs/filb2/public/bin/perl
#
#  abridge_gated:  chop some lines from gated files to make them readable,
#   when the net lists are not what you're worried about at the moment.
#
#   Usage:  abridge_gated nss<xxx>.t3p
#
#  All this does is chop off any list containing the world "masklen" or
#   "peer" the first three entries, and insert an elipese to warn you of it.
#   But it makes multi-meg files readable.  For instance, it chops ENSS136's
#   config file 98%, from 100,000 lines down to 1,500.
#
#     gated.nss128.t3p    2012 lines -> 210 lines (90% reduction)
#     gated.nss136.t3p  105690 lines -> 893 lines (99% reduction)
#
#  $Id$

while (<>)
        {
        if ( /peer/ )
                {
                print if ($peer_count++ < 3 );
                }
        elsif ( /masklen/ )
                {
                print if ($masklen_count++ < 3 );
                }
        elsif ( /^\s*$/ )
                {
                print unless ($last_was_space++);
                }
        else
                {
                print "\t...........[", $peer_count-3, "]...............\n" if ($peer_count);
                print "\t...........[", $masklen_count-3, "]...............\n" if ($masklen_count);
                print;
                $masklen_count = 0;
                $peer_count = 0;
                $last_was_space = 0;
                }
        }
