//
// $Id$
//
// Timer.hh
// Author: Ramesh Govindan <govindan@isi.edu>
//
// One-shot interval timers

#ifndef _Timer_hh
#define _Timer_hh

#include "util/List.hh"
#include "util/Handler.hh"
#include "sys/Time.hh"

// Classes defined elsewhere
// class TimeLong;

// One-shot timers. Calls back handler function upon expiration.
// To have them fire periodically, call fireIn or fireAt repeatedly.

class Timer : public ListNode {

  public:
    // Create a new timer 
    Timer(const Handler&,		// In: Handler to call back after expiry
          const TimeLong&);		// In: Absolute time of expiry

    // Destroy timer, also dequeues timer from any queue its in
    virtual ~Timer();

    bool
    operator<(const Timer&) const;

    bool
    operator==(const Timer&) const;

    bool pending() { return pending_; }
    const TimeLong& time() const { return expireAt; }
    virtual void handle() { 
	    handler.callBack((void*) this);
	    delete this;
    }
protected:
    TimeLong		expireAt;
    Handler		handler;
    bool                pending_;
};

class ITimer : public Timer {
public:
	ITimer(const Handler& h,
	       const TimeShort& ival);
	void handle();
	void restart();
	void stop();
	const TimeShort& interval() const { return ival_; }
protected:
	TimeShort ival_;
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

#endif // _Timer_hh
