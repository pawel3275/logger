// #pragma once
#ifndef _Shared_H
#define _Shared_H

#include <string>
#include <map>

using namespace std;

// Structure is used in multiple object, hence declared here.
// It provides easier acces for them.
// struct date represents each node od date elements
struct date
{
	string year;
	string month;
	map <string, int> day;
};

#endif
