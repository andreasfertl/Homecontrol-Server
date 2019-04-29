#pragma once
#include <string>

class ArctechSwitch
{

public:
	ArctechSwitch(int house, int unit, int group, bool on) : m_House(house), m_Unit(unit), m_Group(group), m_On(on) {};

	int GetHouse() const{ return m_House; };
	int GetUnit() const { return m_Unit; };
	int GetGroup() const { return m_Group; };
	int GetOn() const { return m_On; };

private:
	int m_House;                    //House Code
	int m_Unit;                     //Unit Code
	int m_Group;                    //Group
									//these 3 makes a arctech switch unique
	bool m_On;                      //true = ON / false = OFF


};

