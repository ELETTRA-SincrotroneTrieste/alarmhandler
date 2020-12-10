/*----- PROTECTED REGION ID(AlarmHandler.h) ENABLED START -----*/
//=============================================================================
//
// file :        AlarmHandler.h
//
// description : Include file for the AlarmHandler class
//
// project :     Elettra alarm handler device server
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
//
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#ifndef AlarmHandler_H
#define AlarmHandler_H

#include <tango.h>

#include <boost/version.hpp>
#if BOOST_VERSION  >= 103600
#define BOOST_VER_1_36_0 1
#endif

#include "alarm_table.h"
#include "event_table.h"
#include "SubscribeThread.h"

#define MAX_ALARMS	2000
#define MAX_ATTR_NAME	1000
#define MAX_ATTR_SUMMARY	10000

//#define _USE_ELETTRA_DB_RW

//using namespace Tango;

class alarm_thread;
class log_thread;
class update_thread;

#	define	DECLARE_TIME_VAR	struct timeval
#	define	GET_TIME(t)	gettimeofday(&t, NULL);
#	define	ELAPSED(before, after)	\
		1000.0*(after.tv_sec-before.tv_sec) + \
		((double)after.tv_usec-before.tv_usec) / 1000



/*----- PROTECTED REGION END -----*/	//	AlarmHandler.h

/**
 *  AlarmHandler class description:
 *    Elettra alarm handler device server
 */

namespace AlarmHandler_ns
{
enum _AlarmStateEnum {
	_NORM,
	_UNACK,
	_ACKED,
	_RTNUN,
	_SHLVD,
	_DSUPR,
	_OOSRV,
	_ERROR,
} ;
typedef _AlarmStateEnum AlarmStateEnum;

/*----- PROTECTED REGION ID(AlarmHandler::Additional Class Declarations) ENABLED START -----*/

//	Additional Class Declarations

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::Additional Class Declarations

class AlarmHandler : public TANGO_BASE_CLASS
{

/*----- PROTECTED REGION ID(AlarmHandler::Data Members) ENABLED START -----*/

//	Add your own data members
public:
	bool compare_without_domain(string str1, string str2);
	string remove_domain(string str);
	//TODO: real attributes
	Tango::DevLong attr_AttributeStartedNumber_read;
	Tango::DevLong attr_AttributePausedNumber_read;
	Tango::DevLong attr_AttributeStoppedNumber_read;
	Tango::DevLong attr_AttributeNumber_read;

	string normalAlarms_read[MAX_ALARMS];
	string unacknowledgedAlarms_read[MAX_ALARMS];
	string acknowledgedAlarms_read[MAX_ALARMS];
	string unacknowledgedNormalAlarms_read[MAX_ALARMS];
	string shelvedAlarms_read[MAX_ALARMS];
	string outOfServiceAlarms_read[MAX_ALARMS];
	string silencedAlarms_read[MAX_ALARMS];
	string listAlarms_read[MAX_ALARMS];
	string alarmSummary_read[MAX_ALARMS];

/*	char c_normalAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_unacknowledgedAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_acknowledgedAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_unacknowledgedNormalAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_shelvedAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_outOfServiceAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_silencedAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
	char c_listAlarms_read[MAX_ALARMS][MAX_ATTR_NAME];
#define MAX_SUMMARY		2*MAX_ATTR_NAME		//TODO: max summary length!!!
	char c_alarmSummary_read[MAX_ALARMS][MAX_SUMMARY];*/

	size_t normalAlarms_sz;
	size_t unacknowledgedAlarms_sz;
	size_t acknowledgedAlarms_sz;
	size_t unacknowledgedNormalAlarms_sz;
	size_t shelvedAlarms_sz;
	size_t outOfServiceAlarms_sz;
	size_t silencedAlarms_sz;
	size_t listAlarms_sz;
	size_t alarmSummary_sz;

	double last_statistics_reset_time;

	omni_mutex *prepare_alm_mtx;

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::Data Members

//	Device property data members
public:
	//	GroupNames:	Labels for Group mask, first is for mask 0x00
	vector<string>	groupNames;
	//	SubscribeRetryPeriod:	Retry subscription period in seconds
	Tango::DevLong	subscribeRetryPeriod;
	//	StatisticsTimeWindow:	Time window to compute statistics in seconds
	vector<Tango::DevLong>	statisticsTimeWindow;

//	Attribute data members
public:
	Tango::DevBoolean	*attr_alarmAudible_read;
	Tango::DevDouble	*attr_StatisticsResetTime_read;
	Tango::DevString	*attr_alarm_read;
	Tango::DevString	*attr_alarmNormal_read;
	Tango::DevString	*attr_alarmUnacknowledged_read;
	Tango::DevString	*attr_alarmAcknowledged_read;
	Tango::DevString	*attr_alarmUnacknowledgedNormal_read;
	Tango::DevString	*attr_alarmShelved_read;
	Tango::DevString	*attr_alarmOutOfService_read;
	Tango::DevString	*attr_alarmSilenced_read;
	Tango::DevString	*attr_alarmList_read;
	Tango::DevDouble	*attr_alarmFrequency_read;
	Tango::DevString	*attr_alarmSummary_read;

//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	AlarmHandler(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	AlarmHandler(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	AlarmHandler(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~AlarmHandler() {delete_device();};


//	Miscellaneous methods
public:
	/*
	 *	will be called at device destruction or at init command.
	 */
	void delete_device();
	/*
	 *	Initialize the device
	 */
	virtual void init_device();
	/*
	 *	Read the device properties from database
	 */
	void get_device_property();
	/*
	 *	Always executed method before execution command method.
	 */
	virtual void always_executed_hook();


//	Attribute methods
public:
	//--------------------------------------------------------
	/*
	 *	Method      : AlarmHandler::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(vector<long> &attr_list);

/**
 *	Attribute alarmAudible related methods
 *	Description: True if there is at least one alarm that needs audible indication on the GUI
 *
 *	Data type:	Tango::DevBoolean
 *	Attr type:	Scalar
 */
	virtual void read_alarmAudible(Tango::Attribute &attr);
	virtual bool is_alarmAudible_allowed(Tango::AttReqType type);
/**
 *	Attribute StatisticsResetTime related methods
 *	Description: Time elapsed in seconds since last Resetstatistics
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
	virtual void read_StatisticsResetTime(Tango::Attribute &attr);
	virtual bool is_StatisticsResetTime_allowed(Tango::AttReqType type);
/**
 *	Attribute alarm related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 1024
 */
	virtual void read_alarm(Tango::Attribute &attr);
	virtual bool is_alarm_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmNormal related methods
 *	Description: List of alarms in normal state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmNormal(Tango::Attribute &attr);
	virtual bool is_alarmNormal_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmUnacknowledged related methods
 *	Description: List of alarms in unacknowledged state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmUnacknowledged(Tango::Attribute &attr);
	virtual bool is_alarmUnacknowledged_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmAcknowledged related methods
 *	Description: List of alarms in acknowledged state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmAcknowledged(Tango::Attribute &attr);
	virtual bool is_alarmAcknowledged_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmUnacknowledgedNormal related methods
 *	Description: List of alarms in unacknowledged normal state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmUnacknowledgedNormal(Tango::Attribute &attr);
	virtual bool is_alarmUnacknowledgedNormal_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmShelved related methods
 *	Description: List of alarms in shelved state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmShelved(Tango::Attribute &attr);
	virtual bool is_alarmShelved_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmOutOfService related methods
 *	Description: List of alarms in out of service state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmOutOfService(Tango::Attribute &attr);
	virtual bool is_alarmOutOfService_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmSilenced related methods
 *	Description: List of alarms in silenced state
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmSilenced(Tango::Attribute &attr);
	virtual bool is_alarmSilenced_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmList related methods
 *	Description: List of all alarms
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmList(Tango::Attribute &attr);
	virtual bool is_alarmList_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmFrequency related methods
 *	Description: List of frequency of evaluation of all alarms
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmFrequency(Tango::Attribute &attr);
	virtual bool is_alarmFrequency_allowed(Tango::AttReqType type);
/**
 *	Attribute alarmSummary related methods
 *	Description: 
 *
 *	Data type:	Tango::DevString
 *	Attr type:	Spectrum max = 10000
 */
	virtual void read_alarmSummary(Tango::Attribute &attr);
	virtual bool is_alarmSummary_allowed(Tango::AttReqType type);

//	Dynamic attribute methods
public:

	/**
	 *	Attribute AlarmState related methods
	 *	Description: 
	 *
	 *	Data type:	Tango::DevEnum
	 *	Attr type:	Scalar
	 */
	virtual void read_AlarmState(Tango::Attribute &attr);
	virtual bool is_AlarmState_allowed(Tango::AttReqType type);
	void add_AlarmState_dynamic_attribute(string attname);
	void remove_AlarmState_dynamic_attribute(string attname);
	Tango::DevEnum *get_AlarmState_data_ptr(string &name);
	map<string,Tango::DevEnum>	   AlarmState_data;

	/**
	 *	Attribute AlarmFormula related methods
	 *	Description: 
	 *
	 *	Data type:	Tango::DevString
	 *	Attr type:	Scalar
	 */
	virtual void read_AlarmFormula(Tango::Attribute &attr);
	virtual bool is_AlarmFormula_allowed(Tango::AttReqType type);
	void add_AlarmFormula_dynamic_attribute(string attname);
	void remove_AlarmFormula_dynamic_attribute(string attname);
	Tango::DevString *get_AlarmFormula_data_ptr(string &name);
	map<string,Tango::DevString>	   AlarmFormula_data;

	//--------------------------------------------------------
	/**
	 *	Method      : AlarmHandler::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:
	/**
	 *	Command Ack related method
	 *	Description: Alarm acknowledge
	 *
	 *	@param argin String array containing the alarms to be acknowledged
	 */
	virtual void ack(const Tango::DevVarStringArray *argin);
	virtual bool is_Ack_allowed(const CORBA::Any &any);
	/**
	 *	Command Load related method
	 *	Description: Load a new alarm.
	 *
	 *	@param argin Alarm entry
	 */
	virtual void load(Tango::DevString argin);
	virtual bool is_Load_allowed(const CORBA::Any &any);
	/**
	 *	Command Remove related method
	 *	Description: Remove alarm.
	 *
	 *	@param argin Alarm name
	 */
	virtual void remove(Tango::DevString argin);
	virtual bool is_Remove_allowed(const CORBA::Any &any);
	/**
	 *	Command SearchAlarm related method
	 *	Description: Return list of configured alarms matching the filter string
	 *
	 *	@param argin String containing a filter for output, if empty or * return all alarms
	 *	@returns Configured alarms
	 */
	virtual Tango::DevVarStringArray *search_alarm(Tango::DevString argin);
	virtual bool is_SearchAlarm_allowed(const CORBA::Any &any);
	/**
	 *	Command StopAudible related method
	 *	Description: Stop audible indications on the GUI
	 *
	 */
	virtual void stop_audible();
	virtual bool is_StopAudible_allowed(const CORBA::Any &any);
	/**
	 *	Command Silence related method
	 *	Description: Alarm temporarily silence
	 *
	 *	@param argin String array containing the alarms to be silenced
	 */
	virtual void silence(const Tango::DevVarStringArray *argin);
	virtual bool is_Silence_allowed(const CORBA::Any &any);
	/**
	 *	Command Modify related method
	 *	Description: Modify an existing alarm.
	 *
	 *	@param argin Alarm entry
	 */
	virtual void modify(Tango::DevString argin);
	virtual bool is_Modify_allowed(const CORBA::Any &any);
	/**
	 *	Command Shelve related method
	 *	Description: Shelve an alarm: no state transition recorded, no audible nor visual indication
	 *
	 *	@param argin String array containing alarm to be shelved
	 */
	virtual void shelve(const Tango::DevVarStringArray *argin);
	virtual bool is_Shelve_allowed(const CORBA::Any &any);
	/**
	 *	Command Enable related method
	 *	Description: Enable an alarm from Out of service state
	 *
	 *	@param argin Alarm name
	 */
	virtual void enable(Tango::DevString argin);
	virtual bool is_Enable_allowed(const CORBA::Any &any);
	/**
	 *	Command Disable related method
	 *	Description: Put an alarm in Out of service state
	 *
	 *	@param argin Alarm name
	 */
	virtual void disable(Tango::DevString argin);
	virtual bool is_Disable_allowed(const CORBA::Any &any);
	/**
	 *	Command ResetStatistics related method
	 *	Description: Reset statistics
	 *
	 */
	virtual void reset_statistics();
	virtual bool is_ResetStatistics_allowed(const CORBA::Any &any);
	/**
	 *	Command StopNew related method
	 *	Description: Stop audible indications on the GUI
	 *
	 */
	virtual void stop_new();
	virtual bool is_StopNew_allowed(const CORBA::Any &any);
	/**
	 *	Command GetAlarmInfo related method
	 *	Description: Returns the complete attribute info as an array of key=value
	 *
	 *	@param argin Alarm name followed optionally by wanted key names
	 *	@returns Complete attribute info as an array of key=value
	 */
	virtual Tango::DevVarStringArray *get_alarm_info(const Tango::DevVarStringArray *argin);
	virtual bool is_GetAlarmInfo_allowed(const CORBA::Any &any);
	/**
	 *	Command ReLoadAll related method
	 *	Description: Re Load all alarms.
	 *
	 */
	virtual void re_load_all();
	virtual bool is_ReLoadAll_allowed(const CORBA::Any &any);


	//--------------------------------------------------------
	/**
	 *	Method      : AlarmHandler::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(AlarmHandler::Additional Method prototypes) ENABLED START -----*/

//	Additional Method prototypes
friend class alarm_thread;
friend class SubscribeThread;
friend class event_table;

	alarm_table alarms;

protected :	

private:
	vector<alarm_t> stored;		/* property stored alarms (on exit) */
	event_table* events;
//	event_list evlist;				/* producer/consumer events list */		//gcc 4 problem??
//	EventCallBack ecb;				/* callback handles */
	alarm_thread *almloop;
	update_thread *updateloop;
	vector<alarm_t> alarmed;
	ReadersWritersLock *alarmedlock;
	vector<alarm_t> internal;
	ReadersWritersLock *internallock;
	ReadersWritersLock *dslock;
	int			period;		//subscribe thread period
	map<string, string> saved_alarms;
	
	static int instanceCounter;
	
	int internal_counter;
	int ds_num;
	Tango::DevString ds[MAX_ALARMS];
	char dss[MAX_ALARMS][10124];

	void init_events(vector<string> &evn);
#if 0
	void init_alarms(map< string,vector<string> > &alarm_events);
#endif
	void add_alarm(alarm_t& a, bool starting=false);
	void add_event(alarm_t& a, vector<string> &evn) throw(string&);
#if 0
	void subscribe_event(alarm_t& a, EventCallBack& ecb, vector<string> &evn) throw(string&);
#endif
//	void do_alarm(bei_t& e);											//gcc 4 problem??
	bool remove_alarm(string& s) throw(string&);
	//void add_to_database(alarm_t& a) throw(string&);
	void set_internal_alarm(string name, Tango::TimeVal t, string msg, unsigned int count=1);
	
	void load_alarm(string alarm_string, alarm_t &alm, vector<string> &evn);

	formula_res_t eval_formula(tree_parse_info_t tree, string &attr_values);
	void find_event_formula(tree_parse_info_t tree, vector<string> &);
	
	formula_res_t eval_expression(iter_t const& i, string &attr_values, int ev_ind=0);			//recursive tree node evaluation
	void eval_node_event(iter_t const& i, vector<string> & ev);		//recursive tree node evaluation		

	void prepare_alarm_attr();	//for read attribute alarm and push_change_event

	void remove_AlarmState_dynamic_attribute_no_clean_db(string attname);

	void parse_alarm(string &alarm_string, alarm_t &alm);

	SubscribeThread *thread;

public:
	void put_signal_property();
	void do_alarm(bei_t& e);											//public instead of protected for gcc 4 problem??
	bool do_alarm_eval(string alm_name, string ev_name, Tango::TimeVal ts);
	void timer_update();												//public instead of protected for gcc 4 problem??
	event_list evlist;				/* producer/consumer events list */		//public instead of protected for gcc 4 problem??
	bool abortflag;
	
#ifdef _USE_ELETTRA_DB_RW
private:
	string host_rw;
	long port_rw;
#endif

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::Additional Method prototypes
};

/*----- PROTECTED REGION ID(AlarmHandler::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions

/*----- PROTECTED REGION END -----*/	//	AlarmHandler::Additional Classes Definitions

}	//	End of namespace

#endif   //	AlarmHandler_H
