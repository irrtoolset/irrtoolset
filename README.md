[![Build Status](https://travis-ci.org/irrtoolset/irrtoolset.svg)](https://travis-ci.org/irrtoolset/irrtoolset)

This is the README file for the IRRToolSet

MAILING LIST
	irrtoolset@isc.org

	To subscribe to the mailing list, visit this page:

	https://lists.isc.org/mailman/listinfo/irrtoolset

	And use the form in the section called "Subscribing to irrtoolset".

FTP and WWW SITES

        ftp://ftp.isc.org/isc/IRRToolSet

        http://irrtoolset.isc.org/
        
        https://github.com/irrtoolset/irrtoolset

INSTALLATION
        0. To install you need the GNU C++ compiler, libtool, bison, flex.

     You will need Bison and Flex if you're going to change the RPSL grammar.

	1. unzip and untar the distribution file
	2. in the irrtoolset directory, type `./configure' to configure the
	   package for your system. If you're using `csh' on an old
	   version of System V, you might need to type `sh ./configure'
	   instead to prevent `csh' from trying to execute `configure'
	   itself. The `--prefix=PREFIXDIR' option specifies where the
	   installation process should put IRRToolSet and its data files.  
	   This defaults to `/usr/local'. 
	3. type "make" to build IRRToolSet.
	4. type "make install" to copy the executables/man-pages to
	   /usr/local/{bin,man} directory (or to the directory 
	   specified with --prefix argument to configure).

Please let us know if you face any problems at the mailing list
irrtoolset@lists.isc.org. Bug reports and suggestions are always welcome.
