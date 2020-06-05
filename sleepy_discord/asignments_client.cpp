#include "client.h"

namespace SleepyDiscord {
	void AssignmentBasedDiscordClient::resumeMainLoop() {
		//assignments should be ordered based on dueTime
		for (std::forward_list<Assignment>::iterator
			assignment = assignments.begin(); assignment != assignments.end(); assignment = assignments.begin())
			if (assignment->dueTime <= getEpochTimeMillisecond())
				doAssignment();
			else
				break;
	}

	void AssignmentBasedDiscordClient::doAssignment() {
		Assignment assignment = assignments.front();
		assignment.function();
		assignments.pop_front();
	}

	Timer AssignmentBasedDiscordClient::schedule(TimedTask code, const time_t milliseconds) {
		const time_t millisecondsEpochTime = getEpochTimeMillisecond() + milliseconds;

		static unsigned int uniqueNumber = 0;
		const int newJobID = ++uniqueNumber;

		const Assignment newAssignemt = {
			newJobID,
			code,
			millisecondsEpochTime
		};

		if (assignments.begin() == assignments.end() || millisecondsEpochTime < assignments.front().dueTime) {
			assignments.push_front(newAssignemt);
		} else {
			std::forward_list<Assignment>::iterator lastAssignment = assignments.begin();
			for (std::forward_list<Assignment>::iterator
				assignment = ++(assignments.begin());
				assignment != assignments.end();
				++assignment) {
				if (millisecondsEpochTime < assignment->dueTime)
					break;
				lastAssignment = assignment;
			}
			assignments.insert_after(lastAssignment, newAssignemt);
		}

		return Timer(
			std::bind(&AssignmentBasedDiscordClient::unschedule, this, newJobID)
		);
	}

	void AssignmentBasedDiscordClient::unschedule(const int jobID) {
		std::forward_list<Assignment>::iterator
			lastAssignment = assignments.begin();
		for (std::forward_list<Assignment>::iterator
			assignment = assignments.begin(); assignment != assignments.end();
			++assignment) {
			if (assignment->jobID == jobID) {
				assignments.erase_after(lastAssignment);
				break;
			}
			lastAssignment = assignment;
		}
	}
}

