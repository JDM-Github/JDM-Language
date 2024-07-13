#pragma once

class JDM_DLL StringHigherFunctions 
{
public:
	enum StringFunction
	{
		string_size,
		string_replace,
		string_rreplace,
		string_concat,
		string_rconcat,
		string_to_upper,
		string_to_lower,
		string_rto_upper,
		string_rto_lower,
		string_reverse,
		string_rreverse,
		string_compare,
		string_substr,
		string_rsubstr,
		string_find,
		string_count_occurrences,
		string_is_empty,
		string_starts_with,
		string_ends_with,
		string_insert,
		string_erase,
		string_rinsert,
		string_rerase,
		string_trim,
		string_rtrim,
		string_tokenize,
		string_split,
		string_join,
		string_rjoin,
		string_to_list
	};
	JDM_DLL static std::unordered_map<std::string, StringFunction> stringFunctions;

	JDM_DLL static const std::shared_ptr<HigherObject> manageFunction(
		StringHigherFunctions::StringFunction stringFuncType,
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const int size(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const bool empty(std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const std::vector<std::shared_ptr<HigherObject>> to_list(std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const int find(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &findStr);
	JDM_DLL static const int count(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &findStr);

	JDM_DLL static const std::vector<std::shared_ptr<HigherObject>> tokenize(
		std::shared_ptr<HigherObject> &obj1, const std::string &regexPattern = "\\s+|,\\s*");

	JDM_DLL static const std::vector<std::shared_ptr<HigherObject>> split(
		const std::shared_ptr<HigherObject> &obj1, const std::string &stringObj = " ");

	JDM_DLL static const void join(
		const std::shared_ptr<HigherObject> &obj1,
		const std::shared_ptr<HigherObject> &listObj,
		const std::string &stringObj = "");
	JDM_DLL static const std::string rjoin(
		const std::shared_ptr<HigherObject> &obj1,
		const std::shared_ptr<HigherObject> &listObj,
		const std::string &stringObj = "");

	JDM_DLL static const bool startsWith(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &prefix);
	JDM_DLL static const bool endsWith(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &suffix);

	JDM_DLL static const void insert(std::shared_ptr<HigherObject> &obj1,
		int start, const std::shared_ptr<HigherObject> &insertStr);
	JDM_DLL static const void erase(std::shared_ptr<HigherObject> &obj1,
		int start, int end);
	JDM_DLL static const std::string rinsert(std::shared_ptr<HigherObject> &obj1,
		int start, const std::shared_ptr<HigherObject> &insertStr);
	JDM_DLL static const std::string rerase(std::shared_ptr<HigherObject> &obj1,
		int start, int end);

	JDM_DLL static const void trim(std::shared_ptr<HigherObject> &obj1,
		int start, int end);
	JDM_DLL static const std::string rtrim(std::shared_ptr<HigherObject> &obj1,
		int start, int end);

	JDM_DLL static const void replace(
		std::shared_ptr<HigherObject> &obj1,
		const std::shared_ptr<HigherObject> &from,
		const std::shared_ptr<HigherObject> &to);

	JDM_DLL static const std::string rreplace(
		std::shared_ptr<HigherObject> &obj1,
		const std::shared_ptr<HigherObject> &from,
		const std::shared_ptr<HigherObject> &to);

	JDM_DLL static const void concat(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static const std::string rconcat(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const void        to_lower(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::string rto_lower(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const void        to_upper(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::string rto_upper(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const bool        compare(std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const void reverse(
		std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::string rreverse( std::shared_ptr<HigherObject> &obj1 );
	JDM_DLL static const void substr( std::shared_ptr<HigherObject> &obj1, int start, int end );

	JDM_DLL static const std::string rsubstr( std::shared_ptr<HigherObject> &obj1, int start, int end );
};
