//
// $Id$
//
// Job.hh
// Author: Ramesh Govindan <govindan@isi.edu>
//
// Background job. Use for small, asynchronous computations.

#ifndef _Job_hh
#define _Job_hh

#include "util/List.hh"
#include "util/Handler.hh"

// Background tasks. Run to completion, and scheduled when the
// dispatcher has nothing else to do. Care must be taken in
// the design of background tasks, since these are non-preemptible.

class Job : public ListNode {
public:
    // Create new job
    Job(const Handler&);			// In: Function to execute
    // Destroy job, removes us from any queue we're on
    virtual ~Job();
    // calls the handler
    virtual void handle() {
	handler.callBack((void*) this);
    }
    bool persistent;
private:
    Handler		handler;
};

//
//  Copyright (c) 1994 by the University of Southern California.
//  All rights reserved.
//
//  Permission to use, copy, modify, and distribute this software and
//  its documentation in source and binary forms for lawful
//  non-commercial purposes and without fee is hereby granted, provided
//  that the above copyright notice appear in all copies and that both
//  the copyright notice and this permission notice appear in supporting
//  documentation, and that any documentation, advertising materials,
//  and other materials related to such distribution and use acknowledge
//  that the software was developed by the University of Southern
//  California and/or Information Sciences Institute.
//  The name of the University of Southern California may not
//  be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY REPRESENTATIONS
//  ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
//  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
//  NON-INFRINGEMENT.
//
//  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
//  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
//  THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  info-ra@isi.edu.
//

// Local Variables:
// outline-regexp: "^[a-zA-Z0-9\*]+$\n[a-zA-Z0-9]+[:][:]\\|^inline [a-zA-Z0-9\*]+\n[a-zA-Z0-9]+[:][:]\\|[a-zA-Z0-9]+[:][:]\\|^struct\\|^class"
// outline-heading-end-regexp: "\{\n"
// End:

#endif // _Job_hh
