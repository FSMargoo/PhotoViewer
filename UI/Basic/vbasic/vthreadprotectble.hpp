/*
 * VThreadProtectble.hpp
 *	@description : Provide a Thread Safly API
 *	@birth		 : 2022/6.3
*/
#pragma once

#error The VThreadProtectble feature has been deprecated, dont use this anymore

#include "vbase.hpp"

#include <map>

VLIB_BEGIN_NAMESPACE

/*
 * Local Object's Thread Stats
*/
enum class VThreadStats {
	IN_BUSY, FREE
};

/* The Thread Working Handle */
using VThreadWorkHandle = ULONG;

/*
 * VThreadProtectbleObject class:
 *	@description : The Thread Safe API in VLib
*/
class VThreadProctectbleObject final {
private:
	VThreadStats _ThreadStats;
	std::map<VThreadWorkHandle, VThreadStats> 
			     _ThreadJob;

	VThreadWorkHandle 
				 _ThreadHandlePool = 0;

	/*
	 * Set the Object Thread Stats
	*/
	void SetThreadStats(VThreadStats stats) {
		_ThreadStats = stats;
	}

public:
	VThreadProctectbleObject() {
		_ThreadStats = VThreadStats::FREE;
	}

	/*
	 * GetThreadStats Functional:
	 *	@description : Return the Thread Stasts
	*/
	VThreadStats GetThreadStats() const {
		return _ThreadStats;
	}

public:
	/*
	 * GetLocalWorkingJobHandle Functional:
	 *	@description  : Every job own their handle, this functioanl return
	 *					the handle of working job
	 *	@return value : Job Handle
	*/
	VThreadWorkHandle GetLocalWorkingJobHandle() const {
		return _ThreadHandlePool;
	}

	/*
	 * GetJobStats Functional:
	 *	@description  ： This Functional return the stats of working job
	 *	@return value  : Job working stats
	*/
	VThreadStats GetJobStats(const VThreadWorkHandle job_handle) {
		if (_ThreadJob.find(job_handle) != _ThreadJob.end()) {
			return _ThreadJob[job_handle];
		}

		return VThreadStats::FREE;
	}

	/*
	 * StartJob Functional:
	 *	@description  : When a functional startup better call this functinal
	 *	@return value : Is the object able to start a job (In busy or not)
	*/
	bool StartJob(VThreadWorkHandle& job_handle) {
		/* If object in busy, it couldn't start a job */
		if (GetThreadStats() == VThreadStats::IN_BUSY) {
			job_handle = 0x0;

			return false;
		}

		SetThreadStats(VThreadStats::IN_BUSY);

		++_ThreadHandlePool;
		job_handle = _ThreadHandlePool;

		_ThreadJob.insert(std::pair<VThreadWorkHandle, VThreadStats>
			(_ThreadHandlePool, VThreadStats::IN_BUSY));

		return true;
	}
	/*
	*  EndJob Functional:
	*	@description : Use This Functional to Stop a Job
	*/
	void EndJob(const VThreadWorkHandle job_handle) {
		if (GetJobStats(job_handle) == VThreadStats::IN_BUSY) {
			_ThreadJob.erase(job_handle);
		}
	}
	
};

VLIB_END_NAMESPACE