#pragma once
#include"page_setting.h"
#include"problem_base.h"

using namespace std;

namespace OGE {

	class HtmlGenerator {
		PageSettings page_setting;
		vector<ProblemBase> all_problems;
		string html_page;
	public:
		HtmlGenerator(PageSettings page_setting): 
			page_setting(page_setting){}

	};



}