#pragma once
#include <string>
#include <map>
class Clock;

enum ProfilingCategory {
	DEFAULT,
	RENDERING,
	UPDATE
};


struct ProfileStats {
	double m_startTime;
	double m_endTime;

	double GetElapsedTimeMilliseconds() {
		return (m_endTime - m_startTime) * 1000;
	}

	ProfileStats() : 
		m_startTime(0), 
		m_endTime(0) {}
};


struct CategoryProfileStats {
	double m_averageTime;
	double m_totalTimeMilliseconds;
	int m_numProfiled;

	CategoryProfileStats() : 
		m_averageTime(0),
		m_totalTimeMilliseconds(0),
		m_numProfiled(0) {}
};

class ProfileSection
{
public:
	ProfileSection(const std::string& name, bool scopedLifetime=true, ProfilingCategory pc=DEFAULT);
	static void RemoveProfileSection(const std::string& name);
	static ProfileStats GetProfileStatsForName(const std::string& name);
	static std::string GetNameForCategory(ProfilingCategory cat);

	static std::map<ProfilingCategory, CategoryProfileStats>& GetAllCategoryStats();
	static CategoryProfileStats& GetProfileStatsForCategory(ProfilingCategory pc);
	void StartProfiling();
	void StopProfiling();

	~ProfileSection();

private:
	ProfilingCategory m_profilingCategory;
	ProfileStats m_profileStats;
	std::string m_name;
	static std::map<std::string, ProfileStats> s_profileNameStatsMap;
	static std::map<ProfilingCategory, CategoryProfileStats> s_profileCategoryStatsMap;

	bool m_scopedLifetime;
};

