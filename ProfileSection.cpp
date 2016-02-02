#include "ProfileSection.hpp"
#include "Clock.hpp"
#include "Utilities\Time.hpp"

std::map<std::string, ProfileStats> ProfileSection::s_profileNameStatsMap;
std::map<ProfilingCategory, CategoryProfileStats> ProfileSection::s_profileCategoryStatsMap;

ProfileSection::ProfileSection(const std::string& name, bool scopedLifetime/*=true*/, ProfilingCategory pc/*=DEFAULT*/) :
m_name(name),
m_scopedLifetime(scopedLifetime),
m_profilingCategory(pc)
{
	ProfileSection::GetProfileStatsForCategory(m_profilingCategory);

	if (m_scopedLifetime) {
		m_profileStats.m_startTime = GetAbsoluteTimeSeconds();
	}

}

void ProfileSection::RemoveProfileSection(const std::string& name) {
	s_profileNameStatsMap.erase(s_profileNameStatsMap.find(name));
}

ProfileStats ProfileSection::GetProfileStatsForName(const std::string& name) {
	return s_profileNameStatsMap.find(name)->second;
}

std::string ProfileSection::GetNameForCategory(ProfilingCategory cat)
{
	switch (cat) {
	case DEFAULT:
		return "Default";
		break;
	case RENDERING:
		return "Rendering";
		break;
	case UPDATE:
		return "Update";
		break;
	}

	return "InvalidCategory";
}

std::map<ProfilingCategory, CategoryProfileStats>& ProfileSection::GetAllCategoryStats() {
	return s_profileCategoryStatsMap;
}

CategoryProfileStats& ProfileSection::GetProfileStatsForCategory(ProfilingCategory pc) {

	auto it = s_profileCategoryStatsMap.find(pc);
	if (it == s_profileCategoryStatsMap.end()) {
		s_profileCategoryStatsMap[pc] = CategoryProfileStats();
	}

	return s_profileCategoryStatsMap.find(pc)->second;
}

void ProfileSection::StartProfiling() {
	m_profileStats.m_startTime = GetAbsoluteTimeSeconds();
}

void ProfileSection::StopProfiling() {
	m_profileStats.m_endTime = GetAbsoluteTimeSeconds();
	ProfileSection::s_profileNameStatsMap[m_name] = m_profileStats;
}


ProfileSection::~ProfileSection()
{
	if (m_scopedLifetime) {
		m_profileStats.m_endTime = GetAbsoluteTimeSeconds();
		ProfileSection::s_profileNameStatsMap[m_name] = m_profileStats;
		CategoryProfileStats& cps = ProfileSection::GetProfileStatsForCategory(m_profilingCategory);
		cps.m_numProfiled++;
		cps.m_totalTimeMilliseconds += m_profileStats.GetElapsedTimeMilliseconds();
		cps.m_averageTime = (cps.m_totalTimeMilliseconds / cps.m_numProfiled);
		ProfileSection::s_profileCategoryStatsMap[m_profilingCategory] = cps;
	}
}