
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <pqxx/pqxx>
#include <cstring>
#include <iterator>
#include <cstdio>
#include <cerrno>
#include <cstdlib>

using namespace pqxx;

connection* create_connection();

void db_get_purch(connection* C, std::string status, std::vector<std::unordered_map<std::string, std::string> >& map_array );
void db_get_hid(connection* C, std::string pid, std::vector<std::unordered_map<std::string, int> >& map_array);
