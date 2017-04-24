
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

bool db_get_purch(connection* C, std::string status, std::string pur_id, std::vector<std::unordered_map<std::string, std::string> >& map_array );
void db_get_hid(connection* C, std::string pid, std::vector<std::unordered_map<std::string, int> >& map_array);
bool db_add_ship_temp(connection* C, std::string wid, std::string hid, std::string pid, std::string num_buy, std::string cid);
std::string get_tids_by_same_hid_cid(connection* C, std::string hid, std::string cid);
void db_add_shipments(connection* C, std::string wid, std::string cid, int wh_count);
bool db_add_ship(connection* C, std::string wid,std::string hid, std::string cid, std::string tid_list);
