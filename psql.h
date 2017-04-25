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

bool db_add_ship(connection* C, std::string wid,std::string hid, std::string cid, std::string tid_list);

bool  db_get_ship_topack(connection* C, int sockfd, std::string sid, std::string status, uint32_t& whnum, std::vector<std::unordered_map<std::string, std::string> >&prods, uint64_t& shipid);

bool db_get_ship_topack(
connection* C,
  std::string sid,
  std::string status,
  uint32_t& whnum,
  std::vector<std::unordered_map<std::string, std::string> >&prods,
uint64_t& shipid);

bool db_add_stock(connection* C,std::string wid, std::string whnum, std::string pid, std::string num, std::string descr);


void db_add_shipments(connection* C, std::string wid, std::string cid, int wh_count, std::vector<std::unordered_map<std::string, int> >& ship_maps, std::vector<std::vector<std::unordered_map<std::string, std::string> > >& prod_maps);

