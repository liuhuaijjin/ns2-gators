/*
Copyright (c) 1997, 1998 Carnegie Mellon University.  All Rights
Reserved. 

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The AODV code developed by the CMU/MONARCH group was optimized and tuned by Samir Das and Mahesh Marina, University of Cincinnati. The work was partially done in Sun Microsystems.

*/

#ifndef __aorglu_h__
#define __aorglu_h__

//#include <agent.h>
//#include <packet.h>
//#include <sys/types.h>
//#include <cmu/list.h>
//#include <scheduler.h>

#include <cmu-trace.h>
#include <priqueue.h>
#include <aorglu/aorglu_rtable.h>
#include <aorglu/aorglu_rqueue.h>
#include <classifier/classifier-port.h>

/*RGK - Include location cache header*/
#include <aorglu/aorglu_loctable.h>

#define RERR_REASON_SEND 1

/*
  Allows local repair of routes 
*/
#define AORGLU_LOCAL_REPAIR

/*
  Allows AORGLU to use link-layer (802.11) feedback in determining when
  links are up/down.
*/
//#define AORGLU_LINK_LAYER_DETECTION

/*
  Causes AORGLU to apply a "smoothing" function to the link layer feedback
  that is generated by 802.11.  In essence, it requires that RT_MAX_ERROR
  errors occurs within a window of RT_MAX_ERROR_TIME before the link
  is considered bad.
*/
#define AORGLU_USE_LL_METRIC

/*
  Only applies if AORGLU_USE_LL_METRIC is defined.
  Causes AORGLU to apply omniscient knowledge to the feedback received
  from 802.11.  This may be flawed, because it does not account for
  congestion.
*/
//#define AORGLU_USE_GOD_FEEDBACK


class AORGLU;

#define MY_ROUTE_TIMEOUT        10                      	// 100 seconds
#define ACTIVE_ROUTE_TIMEOUT    10				// 50 seconds
#define REV_ROUTE_LIFE          6				// 5  seconds
#define BCAST_ID_SAVE           6				// 3 seconds

#define REPA_RETX_TIME          0.8
#define LOC_EXP_TIME            3

/*RGK - LOC_CACHE_EXP Time (How long a cache entry lives. 0 disables)*/
#define LOC_CACHE_EXP -1 

/*RGK - LUDP_CACHE_SAVE (How long an entry in the LUDP cache lasts.*/
#define LUDP_CACHE_SAVE       30

// No. of times to do network-wide search before timing out for 
// MAX_RREQ_TIMEOUT sec. 
#define RREQ_RETRIES            3  

// timeout after doing network-wide search RREQ_RETRIES times
#define MAX_RREQ_TIMEOUT	10.0 //sec

/* Various constants used for the expanding ring search */
#define TTL_START     5
#define TTL_THRESHOLD 7
#define TTL_INCREMENT 2 

// This should be somewhat related to arp timeout
#define NODE_TRAVERSAL_TIME     0.03             // 30 ms
#define LOCAL_REPAIR_WAIT_TIME  0.15 //sec

// Should be set by the user using best guess (conservative) 
#define NETWORK_DIAMETER        30             // 30 hops

// Must be larger than the time difference between a node propagates a route 
// request and gets the route reply back.

//#define RREP_WAIT_TIME     (3 * NODE_TRAVERSAL_TIME * NETWORK_DIAMETER) // ms
//#define RREP_WAIT_TIME     (2 * REV_ROUTE_LIFE)  // seconds
#define RREP_WAIT_TIME         1.0  // sec

#define ID_NOT_FOUND    0x00
#define ID_FOUND        0x01
//#define INFINITY        0xff

// The followings are used for the forward() function. Controls pacing.
#define DELAY 1.0           // random delay
#define NO_DELAY -1.0       // no delay 

// think it should be 30 ms
#define ARP_DELAY 0.01      // fixed delay to keep arp happy


#define HELLO_INTERVAL          0.5               // 1000 ms
#define ALLOWED_HELLO_LOSS      3               // packets
#define BAD_LINK_LIFETIME       3               // 3000 ms
#define LUDP_INTERVAL		5		// 5000 ms - csh
#define LUDP_RADIUS             50              // 50 units - rgk
#define MaxHelloInterval        (0.85 * HELLO_INTERVAL)
#define MinHelloInterval        (0.75 * HELLO_INTERVAL)

/*
  Timers (Broadcast ID, Hello, Neighbor Cache, Route Cache)
*/

/*RGK - Location Cache Timer*/
class AORGLULocationCacheTimer : public Handler
{
  public:
	AORGLULocationCacheTimer(AORGLU* a) : agent(a) {}
        void handle(Event*);
  private:
	AORGLU *agent;
	Event intr;
};

class AORGLUBroadcastTimer : public Handler {
public:
        AORGLUBroadcastTimer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

/*RGK - LUDPCacheTimer*/
class AORGLULUDPCacheTimer : public Handler
{
  public:
	AORGLULUDPCacheTimer(AORGLU *a) : agent(a) {}
	void  handle(Event*);
  private:
	AORGLU *agent;
	Event intr;
};




//csh - added Location Update Timer prototype
class AORGLULocationUpdateTimer : public Handler {
public:
        AORGLULocationUpdateTimer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

class AORGLUHelloTimer : public Handler {
public:
        AORGLUHelloTimer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

class AORGLUNeighborTimer : public Handler {
public:
        AORGLUNeighborTimer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

class AORGLURouteCacheTimer : public Handler {
public:
        AORGLURouteCacheTimer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};


/*Route Maintenance Timers*/

class AORGLU_REPA_RETX_Timer : public Handler {
public:
        friend class AORGLU;
        AORGLU_REPA_RETX_Timer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

class AORGLU_LOC_EXP_Timer : public Handler {
       friend class AORGLU;
public:
        AORGLU_LOC_EXP_Timer(AORGLU* a) : agent(a) {}
        void	handle(Event*);
private:
        AORGLU    *agent;
	Event	intr;
};

//Deleted LocalRepairTimer


/*
  Broadcast ID Cache
*/
class BroadcastID {
        friend class AORGLU;
 public:
        BroadcastID(nsaddr_t i, u_int32_t b) { src = i; id = b;  }
 protected:
        LIST_ENTRY(BroadcastID) link;
        nsaddr_t        src;
        u_int32_t       id;
        double          expire;         // now + BCAST_ID_SAVE s
};

LIST_HEAD(aorglu_bcache, BroadcastID);

/*
  RGK - LUDP Cache
  This cache keeps track of nodes to which have been
  recently communicated.
*/
class LUDPCacheEntry {
        friend class AORGLULocationUpdateTimer;
	friend class AORGLU;
  public:
	LUDPCacheEntry(nsaddr_t id) { dst = id; }
  private: 
	LIST_ENTRY(LUDPCacheEntry) lclink;
	nsaddr_t dst;
	bool active; /*Checks if this node has been active*/ 
  	double expire;
};

LIST_HEAD(aorglu_ludpcache, LUDPCacheEntry);

class RouteFailEvent : public Event
{
  friend class AORGLU_LOC_EXP_Timer;
  friend class AORGLU_REPA_RETX_Timer;
  public:
   RouteFailEvent(aorglu_rt_entry *rt) { this->rt = rt; }
  private:
  aorglu_rt_entry *rt;
};

/*
  The Routing Agent
*/
class AORGLU: public Agent {

  /*
   * make some friends first 
   */

        friend class aorglu_rt_entry;
	friend class aorglu_loctable;

        /*Maintenance Timers*/
        friend class AORGLU_REPA_RETX_Timer;
        friend class AORGLU_LOC_EXP_Timer;

	/*RGK*/
        friend class AORGLULocationCacheTimer;
	friend class AORGLULUDPCacheTimer;

        friend class AORGLUBroadcastTimer;
	friend class AORGLULocationUpdateTimer; //csh - friend class
        friend class AORGLUHelloTimer;
        friend class AORGLUNeighborTimer;
        friend class AORGLURouteCacheTimer;

 public:
        AORGLU(nsaddr_t id);

        void		recv(Packet *p, Handler *);

 protected:
        int             command(int, const char *const *);
        int             initialized() { return 1 && target_; }

        /*
         * Route Table Management
         */
        void            rt_resolve(Packet *p);
        void            rt_update(aorglu_rt_entry *rt, u_int32_t seqnum,
		     	  	u_int16_t metric, nsaddr_t nexthop,
		      		double expire_time);
        void            rt_down(aorglu_rt_entry *rt);
 public:
        void            rt_ll_failed(Packet *p);
 protected:
        
        void            rt_purge(void);

        void            enque(aorglu_rt_entry *rt, Packet *p);
        Packet*         deque(aorglu_rt_entry *rt);

       /* RGK - Location Management*/
        void            loc_purge(void);

        void            init_route_maintenance(aorglu_rt_entry *rt, aorglu_loc_entry *le);  

        /*
         * Neighbor Management
         */
        void            nb_insert(nsaddr_t id);
        AORGLU_Neighbor*       nb_lookup(nsaddr_t id);
        void            nb_delete(nsaddr_t id);
        void            nb_purge(void);

       /*
        * RGK -  LUDP Cache Management
        */
        void		ludpcache_insert(nsaddr_t id);
	LUDPCacheEntry*	ludpcache_lookup(nsaddr_t id);
	void		ludpcache_checkactive(void);
        void            ludpcache_update(nsaddr_t id); 
        void            ludpcache_delete(nsaddr_t id);

        /*
         * Broadcast ID Management
         */

        void            id_insert(nsaddr_t id, u_int32_t bid);
        bool	        id_lookup(nsaddr_t id, u_int32_t bid);
        void            id_purge(void);

        /*
         * Packet TX Routines
         */
       
        //Route Maintenance 
        void            handle_link_failure(nsaddr_t id);
        void            sendRepa(nsaddr_t dst, nsaddr_t next);
        void            forwardRepa(Packet *p, nsaddr_t next);

	void            forwardRepc(Packet *p);

        void            forward(aorglu_rt_entry *rt, Packet *p, double delay);
        void            sendHello(void);
        void            sendRequest(nsaddr_t dst);
      
	void            sendReply(nsaddr_t ipdst, u_int32_t hop_count,
                                  nsaddr_t rpdst, u_int32_t rpseq,
                                  u_int32_t lifetime, double timestamp);
        void            sendError(nsaddr_t dst, nsaddr_t unreachable);
                                          
        /*
         * Packet RX Routines
         */

        //Route Maintenance
        void            recvRepa(Packet *p);

        void            recvAORGLU(Packet *p);
        void            recvHello(Packet *p);
        void            recvRequest(Packet *p);
        void            recvReply(Packet *p);
        void            recvError(Packet *p);
	

        /*
         * csh - LUDP functions
         */
	void		sendLudp(nsaddr_t ipdst);
	void		recvLudp(Packet *p);

	/*rgk - coordinates of last ludp update*/
        double lastX_;
	double lastY_;
	double lastZ_;

	/*
	 * History management
	 */
	
	double 		PerHopTime(aorglu_rt_entry *rt);


        nsaddr_t        index;                  // IP Address of this node
        u_int32_t       seqno;                  // Sequence Number
        int             bid;                    // Broadcast ID

        aorglu_rtable         rthead;                 // routing table
        aorglu_ncache         nbhead;                 // Neighbor Cache
        aorglu_bcache          bihead;                 // Broadcast ID Cache

	/*RGK - LUDP Cache Head*/
        aorglu_ludpcache       lchead; 	//LUDP Cache Head

        /*
         * Timers
         */


        AORGLU_REPA_RETX_Timer    reparetxtimer;
        AORGLU_LOC_EXP_Timer      locexptimer;


        /*RGK*/
        AORGLULocationCacheTimer loctimer;
	AORGLULUDPCacheTimer cctimer;

        //csh - instance of LUDP timer
	AORGLULocationUpdateTimer	lutimer; 

        AORGLUBroadcastTimer  btimer;
        AORGLUHelloTimer      htimer;
        AORGLUNeighborTimer   ntimer;
        AORGLURouteCacheTimer rtimer;

	/*RGK - Location Table*/
	aorglu_loctable	      loctable;

        /*
         * Routing Table
         */
        aorglu_rtable          rtable;
        /*
         *  A "drop-front" queue used by the routing layer to buffer
         *  packets to which it does not have a route.
         */
        aorglu_rqueue         rqueue;

        /*
         * A mechanism for logging the contents of the routing
         * table.
         */
        Trace           *logtarget;

        /*
         * A pointer to the network interface queue that sits
         * between the "classifier" and the "link layer".
         */
        PriQueue        *ifqueue;

        /*
         * Logging stuff
         */
        void            log_link_del(nsaddr_t dst);
        void            log_link_broke(Packet *p);
        void            log_link_kept(nsaddr_t dst);

	/* for passing packets up to agents */
	PortClassifier *dmux_;

};

#endif /* __aorglu_h__ */
