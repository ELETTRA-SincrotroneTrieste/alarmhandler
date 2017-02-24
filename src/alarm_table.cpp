/*
 * alarm_table.cpp
 *
 * $Author: graziano $
 *
 * $Revision: 1.5 $
 *
 * $Log: alarm_table.cpp,v $
 *
 *
 * copyleft: Sincrotrone Trieste S.C.p.A. di interesse nazionale
 *           Strada Statale 14 - km 163,5 in AREA Science Park
 *           34012 Basovizza, Trieste ITALY
 */

#include <sys/time.h>
#include <tango.h>
#include "alarm_table.h"
#include "alarm_grammar.h"
#include "log_thread.h"
#include "cmd_thread.h"


static const char __FILE__rev[] = __FILE__ " $Revision: 1.5 $";

/*
 * alarm_t class methods
 */
alarm_t::alarm_t()
{
	grp=0;
	counter=0;
	stat = S_NORMAL;
	ack = ACK;
} 
 
bool alarm_t::operator==(const alarm_t &that)
{
//	return((stat == that.stat) && (ack == that.ack));
	//verify if stat not equal but only contained because added *n in internal alarm
	return((stat.find(that.stat) != std::string::npos) && (ack == that.ack));		
}

bool alarm_t::operator==(const string &n)
{
	return(name == n);
}

void alarm_t::str2alm(const string &s)
{
	istringstream is(s);
	ostringstream temp_msg;
	string temp_grp;
	is >> ts.tv_sec >> ts.tv_usec >> name >> stat >> ack >> counter >> lev >> silent_time >> temp_grp >> msg;		//stop at first white space in msg
	temp_msg << is.rdbuf();		//read all remaining characters as msg
	msg += temp_msg.str();
	str2grp(temp_grp);
}

string alarm_t::alm2str(void)
{
	ostringstream os;
	os.clear();
	os << ts.tv_sec << "\t" << ts.tv_usec << "\t" << name << "\t" \
		 << stat << "\t" << ack << "\t" << counter << "\t" << lev << "\t" << silent_time << "\t" << grp2str() << "\t" << msg << ends;
	return(os.str());
}

map<string, unsigned int> alarm_t::grp_str;		//needed here because static
void alarm_t::init_static_map(vector<string> &group_names)
{
	//LOG_STREAM << "alarm_table::init_static_map(vector<string> &group_names): Entering..." << endl;
	int j=0;
	vector<string>::iterator i;
	if(grp_str.size() > 0)
		return;
	if(group_names.empty())
	{
		//LOG_STREAM << "alarm_table::init_static_map(): inserting: default group " << GR_NONE_NAME << " = " << showbase << hex << GR_NONE << endl;
		grp_str.insert(make_pair(string(GR_NONE_NAME), GR_NONE));	
	}	
	for (i = group_names.begin(); i != group_names.end(); i++)
	{
		if((*i) == string(GR_ALL_NAME))
			continue;
		if(i == group_names.begin())
		{
			//LOG_STREAM << "alarm_table::init_static_map(): inserting: group " << *i << " = " << showbase << hex << GR_NONE << endl;
			grp_str.insert(make_pair(*i, GR_NONE));
		}
		else
		{
			//LOG_STREAM << "alarm_table::init_static_map(): inserting: group " << *i << " = " << showbase << hex << int(0x1 << j) << endl;
			grp_str.insert(make_pair(*i, 0x1 << j));
			j++;
		}
	}
	//LOG_STREAM << "alarm_table::init_static_map(): inserting: group " << GR_ALL_NAME << " = " << showbase << hex << GR_ALL << endl;
	grp_str.insert(make_pair(string(GR_ALL_NAME), GR_ALL));
}

string alarm_t::grp2str(void)
{
	map<string, unsigned int>::iterator i = grp_str.begin();
	bool first=true;
	string argout;
	if(grp == GR_ALL)
		argout = string(GR_ALL_NAME);
	else if(grp == GR_NONE)
	{
		if(i != grp_str.end())
			argout = i->first;
		else
			argout = string(GR_NONE_NAME);		
	}
	else
	{
		for (; i != grp_str.end(); i++)
		{
			if(i->first == string(GR_ALL_NAME))
				continue;
			if(grp & i->second)
			{
				if(first)
				{
					argout = i->first;
					first = false;
				}
				else
					argout += string("|") + i->first;
			}
		}
	}
	return argout;
}

void alarm_t::add_grp_from_str(string &s)
{
	map<string, unsigned int>::iterator i = grp_str.find(s);
	if(i != grp_str.end())
		grp |= (*i).second;
}

void alarm_t::str2grp(string &s)
{
	for(map<string, unsigned int>::iterator i=grp_str.begin(); i != grp_str.end(); i++)
		if(s.find(i->first) != string::npos)
			grp |= i->second;
}

void alarm_t::insert(const string& s)
{
	s_event.insert(s);
}

void alarm_t::clear()
{
	name.clear();
	formula.clear();
	msg.clear();
	lev.clear();
	grp=0;
	s_event.clear();
	to_be_evaluated = false;
	stat.clear();
	ack.clear();
	done = false;
//	ts = 0;
}

/*
 * alarm_table class methods
 */
/* typedef std::string::iterator  iterator_t;
 typedef boost::spirit::node_val_data_factory<unsigned int> factory_t; /////TEMP!!!!!!!!!!!!!!!!!!!
 typedef boost::spirit::tree_match<iterator_t, factory_t> parse_tree_match_t;     
typedef boost::spirit::tree_parse_info<iterator_t, factory_t>    tree_parse_info_t;
*/
/*
void alarm_table::init(vector<string>& avs, vector<string> &evn, map< string,vector<string> > &alarm_event)
{
	//LOG_STREAM << "alarm_table::init(vector<string>& avs,map< string,vector<string> > &alarm_event): Entering..." << endl;
	alarm_t tmp_alm;
	LOG_STREAM << gettime().tv_sec << " " << __FILE__rev << endl;
	LOG_STREAM << gettime().tv_sec << " alarm_table::init(): Creating Spirit Parser..." << endl;
	alarm_parse al(tmp_alm);    //  Construct Spirit grammar

	if (avs.empty() == false) {
		for (vector<string>::iterator i = avs.begin(); \
				 i != avs.end(); i++) {

			tmp_alm.name.clear();
			tmp_alm.formula.clear();
			tmp_alm.msg.clear();
			tmp_alm.lev.clear();
			tmp_alm.grp=0;
			tree_parse_info_t info = ast_parse<factory_t>(i->begin(), i->end(), al, space_p);
			if (info.full)
			{
           		LOG_STREAM << gettime().tv_sec << " Parsing succeeded: " << tmp_alm.name << endl;
				for (vector<string>::iterator i = evn.begin(); i != evn.end(); i++) 
					LOG_STREAM << gettime().tv_sec << " READ Event! ->" << *i << endl;
			}	           		
       		else
        	{
				LOG_STREAM << gettime().tv_sec << " Parsing failed, stopped at: " << string(info.stop, i->end()) << ends; //TODO
        	}

			if ((tmp_alm.name.empty() == false) && \
					(tmp_alm.formula.empty() == false) && \
					((tmp_alm.lev==LEV_LOG)||(tmp_alm.lev==LEV_WARNING)|| \
					(tmp_alm.lev==LEV_FAULT)||(tmp_alm.lev.empty() == true))) 
			{
				tmp_alm.stat = S_NORMAL;
				tmp_alm.ack = ACK;
				tmp_alm.done = false;
				if(tmp_alm.grp == 0)
					tmp_alm.grp = GR_DEFAULT;
				if(tmp_alm.lev.empty() == true)
					tmp_alm.lev = LEV_DEFAULT;
				push_back(tmp_alm);
			} else {
				cerr << gettime().tv_sec << " alarm_table::init(): syntax error in '" << *i \
						 << "', skipping!" << endl;
			}
		}
	}
}*/

void alarm_table::push_back(alarm_t &a)
{
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->writerIn();
#endif
	//v_alarm.push_back(a);
	v_alarm.insert(make_pair(a.name,a));
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->writerOut();
#endif
}

void alarm_table::show(vector<string> &al_table_string)
{
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	ostringstream log_msg;
	string log_str;
	if (v_alarm.empty() == false) {
		log_msg << "### alarms table: ###" << ends;
		al_table_string.push_back(log_msg.str());
		log_msg.str(string());
		alarm_container_t::iterator i = v_alarm.begin();
		unsigned int j = 0;
		while (i != v_alarm.end()) {
			log_msg << j << " - name: '" << i->second.name << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());			
			log_msg << "    formula: '" << i->second.formula << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());
	  		log_msg << "    stat: '" << i->second.stat << "'" << ends;
	  		al_table_string.push_back(log_msg.str());
			log_msg.str(string());
			log_msg << "    ack: '" << i->second.ack << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());
			log_msg << "    msg: '" << i->second.msg << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());
			log_msg << "    grp: '" << showbase << hex << i->second.grp << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());
			log_msg << "    lev: '" << i->second.lev << "'" << ends;
			al_table_string.push_back(log_msg.str());
			log_msg.str(string());
			i++;
			j++;
		}
	}
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
}

unsigned int alarm_table::size(void)
{
	return(v_alarm.size());
}

alarm_container_t& alarm_table::get(void)
{
	return(v_alarm);
}

void alarm_table::stored(vector<alarm_t>& a)
{
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	if (a.empty() == false) {
		for (vector<alarm_t>::iterator i = a.begin(); i != a.end(); i++) 
		{
			alarm_container_t::iterator found = v_alarm.find(i->name);
			if (found != v_alarm.end()) {
				found->second.ts = i->ts;
				found->second.stat = i->stat;
				found->second.ack = i->ack;
				found->second.done = i->done;
				found->second.is_new = i->is_new;
			} else {
				/*
			 	 * shouldn't happen!!!
			   */
				LOG_STREAM << "alarm_table::stored(): " << i->name \
					 	 			 << " NOT found in alarm table" << endl;
			}
		}  /* for */
	}  /* if */
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
}

bool alarm_table::update(const string& alm_name, Tango::TimeVal ts, formula_res_t res, string &attr_values, string grp, string msg, string formula)
{
	bool ret_changed=false;
	//Tango::TimeVal now = gettime();
	TangoSys_MemStream out_stream;
	alm_log_t a;
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	alarm_container_t::iterator found = v_alarm.find(alm_name);
	if (found != v_alarm.end()) 
	{
		if(found->second.silenced > 0)
		{
			Tango::TimeVal now = gettime();
			double dnow = now.tv_sec + ((double)now.tv_usec) / 1000000;
			double dsilent = found->second.ts_time_silenced.tv_sec + ((double)found->second.ts_time_silenced.tv_usec) / 1000000;
			double dminutes = (dnow - dsilent)/60;
			if(dminutes < found->second.silent_time)
				found->second.silenced = found->second.silent_time - floor(dminutes);
			else
				found->second.silenced = 0;
		}
		found->second.quality = res.quality;
		found->second.ex_reason = res.ex_reason;
		found->second.ex_desc = res.ex_desc;
		found->second.ex_origin = res.ex_origin;
		bool status_time_threshold;
		if(found->second.time_threshold > 0)		//if enabled time threshold
			status_time_threshold = ((int)(res.value)) && (found->second.counter >= 1) && ((ts.tv_sec - found->second.time_threshold) > found->second.ts_time_threshold.tv_sec);	//formula gives true and time threshold is passed
		else
			status_time_threshold = (int)(res.value);
		//if status changed:
		// - from S_NORMAL to S_ALARM considering also time threshold
		//or
		// - from S_ALARM to S_NORMAL		
		if((status_time_threshold && (found->second.stat == S_NORMAL)) || (!(int)(res.value) && (found->second.stat == S_ALARM)))
		{
			ret_changed=true;
			a.type_log = TYPE_LOG_STATUS;
			a.name = alm_name;
			a.time_s = ts.tv_sec;		
			a.time_us = ts.tv_usec;
			a.status = (int)(res.value) ? S_ALARM : S_NORMAL;
			//a.level = found->second.lev;
			if((int)(res.value))
				found->second.ack = NOT_ACK;	//if changing from NORMAL to ALARM -> NACK
			a.ack = found->second.ack;
			a.values = attr_values;
			//a.grp = found->second.grp2str();
			//a.msg = (int)(res.value) ? found->second.msg : "";
			logloop->log_alarm_db(a);
			found->second.ts = ts;	/* store event timestamp into alarm timestamp */ //here update ts only if status changed
			if((int)(res.value))
			{
				found->second.is_new = 1;		//here set this alarm as new, read attribute set it to 0	//12-06-08: StopNew command set it to 0
				if(found->second.dp_a && ((ts.tv_sec - startup_complete.tv_sec) > 10))		//action from S_NORMAL to S_ALARM
				{
					/*try {
						long call_id;
						ostringstream tmp;
						tmp << alm_name << ";" << attr_values;
						Tango::DevString str = CORBA::string_dup(tmp.str().c_str());
						Tango::DeviceData Din;
						Din << str;
						CORBA::string_free(str);
						//found->second.dp_a->ping();
						cmdloop->mutex_dp->lock();
						//call_id = found->second.dp_a->command_inout_asynch(found->second.cmd_action_a, Din, true);		//true -> "fire and forget" mode: client do not care at all about the server answer
						call_id = found->second.dp_a->command_inout_asynch(found->second.cmd_action_a, Din);		//true -> "fire and forget" mode: client do not care at all about the server answer
						cmdloop->mutex_dp->unlock();
						LOG_STREAM << "alarm_table::update() executed action: " << found->second.cmd_name_a << " !!!" << endl;
						cmd_t arg;
						arg.cmd_id = call_id;
						arg.dp_add = (long)found->second.dp_a;
						arg.arg_s = found->second.cmd_name_a;	
						cmdloop->list.push_back(arg);						
					} catch(Tango::DevFailed e) 
					{
						string err(e.errors[0].desc);
						if(err.find("is not yet arrived") == string::npos)			//TODO: change this!!
							out_stream << "Failed to execute action " << found->second.cmd_name_a << ", err=" << e.errors[0].desc << ends;
						//LOG_STREAM << "alarm_table::update() ERROR: " << out_stream.str() << endl;
					}*/
					ostringstream tmp;
					string tmp_attr_val = attr_values;
					replace(tmp_attr_val.begin(), tmp_attr_val.end(), ';' , ',');
					string tmp_msg = msg;
					replace(tmp_msg.begin(), tmp_msg.end(), ';' , ',');
					tmp << "name=" << alm_name << ";groups=" << grp << ";msg="<<tmp_msg<<";values="<<tmp_attr_val<<";formula="<<formula;
					cmd_t arg;
					arg.cmd_id = CMD_COMMAND;
					arg.dp_add = (long)found->second.dp_a;
					arg.arg_s1 = tmp.str();
					arg.arg_s2 = found->second.cmd_action_a;
					arg.arg_s3 = found->second.cmd_name_a;
					arg.arg_b = found->second.send_arg_a;	
					cmdloop->list.push_back(arg);					
				}
			}
			else
			{
				if(found->second.dp_n && ((ts.tv_sec - startup_complete.tv_sec) > 10))		//action from S_ALARM to S_NORMAL 
				{
					/*try {
						long call_id;
						ostringstream tmp;
						tmp << alm_name << ";" << attr_values;
						Tango::DevString str = CORBA::string_dup(tmp.str().c_str());
						Tango::DeviceData Din;
						Din << str;
						CORBA::string_free(str);						
						//found->second.dp_n->ping();
						cmdloop->mutex_dp->lock();
						//call_id = found->second.dp_n->command_inout_asynch(found->second.cmd_action_n, Din, true);		//true -> "fire and forget" mode: client do not care at all about the server answer
						call_id = found->second.dp_n->command_inout_asynch(found->second.cmd_action_n, Din);				//true -> "fire and forget" mode: client do not care at all about the server answer
						cmdloop->mutex_dp->unlock();
						LOG_STREAM << "alarm_table::update() executed action: " << found->second.cmd_name_n << " !!!" << endl;
						cmd_t arg;
						arg.cmd_id = call_id;
						arg.dp_add = (long)found->second.dp_n;
						arg.arg_s = found->second.cmd_name_n;	
						cmdloop->list.push_back(arg);						
					} catch(Tango::DevFailed e) 
					{
						string err(e.errors[0].desc);
						if(err.find("is not yet arrived") == string::npos)			//TODO: change this!!
							out_stream << "Failed to execute action " << found->second.cmd_name_n << ", err=" << e.errors[0].desc << ends;
						//LOG_STREAM << "alarm_table::update() ERROR: " << out_stream.str() << endl;
					}*/
					ostringstream tmp;
					string tmp_attr_val = attr_values;
					replace(tmp_attr_val.begin(), tmp_attr_val.end(), ';' , ',');
					string tmp_msg = msg;
					replace(tmp_msg.begin(), tmp_msg.end(), ';' , ',');
					tmp << "name=" << alm_name << ";groups=" << grp << ";msg="<<tmp_msg<<";values="<<tmp_attr_val<<";formula="<<formula;
					cmd_t arg;
					arg.cmd_id = CMD_COMMAND;
					arg.dp_add = (long)found->second.dp_n;
					arg.arg_s1 = tmp.str();
					arg.arg_s2 = found->second.cmd_action_n;
					arg.arg_s3 = found->second.cmd_name_n;
					arg.arg_b = found->second.send_arg_n;
					cmdloop->list.push_back(arg);					
				}
			}
		}
		if (status_time_threshold) {
			found->second.stat = S_ALARM;
			//found->second.ack = NOT_ACK;
		}
		if((int)(res.value)) {
			found->second.counter++;
		} else {
			found->second.stat = S_NORMAL;
			found->second.counter = 0;
		}
		if(found->second.counter == 1)
			found->second.ts_time_threshold = gettime();		//first occurrance of this alarm, now begin to wait for time threshold
		if(found->second.counter >= 1)
			found->second.attr_values_time_threshold = attr_values;		//save last attr_values to be used in timer_update if this alarm pass over time threshold

		//found->second.ts = ts;	/* store event timestamp into alarm timestamp */ //here update ts everytime
	} else {
		/*
		 * shouldn't happen!!!!
		 */
		out_stream << "couldn't find alarm '" << alm_name << "' in 'alarms' table" << ends;
		LOG_STREAM << gettime().tv_sec << " alarm_table::update(): " << out_stream.str() << endl;
	}
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
	if(out_stream.str().length() > 0)
		throw out_stream.str();
	return ret_changed;
}

bool alarm_table::timer_update()
{
	bool ret_changed=false;
	Tango::TimeVal ts = gettime();
	TangoSys_MemStream out_stream;
	alm_log_t a;
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	for(alarm_container_t::iterator i = v_alarm.begin(); i != v_alarm.end(); i++)
	{		
		bool status_time_threshold;
		if(i->second.time_threshold > 0)		//if enabled time threshold
			status_time_threshold = (i->second.counter >= 1) && ((ts.tv_sec - i->second.time_threshold) > i->second.ts_time_threshold.tv_sec);	//waiting for threshold and time threshold is passed
		else
			continue;			//if not enabled time threshold, nothing to do in timer	

		//if status changed from S_NORMAL to S_ALARM considering also time threshold	
		if(status_time_threshold && (i->second.stat == S_NORMAL))
		{
			ret_changed = true;
			if(i->second.silenced > 0)
			{
				Tango::TimeVal now = gettime();
				double dnow = now.tv_sec + ((double)now.tv_usec) / 1000000;
				double dsilent = i->second.ts_time_silenced.tv_sec + ((double)i->second.ts_time_silenced.tv_usec) / 1000000;
				double dminutes = (dnow - dsilent)/60;
				if(dminutes < i->second.silent_time)
					i->second.silenced = i->second.silent_time - floor(dminutes);
				else
					i->second.silenced = 0;
			}

			a.type_log = TYPE_LOG_STATUS;
			a.name = i->second.name;
			a.time_s = ts.tv_sec;		
			a.time_us = ts.tv_usec;
			a.status = S_ALARM;
			//a.level = found->second.lev;
			i->second.ack = NOT_ACK;	//if changing from NORMAL to ALARM -> NACK
			a.ack = i->second.ack;
			a.values = i->second.attr_values_time_threshold;
			logloop->log_alarm_db(a);
			i->second.ts = ts;	/* store event timestamp into alarm timestamp */ //here update ts only if status changed

			i->second.is_new = 1;		//here set this alarm as new, read attribute set it to 0	//12-06-08: StopNew command set it to 0
			if(i->second.dp_a && ((ts.tv_sec - startup_complete.tv_sec) > 10))
			{
				/*try {
					long call_id;
					ostringstream tmp;
					tmp << i->second.name << ";" << i->second.attr_values_time_threshold;
					Tango::DevString str = CORBA::string_dup(tmp.str().c_str());
					Tango::DeviceData Din;
					Din << str;
					CORBA::string_free(str);
					//i->second.dp_a->ping();		
					cmdloop->mutex_dp->lock();			
					//call_id = i->second.dp_a->command_inout_asynch(i->second.cmd_action_a, Din, true);		//true -> "fire and forget" mode: client do not care at all about the server answer
					call_id = i->second.dp_a->command_inout_asynch(i->second.cmd_action_a, Din);		
					cmdloop->mutex_dp->unlock();
					LOG_STREAM << gettime().tv_sec << " alarm_table::timer_update() executed action: " << i->second.cmd_name_a << " !!!" << endl;
					cmd_t arg;
					arg.cmd_id = call_id;
					arg.dp_add = (long)i->second.dp_a;
					arg.arg_s = i->second.cmd_name_a;	
					cmdloop->list.push_back(arg);					
				} catch(Tango::DevFailed e) 
				{
					string err(e.errors[0].desc);
					if(err.find("is not yet arrived") == string::npos)			//TODO: change this!!			
						out_stream << "Failed to execute action " << i->second.cmd_name_a << ", err=" << e.errors[0].desc << ends;
					//LOG_STREAM << "alarm_table::timer_update() ERROR: " << out_stream.str() << endl;
				}*/
				ostringstream tmp;
				string tmp_attr_val = i->second.attr_values_time_threshold;
				replace(tmp_attr_val.begin(), tmp_attr_val.end(), ';' , ',');
				string tmp_msg = i->second.msg;
				replace(tmp_msg.begin(), tmp_msg.end(), ';' , ',');
				tmp << "name=" << i->second.name << ";groups=" << i->second.grp2str() << ";msg="<<tmp_msg<<";values="<<tmp_attr_val<<";formula="<<i->second.formula;
				cmd_t arg;
				arg.cmd_id = CMD_COMMAND;
				arg.dp_add = (long)i->second.dp_a;
				arg.arg_s1 = tmp.str();
				arg.arg_s2 = i->second.cmd_action_a;
				arg.arg_s3 = i->second.cmd_name_a;
				arg.arg_b = i->second.send_arg_a;	
				cmdloop->list.push_back(arg);
			}
			*(i->second.attr_value) = true;
			try
			{
				if(i->second.ex_reason.length() == 0)
				{
					timeval now;
					gettimeofday(&now, NULL);
					mydev->push_change_event(i->second.attr_name,(Tango::DevBoolean *)i->second.attr_value,now,(Tango::AttrQuality)i->second.quality, 1/*size*/, 0, false);
					mydev->push_archive_event(i->second.attr_name,(Tango::DevBoolean *)i->second.attr_value,now,(Tango::AttrQuality)i->second.quality, 1/*size*/, 0, false);
				}
				else
				{
					Tango::DevErrorList errors(1);
					errors.length(1);
					errors[0].desc = CORBA::string_dup(i->second.ex_desc.c_str());
					errors[0].severity = Tango::ERR;
					errors[0].reason = CORBA::string_dup(i->second.ex_reason.c_str());
					errors[0].origin = CORBA::string_dup(i->second.ex_origin.c_str());
					Tango::DevFailed except(errors);
					mydev->push_change_event(i->second.attr_name, &except);
					mydev->push_archive_event(i->second.attr_name, &except);
				}
			} catch(Tango::DevFailed &e)
			{}
		}
		if (status_time_threshold) {
			i->second.stat = S_ALARM;
			//found->second.ack = NOT_ACK;
		}
		//found->second.ts = ts;	/* store event timestamp into alarm timestamp */ //here update ts everytime
	}
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
	if(out_stream.str().length() > 0)
		throw out_stream.str();
	return ret_changed;
}

void alarm_table::erase(alarm_container_t::iterator i)
{
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->writerIn();
#endif
	v_alarm.erase(i);
#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->writerOut();
#endif
}

bool alarm_table::exist(string& s)
{
	alarm_container_t::iterator found = v_alarm.find(s);
	if (found != v_alarm.end())
		return true;
	else
		return false;
}

unsigned int alarm_table::to_be_evaluated_num()
{
	unsigned int ret=0;
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	for(alarm_container_t::iterator i = v_alarm.begin(); i != v_alarm.end(); i++)
	{
		if(i->second.to_be_evaluated == true)
			ret++;
	}

#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
	return ret;
}

vector<string> alarm_table::to_be_evaluated_list()
{
	vector<string> ret;
#ifndef _RW_LOCK
	this->lock();
#else
	vlock->readerIn();
#endif
	for(alarm_container_t::iterator i = v_alarm.begin(); i != v_alarm.end(); i++)
	{
		if(i->second.to_be_evaluated == true)
			ret.push_back(i->first);
	}

#ifndef _RW_LOCK
	this->unlock();
#else
	vlock->readerOut();
#endif
	return ret;
}

#ifdef _RW_LOCK
void alarm_table::new_rwlock()
{
	vlock = new(ReadersWritersLock);
}
void alarm_table::del_rwlock()
{
	delete vlock;
}
#endif

void alarm_table::init_logdb(string dbhost, string dbuser, string dbpw, string dbname, int dbport, string instance_name)
{
	logloop = new log_thread(dbhost, dbuser, dbpw, dbname, dbport, instance_name);
	logloop->start();
}

void alarm_table::stop_logdb()
{
	alm_log_t a;
	a.name = LOG_THREAD_EXIT;
	a.time_s = LOG_THREAD_EXIT_TIME;
	logloop->log_alarm_db(a);
	//sleep(1);
	//delete logloop;	
}

void alarm_table::init_cmdthread()
{
	cmdloop = new cmd_thread();
	cmdloop->start();
}

void alarm_table::stop_cmdthread()
{
	cmd_t arg;
	//arg.arg_s = CMD_THREAD_EXIT;
	arg.cmd_id = CMD_THREAD_EXIT;
	cmdloop->list.push_back(arg);	
}

void alarm_table::log_alarm_db(unsigned int type, Tango::TimeVal ts, string name, string status, string ack, 
		string formula, unsigned int time_threshold, string grp, string lev, string msg, string action, int silent_time, vector<string> alm_list)
{
	alm_log_t a;
	a.type_log = type;
	a.name = name;
	a.time_s = ts.tv_sec;	
	a.time_us = ts.tv_usec;
	a.time_threshold = time_threshold;
	a.status = status;
	a.level = lev;
	a.ack = ack;
	a.grp = grp;
	a.msg = msg;
	a.action = action;
	a.formula = formula;
	a.alm_list = alm_list;
	a.silent_time = silent_time;
	logloop->log_alarm_db(a);	
}

void alarm_table::save_alarm_conf_db(string att_name, Tango::TimeVal ts, string name, string status, string ack,
		string formula, unsigned int time_threshold, string grp, string lev, string msg, string action, int silent_time, vector<string> alm_list)
{
	// We want to put properties for attribute "att_name"
	Tango::DbDatum dbd_att_name(att_name);
	Tango::DbDatum dbd_name("name");
	Tango::DbDatum dbd_formula("formula");
	Tango::DbDatum dbd_time_threshold("time_threshold");
	Tango::DbDatum dbd_level("level");
	Tango::DbDatum dbd_silence_time("silence_time");	//TODO: silent_time
	Tango::DbDatum dbd_group("group");
	Tango::DbDatum dbd_message("message");
	Tango::DbDatum dbd_command("command");	//TODO: action

	Tango::DbData db_data;
	dbd_att_name << 8;                               // Eigth properties for attribute "att_name"
	dbd_name << name;
	dbd_formula << formula;
	dbd_time_threshold << time_threshold;
	dbd_level << lev;
	dbd_silence_time << silent_time;
	dbd_group << grp;
	dbd_message << msg;
	dbd_command << action;

	db_data.push_back(dbd_att_name);
	db_data.push_back(dbd_name);
	db_data.push_back(dbd_formula);
	db_data.push_back(dbd_time_threshold);
	db_data.push_back(dbd_level);
	db_data.push_back(dbd_silence_time);
	db_data.push_back(dbd_group);
	db_data.push_back(dbd_message);
	db_data.push_back(dbd_command);

	string dev_name(mydev->get_name());

	Tango::DbDevice *db_dev = mydev->get_db_device();
	db_dev->get_dbase()->put_device_attribute_property(dev_name,db_data);
}

void alarm_table::get_alarm_list_db(vector<string> &al_list)
{
	//logloop->get_alarm_list(al_list);
/*
 * 			" CONCAT('\t', " << DESC_COL_NAME << ",'\t'," << DESC_COL_FORMULA <<
				",'\t'," << "IFNULL(" << DESC_COL_TIME_THRESHOLD << ",0)" << ",'\t'," << DESC_COL_LEVEL <<
				",'\t'," << "IFNULL(" << DESC_COL_SILENT_TIME << ",-1)" <<
				",'\t'," << DESC_COL_GRP <<	",'\t','\"'," << "IFNULL(" << DESC_COL_MSG << ",'')" <<
				",'\"','\t'," << "IFNULL(" << DESC_COL_ACTION << ",';')" << ")" <<
 */


	string dev_name(mydev->get_name());
	vector<string> att_list;

	Tango::DbDevice *db_dev = mydev->get_db_device();
	db_dev->get_dbase()->get_device_attribute_list(dev_name,att_list);
	Tango::DbData db_data;
	vector<string>::iterator it;
	for(it = att_list.begin(); it!=att_list.end(); it++)
	{
		db_data.push_back(Tango::DbDatum(*it));
	}

	db_dev->get_attribute_property(db_data);

	for (size_t i=0;i < db_data.size();/*i++*/)
	{
		Tango::DevLong64 nb_prop;
		string &att_name = db_data[i].name;
		db_data[i] >> nb_prop;
		i++;
		string alm_name;
		string alm_formula;
		string alm_time_threshold("0");
		string alm_level;
		string alm_silence_time("-1");
		string alm_group;
		string alm_message;
		string alm_command(";");
		for (long k=0;k < nb_prop;k++)
		{
			string &prop_name = db_data[i].name;

			if (prop_name == "name")
				db_data[i] >> alm_name;
			else if (prop_name == "formula")
				db_data[i] >> alm_formula;
			else if (prop_name == "time_threshold")
				db_data[i] >> alm_time_threshold;
			else if (prop_name == "level")
				db_data[i] >> alm_level;
			else if (prop_name == "silence_time")
				db_data[i] >> alm_silence_time;
			else if (prop_name == "group")
				db_data[i] >> alm_group;
			else if (prop_name == "message")
				db_data[i] >> alm_message;
			else if (prop_name == "command")
				db_data[i] >> alm_command;
			else
			{
				cout << "att_name="<<att_name<<" UNKWNOWN prop_name="<<prop_name<<endl;
				i++;
				continue;
			}
			i++;
		}
		stringstream alm;
		alm << alm_name << "\t" << alm_formula << "\t" << alm_time_threshold << "\t" << alm_level <<
				"\t" << alm_silence_time << "\t" << alm_group << "\t\"" << alm_message <<	"\"\t" << alm_command;
		al_list.push_back(alm.str());
	}


#if 0

	db_dev->get_attribute_property(db_data);

	//Tango::Util *tg = Tango::Util::instance();
	//tg->get_database()->get_device_attribute_property("id11/motor/1", db_data);
	Tango::Util::instance()->get_database()->get_device_attribute_property("id11/motor/1", db_data);
	//mydev->get_device_attr();
#endif
}

/* EOF */
